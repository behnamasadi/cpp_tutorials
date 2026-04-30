# Passing and Returning Smart Pointers

How to write function signatures that handle smart pointers correctly — without leaking ownership semantics, without unnecessary refcount bumps, and without the common anti-patterns.

> **The single most important rule** (from the [C++ Core Guidelines, R.30](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#r30-take-smart-pointers-as-parameters-only-to-explicitly-express-lifetime-semantics)):
>
> A function should take a smart pointer **only if it needs to express ownership or lifetime semantics**. Otherwise, pass the object itself: `T&`, `const T&`, or `T*`.

The function signature is documentation. Reading `void f(std::shared_ptr<S>)` should immediately tell the caller "this function will share ownership of the object." If it actually just reads from the object, that signature is lying.

- [1. Decision Table](#1-decision-table)
- [2. unique_ptr](#2-unique_ptr)
  - [2.1. Don't pass a unique_ptr if you only need the object](#21-dont-pass-a-unique_ptr-if-you-only-need-the-object)
  - [2.2. Transfer ownership: by value with std::move](#22-transfer-ownership-by-value-with-stdmove)
  - [2.3. Mutate the pointer itself: by reference](#23-mutate-the-pointer-itself-by-reference)
  - [2.4. Returning unique_ptr (factory pattern)](#24-returning-unique_ptr-factory-pattern)
  - [2.5. Borrowing the underlying object via .get()](#25-borrowing-the-underlying-object-via-get)
  - [2.6. Containers of unique_ptr](#26-containers-of-unique_ptr)
- [3. shared_ptr](#3-shared_ptr)
  - [3.1. Don't pass a shared_ptr if you only need the object](#31-dont-pass-a-shared_ptr-if-you-only-need-the-object)
  - [3.2. Take a copy to share ownership: by value](#32-take-a-copy-to-share-ownership-by-value)
  - [3.3. Just observe, don't bump refcount: const shared_ptr<T>&](#33-just-observe-dont-bump-refcount-const-shared_ptrt)
  - [3.4. Mutate the pointer itself: by reference](#34-mutate-the-pointer-itself-by-reference)
  - [3.5. Returning shared_ptr](#35-returning-shared_ptr)
  - [3.6. enable_shared_from_this for self-sharing](#36-enable_shared_from_this-for-self-sharing)
  - [3.7. Anti-patterns](#37-anti-patterns)
- [4. weak_ptr](#4-weak_ptr)
  - [4.1. Pass by value](#41-pass-by-value)
  - [4.2. Always lock before use](#42-always-lock-before-use)
- [5. Cheat Sheet](#5-cheat-sheet)

---

# 1. Decision Table

| Function intent | Take | Why |
|---|---|---|
| Read the object, can't be null | `const T&` | Cheapest, clearest, no ownership claim. |
| Read or modify the object, can't be null | `T&` | Same, mutable. |
| Read the object, may be null | `const T*` | Pointer signals nullability. |
| Read or modify the object, may be null | `T*` | Same, mutable. |
| **Take ownership** (caller gives it up) | `std::unique_ptr<T>` | The signature *announces* ownership transfer. |
| **Share ownership** (caller and callee both keep) | `std::shared_ptr<T>` (by value) | One refcount bump per call. |
| Observe a `shared_ptr` without bumping refcount | `const std::shared_ptr<T>&` | Hot-loop callers in shared-pointer-heavy codebases. |
| Mutate the smart pointer itself (re-target it) | `std::unique_ptr<T>&` / `std::shared_ptr<T>&` | Rare; document it. |
| Watch a `shared_ptr`-managed object that may die | `std::weak_ptr<T>` | Caller hasn't extended its lifetime. |
| **Return** an owned new object | `std::unique_ptr<T>` | Factory pattern; transfers ownership. |
| **Return** a co-owned object | `std::shared_ptr<T>` | When the receiver participates in shared ownership. |

---

# 2. unique_ptr

`std::unique_ptr<T>` expresses **exclusive ownership**. Use the example type below for all the snippets:

```cpp
struct S {
    int m_id;
    explicit S(int id) : m_id(id) { std::cout << "ctor " << m_id << '\n'; }
    ~S()                          { std::cout << "dtor " << m_id << '\n'; }
    S(S&&) = default;
    S& operator=(S&&) = default;
    S(const S&) = delete;
    S& operator=(const S&) = delete;
};
```

## 2.1. Don't pass a unique_ptr if you only need the object

❌ Wrong — leaks ownership semantics into the signature for no reason:

```cpp
void use(std::unique_ptr<S>& ptr);
void use(const std::unique_ptr<S>& ptr);
```

✅ Right:

```cpp
void use(S& s);              // non-null
void use(const S& s);        // read-only, non-null
void use(const S* s);        // may be null
```

The caller writes `use(*ptr);` or `use(ptr.get());`. The signature now says exactly what `use` needs.

## 2.2. Transfer ownership: by value with std::move

To take ownership, take the `unique_ptr` by value. The caller must `std::move` it in:

```cpp
void take(std::unique_ptr<S> ptr) {
    // `ptr` now owns the resource and will destroy it on scope exit
}

auto p = std::make_unique<S>(10);
take(std::move(p));
// p is empty; do not dereference
```

`std::move` is **required** at the call site — `unique_ptr` has no copy constructor.

## 2.3. Mutate the pointer itself: by reference

Rare. Use only when the function genuinely needs to *re-target* the caller's pointer:

```cpp
void replace(std::unique_ptr<S>& ptr) {
    ptr = std::make_unique<S>(99);
}
```

This is unusual enough that you should document it at the declaration.

## 2.4. Returning unique_ptr (factory pattern)

Returning `unique_ptr` by value is the standard factory:

```cpp
std::unique_ptr<S> create() {
    return std::make_unique<S>(10);    // no std::move needed
}

auto p = create();
```

Notes:
- Do **not** write `return std::move(local);` — that disables NRVO and is no faster.
- Do **not** return a `unique_ptr` that owns a stack object:

```cpp
std::unique_ptr<S> bad() {
    S local(10);
    return std::unique_ptr<S>(&local);   // ⚠️ undefined behavior — &local is on the stack
}
```

## 2.5. Borrowing the underlying object via .get()

If a function takes `const S*` (because it may be null), call it with `ptr.get()`:

```cpp
void inspect(const S* s);

auto p = std::make_unique<S>(10);
inspect(p.get());                        // OK — p still owns the resource
```

Safe **only** if the called function:
- doesn't store the raw pointer beyond its scope,
- doesn't `delete` it,
- doesn't outlive the `unique_ptr`.

## 2.6. Containers of unique_ptr

A `std::vector<std::unique_ptr<T>>` is the standard way to own a polymorphic collection. Pass it the same way you'd pass any other vector — by `const&` to read, by `&` to mutate:

```cpp
void scan(const std::vector<std::unique_ptr<S>>& items) {           // read-only
    for (const auto& p : items) std::cout << p->m_id << '\n';
}

void absorb(std::vector<std::unique_ptr<S>>& items,
            std::unique_ptr<S> extra) {                              // add to caller's vector
    items.push_back(std::move(extra));
}

void take_all(std::vector<std::unique_ptr<S>> items) {               // transfer the whole batch
    // items now owns every element; original caller's vector is empty
}
```

Iterating yields `std::unique_ptr<S>&` (or `const&`); use `p->member` or `*p` as usual. Don't `std::move(p)` out of the vector mid-iteration unless you intend to leave a null behind — `unique_ptr`'s move leaves the source empty, which can surprise the next loop iteration that expects all elements valid.

---

# 3. shared_ptr

`std::shared_ptr<T>` expresses **shared ownership**. The same example type `S` from §2 applies.

## 3.1. Don't pass a shared_ptr if you only need the object

❌ Wrong:

```cpp
void read(std::shared_ptr<S> p);
```

This bumps the refcount on every call (atomic operation, not free) for nothing.

✅ Right:

```cpp
void read(const S& s);
```

## 3.2. Take a copy to share ownership: by value

When the callee genuinely participates in ownership — e.g. stores the pointer in a member, hands it off to a worker thread — take it by value:

```cpp
class Scheduler {
    std::vector<std::shared_ptr<Task>> queue_;
public:
    void enqueue(std::shared_ptr<Task> t) {
        queue_.push_back(std::move(t));   // refcount stays at the right level
    }
};
```

The function commits to extending the object's lifetime.

## 3.3. Just observe, don't bump refcount: const shared_ptr<T>&

If a function needs to *know* it's looking at a shared pointer (perhaps to re-share it conditionally) but most calls won't actually share it, take by `const&` to skip the refcount bump:

```cpp
void maybe_share(const std::shared_ptr<S>& sp) {
    if (some_condition()) {
        registry.add(sp);          // copy here — refcount goes up only if needed
    }
}
```

This is mostly relevant in performance-sensitive code that calls into a `shared_ptr`-heavy API. For pure read access, prefer `const T&` (§3.1) — the cleaner signature.

## 3.4. Mutate the pointer itself: by reference

Same as `unique_ptr` — rare. Reach for non-const reference only when the function must replace the caller's pointer:

```cpp
void rotate(std::shared_ptr<S>& current) {
    current = std::make_shared<S>(next_id());
}
```

## 3.5. Returning shared_ptr

```cpp
std::shared_ptr<S> create() {
    return std::make_shared<S>(100);
}
```

Notes:
- Return by value. RVO/NRVO handles the move; no `std::move` needed.
- Do **not** write `return std::move(local);` — disables NRVO.
- Do **not** construct from a stack object:

```cpp
std::shared_ptr<S> bad() {
    S local(10);
    return std::shared_ptr<S>(&local);   // ⚠️ deletes a stack address — UB
}
```

## 3.6. enable_shared_from_this for self-sharing

If a member function needs to hand out a `shared_ptr` to itself (e.g. registering with a callback queue), inheriting from `std::enable_shared_from_this<T>` is the only correct way:

```cpp
class Worker : public std::enable_shared_from_this<Worker> {
public:
    void schedule(Queue& q) {
        q.push(shared_from_this());      // ✅ shares the existing control block
    }
};
```

Never write `std::shared_ptr<Worker>(this)` — that creates a *second* control block, leading to a double-delete when both refcounts hit zero. See [smart_pointers.md §3.4](smart_pointers.md#34-enable_shared_from_this).

`shared_from_this()` only works after the object is already managed by some `shared_ptr` — calling it from a constructor throws `std::bad_weak_ptr`.

## 3.7. Anti-patterns

### Two shared_ptrs from the same raw pointer

```cpp
S* raw = new S(10);
std::shared_ptr<S> p1(raw);
std::shared_ptr<S> p2(raw);   // ⚠️ both delete on destruction — double-free
```

Each `shared_ptr` constructor from a raw pointer creates its own control block. They don't know about each other.

✅ Right:

```cpp
auto p1 = std::make_shared<S>(10);
auto p2 = p1;                  // copy — same control block
```

### Storing a shared_ptr to *this* without enable_shared_from_this

Same root cause as above — see §3.6.

---

# 4. weak_ptr

A `std::weak_ptr<T>` is a non-owning observer. Use it when you want to *check* whether a shared object is still alive without keeping it alive yourself.

## 4.1. Pass by value

```cpp
void observe(std::weak_ptr<S> wp) {
    if (auto sp = wp.lock()) {
        std::cout << sp->m_id << '\n';   // sp keeps the object alive in this scope
    } else {
        std::cout << "object is gone\n";
    }
}
```

`weak_ptr` is cheap to copy — by-value is fine.

## 4.2. Always lock before use

```cpp
*wp.lock();                     // ⚠️ if expired, lock() returns empty shared_ptr — UB
```

```cpp
if (auto sp = wp.lock()) {      // ✅ check first
    // sp is alive here for the rest of the scope
}
```

`expired()` is a cheaper check, but in multithreaded code its answer can change between the call and the next instruction. **Prefer `lock()`** — it's atomic.

---

# 5. Cheat Sheet

| Intent | Pass as | Return as |
|---|---|---|
| Read object only, non-null | `const T&` | — |
| Read object only, may be null | `const T*` | — |
| Modify object, non-null | `T&` | — |
| Take ownership (caller gives up) | `std::unique_ptr<T>` (move at call) | `std::unique_ptr<T>` (factory) |
| Share ownership (callee keeps it) | `std::shared_ptr<T>` (by value) | `std::shared_ptr<T>` |
| Observe a `shared_ptr` without bumping count | `const std::shared_ptr<T>&` | rare |
| Watch but don't extend lifetime | `std::weak_ptr<T>` | `std::weak_ptr<T>` |
| Replace the caller's pointer | `std::unique_ptr<T>&` / `std::shared_ptr<T>&` | — |

**Never:**
- Pass a smart pointer "just to be safe" if you don't need ownership semantics.
- `return std::move(local_smart_ptr);` — disables RVO/NRVO.
- Build two `shared_ptr`s from the same raw pointer.
- Build a `shared_ptr<T>(this)` inside a member; use `enable_shared_from_this`.

**See also:**
- [smart_pointers.md](smart_pointers.md) — the underlying types and their guarantees.
- [smart_pointers_class_member.md](smart_pointers_class_member.md) — owning resources via class members.
- [smart_pointers_class_member.md §7](smart_pointers_class_member.md#7-real-world-worked-examples) — three real-world scenarios (document editor, HTTP server, texture cache) showing the function-passing rules from this doc combined with class-member ownership.
