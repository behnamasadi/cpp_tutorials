# Smart Pointers

- [1. Overview](#1-overview)
- [2. std::unique_ptr](#2-stdunique_ptr)
  - [2.1. Construction](#21-construction)
  - [2.2. Move-Only Semantics](#22-move-only-semantics)
  - [2.3. release() vs reset()](#23-release-vs-reset)
  - [2.4. Custom Deleters](#24-custom-deleters)
- [3. std::shared_ptr](#3-stdshared_ptr)
  - [3.1. Control Block](#31-control-block)
  - [3.2. make_shared vs new](#32-make_shared-vs-new)
  - [3.3. Inspecting and Resetting](#33-inspecting-and-resetting)
  - [3.4. enable_shared_from_this](#34-enable_shared_from_this)
  - [3.5. Cyclic References](#35-cyclic-references)
- [4. std::weak_ptr](#4-stdweak_ptr)
  - [4.1. Detecting a Dead Object](#41-detecting-a-dead-object)
  - [4.2. Breaking Cycles](#42-breaking-cycles)
- [5. Checking for Null](#5-checking-for-null)
- [6. Pointer Casting](#6-pointer-casting)
- [7. Atomic Smart Pointers (C++20)](#7-atomic-smart-pointers-c20)
- [8. See Also](#8-see-also)

---

# 1. Overview

A smart pointer is a class that owns a heap object and frees it automatically. The C++ standard library provides three:

| Type | Ownership | Cost | Use when |
|---|---|---|---|
| `std::unique_ptr<T>` | Sole, transferable | Same as a raw pointer | Default choice for owning a heap resource. |
| `std::shared_ptr<T>` | Shared via reference count | Two pointers + atomic refcount | Multiple parts of the program genuinely co-own the object. |
| `std::weak_ptr<T>` | None — observes a `shared_ptr` | Same as `shared_ptr` | Break cycles; check whether a shared object is still alive. |

**Default to `unique_ptr`.** Reach for `shared_ptr` only when ownership truly is shared. See [shared_ptr_use_cases.md](shared_ptr_use_cases.md) for concrete patterns where shared ownership is justified.

# 2. std::unique_ptr

A `unique_ptr<T>` owns exactly one `T` allocation. When the `unique_ptr` is destroyed (or reset, or moved-from), the `T` is destroyed too. It has the same memory footprint as a raw pointer.

## 2.1. Construction

```cpp
#include <memory>

// ✅ Preferred — exception-safe, single allocation possible for shared_ptr
auto p = std::make_unique<Person>("Alice", 30);

// Also valid, but two-step and not exception-safe in expressions
std::unique_ptr<Person> q(new Person("Bob", 25));

// Empty
std::unique_ptr<Person> r;          // r == nullptr
```

`std::make_unique` (C++14) is the default; it forwards arguments to `T`'s constructor.

## 2.2. Move-Only Semantics

A `unique_ptr` cannot be copied — copying would create two owners, defeating the point.

```cpp
auto a = std::make_unique<Person>();
auto b = a;                  // ❌ compile error: deleted copy ctor
auto c = std::move(a);       // ✅ ownership moves; a is now empty
```

This makes `unique_ptr` cheap to pass around: pass by value and `std::move` to transfer ownership.

## 2.3. release() vs reset()

| Method | What it does |
|---|---|
| `p.reset()` | Deletes the owned object, sets `p` to `nullptr`. |
| `p.reset(q)` | Deletes the old object, then takes ownership of raw pointer `q`. |
| `p.release()` | **Does not delete.** Returns the raw pointer and sets `p` to `nullptr`. Caller now owns it. |
| `p.get()` | Returns the raw pointer **without** giving up ownership. Don't `delete` it. |

```cpp
auto u = std::make_unique<int>(42);

int* raw = u.release();   // u is empty; YOU now own *raw
delete raw;               // must delete manually

u.reset(new int(7));      // u now owns a fresh int
u.reset();                // deletes the int, u is empty
```

`release()` is rare in modern code — usually means you're handing ownership to a C API.

## 2.4. Custom Deleters

`unique_ptr` accepts a deleter as a second template argument. This makes it perfect for managing C resources:

```cpp
auto file = std::unique_ptr<FILE, decltype(&fclose)>(fopen("data.txt", "r"), &fclose);
//                          ^^^^^   ^^^^^^^^^^^^^^^^                          ^^^^^^^
//                          managed type    deleter type                      deleter

// fclose is called automatically when `file` goes out of scope
```

Or with a lambda:

```cpp
auto window = std::unique_ptr<SDL_Window, void(*)(SDL_Window*)>(
    SDL_CreateWindow(...), SDL_DestroyWindow);
```

Note: the deleter type is part of the `unique_ptr`'s type. A `unique_ptr<FILE, decltype(&fclose)>` is *not* the same type as `unique_ptr<FILE>`.

# 3. std::shared_ptr

A `shared_ptr<T>` participates in a reference-counted lifetime. Every copy bumps the count; the last to be destroyed deletes the object.

```cpp
auto a = std::make_shared<Person>("Alice");
auto b = a;                                    // refcount = 2
{
    auto c = a;                                // refcount = 3
}                                              // refcount = 2 again
// when a and b both go out of scope, Person is deleted
```

## 3.1. Control Block

A `shared_ptr` is two pointers wide: one to the object, one to a heap-allocated **control block**:

```
shared_ptr A ─┐                                ┌─ shared_ptr B
              │                                │
              ├─► Object ◄──────────────────────┤
              │                                │
              └─► Control Block ◄───────────────┘
                  ├── strong refcount
                  ├── weak   refcount
                  ├── deleter
                  └── allocator
```

Every copy of the `shared_ptr` shares the same control block. The block lives until the last `weak_ptr` to it is gone (so weak pointers can check if the object is dead).

## 3.2. make_shared vs new

```cpp
// ✅ Preferred — single allocation for both object and control block
auto p = std::make_shared<Person>("Alice");

// Two allocations: one for Person, another for the control block
auto q = std::shared_ptr<Person>(new Person("Bob"));
```

`make_shared` is faster (one allocation, better cache locality) and exception-safe. The downside: the object's memory isn't freed until the **last `weak_ptr`** is gone, because the control block lives in the same allocation. If you have weak pointers far outliving the object and the object is large, prefer the two-allocation form.

## 3.3. Inspecting and Resetting

```cpp
auto p = std::make_shared<int>(42);
std::cout << p.use_count();    // 1 — number of shared_ptrs to this object
p.reset();                     // deletes if this was the last owner
p.reset(new int(7));           // owns a new int now
```

`use_count()` is approximate in multithreaded code — by the time you read it, another thread may have changed it. Don't gate logic on it.

## 3.4. enable_shared_from_this

A common bug:

```cpp
class Worker {
public:
    void schedule() {
        scheduler.push(std::shared_ptr<Worker>(this));   // ⚠️ second control block!
    }
};

auto w = std::make_shared<Worker>();
w->schedule();
// Now there are TWO control blocks managing the same Worker.
// When either hits zero refs, it deletes the Worker — the other one then
// has a dangling pointer. Crash.
```

The fix: inherit from `std::enable_shared_from_this<T>` and call `shared_from_this()`:

```cpp
class Worker : public std::enable_shared_from_this<Worker> {
public:
    void schedule() {
        scheduler.push(shared_from_this());     // ✅ shares the existing control block
    }
};
```

Caveat: `shared_from_this()` only works after the object is *already* owned by some `shared_ptr`. Calling it from the constructor throws `std::bad_weak_ptr`.

## 3.5. Cyclic References

If two objects each hold a `shared_ptr` to the other, neither refcount ever reaches zero — the cycle leaks forever:

```cpp
struct Node {
    std::shared_ptr<Node> next;   // ⚠️ if next->next points back to this, leak
};
```

Solution: make at least one direction a `weak_ptr`. See §4.2.

# 4. std::weak_ptr

A `weak_ptr<T>` is a non-owning observer of a `shared_ptr`. It doesn't keep the object alive; it just lets you ask "is the object still there?"

## 4.1. Detecting a Dead Object

```cpp
std::shared_ptr<int> sp = std::make_shared<int>(10);
std::weak_ptr<int>   wp = sp;        // observes sp, doesn't own

if (auto locked = wp.lock()) {       // returns shared_ptr — empty if expired
    std::cout << *locked;            // safe to use here
} else {
    std::cout << "object is gone";
}

sp.reset();                          // destroys the int
std::cout << wp.expired();           // true
```

Two ways to check:
- `wp.expired()` — bool, racy in multithreaded code (the answer can change before you act).
- `wp.lock()` — atomic; returns a `shared_ptr` if alive, empty if not. **Always prefer this** when you intend to use the object.

## 4.2. Breaking Cycles

A parent–child tree where children need to point back at their parent:

```cpp
struct Node {
    std::vector<std::shared_ptr<Node>> children;
    std::weak_ptr<Node>                parent;     // ✅ weak — breaks the cycle
};

auto root  = std::make_shared<Node>();
auto child = std::make_shared<Node>();
root->children.push_back(child);
child->parent = root;                              // weak assignment from shared
```

Going up the tree: `if (auto p = child->parent.lock()) { ... }`.

The general rule: when ownership has a clear direction (parent owns children, observer observes subject), the back-reference should be `weak_ptr`.

# 5. Checking for Null

All three smart pointers are contextually convertible to `bool`:

```cpp
if (p)         { /* not null */ }
if (!p)        { /* null */ }
if (p != nullptr) { /* same thing */ }
```

For `weak_ptr` specifically, check via `lock()` (see §4.1) — `weak_ptr` itself is not bool-convertible.

# 6. Pointer Casting

For polymorphic types you can cast through smart pointers without losing the refcount:

```cpp
struct Base { virtual ~Base() = default; };
struct Derived : Base {};

std::shared_ptr<Base> b = std::make_shared<Derived>();

auto d1 = std::static_pointer_cast<Derived>(b);    // unchecked
auto d2 = std::dynamic_pointer_cast<Derived>(b);   // checked, returns empty if wrong type
auto cb = std::const_pointer_cast<Base>(b);        // strip const
```

All three return a new `shared_ptr` that shares the same control block — refcount stays correct.

`unique_ptr` has no cast helpers; if you need to downcast you must move and re-wrap manually (rarely needed).

# 7. Atomic Smart Pointers (C++20)

Pre-C++20, you needed `std::atomic_load(&sp)` / `std::atomic_store(&sp, …)` (now deprecated) to safely share a `shared_ptr` *variable* across threads — note: this is about the pointer object itself, not the pointee. Multiple threads reading and writing the same `sp` variable was a data race.

C++20 adds proper specializations:

```cpp
std::atomic<std::shared_ptr<Config>> current_config;

// thread A — publish
current_config.store(std::make_shared<Config>(load()));

// thread B — read latest
auto cfg = current_config.load();    // gets a fresh shared_ptr safely
cfg->use();
```

This is the right primitive for "hot reload"-style configuration where many readers occasionally see a new pointer.

A separate `std::atomic<std::weak_ptr<T>>` exists for the same reason on weak pointers.

# 8. See Also

- **[shared_ptr_use_cases.md](shared_ptr_use_cases.md)** — twelve patterns where shared ownership is the right tool, each with minimal code.
- **[passing_returning_smart_pointers_to_from_functions.md](passing_returning_smart_pointers_to_from_functions.md)** — function-signature rules (when to take `T*`, `T&`, `unique_ptr<T>`, `const shared_ptr<T>&`).
- **[smart_pointers_class_member.md](smart_pointers_class_member.md)** — owning resources from class members.
- **[pointers.md](pointers.md)** — raw pointers, dangling, AddressSanitizer.
- **[references.md](references.md)** — `reference_wrapper` for storing references in containers.

References:
- [shared_ptr internals (nextptr)](https://www.nextptr.com/tutorial/ta1358374985/shared_ptr-basics-and-internals-with-examples)
- [Top 10 mistakes with C++11 smart pointers](https://www.acodersjourney.com/top-10-dumb-mistakes-avoid-c-11-smart-pointers/)
- [Atomic smart pointers](https://www.modernescpp.com/index.php/atomic-smart-pointers)
