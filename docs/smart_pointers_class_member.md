# Smart Pointers as Class Members

When a class owns heap resources, smart-pointer members give you automatic cleanup, exception safety, and the right copy/move behavior — for free. The trick is picking the right pointer for the ownership story.

- [1. Quick Decision Guide](#1-quick-decision-guide)
- [2. unique_ptr Member](#2-unique_ptr-member)
  - [2.1. The default for owned resources](#21-the-default-for-owned-resources)
  - [2.2. Rule of Zero — let the compiler do it](#22-rule-of-zero--let-the-compiler-do-it)
  - [2.3. Polymorphic ownership](#23-polymorphic-ownership)
- [3. The PIMPL Idiom](#3-the-pimpl-idiom)
  - [3.1. Why PIMPL](#31-why-pimpl)
  - [3.2. The two-file pattern](#32-the-two-file-pattern)
  - [3.3. The "destructor in header" gotcha](#33-the-destructor-in-header-gotcha)
- [4. shared_ptr Member](#4-shared_ptr-member)
  - [4.1. When it's actually appropriate](#41-when-its-actually-appropriate)
  - [4.2. Thread safety: pointer vs pointee](#42-thread-safety-pointer-vs-pointee)
- [5. weak_ptr Member](#5-weak_ptr-member)
  - [5.1. Breaking parent–child cycles](#51-breaking-parentchild-cycles)
  - [5.2. Cached lookups](#52-cached-lookups)
- [6. Exposing the Resource Through Getters](#6-exposing-the-resource-through-getters)
  - [6.1. Read-only access](#61-read-only-access)
  - [6.2. Mutable access](#62-mutable-access)
  - [6.3. Optional access](#63-optional-access)
  - [6.4. Sharing ownership outward (shared_ptr / weak_ptr)](#64-sharing-ownership-outward-shared_ptr--weak_ptr)
  - [6.5. Don't expose the smart pointer itself](#65-dont-expose-the-smart-pointer-itself)
- [7. Real-World Worked Examples](#7-real-world-worked-examples)
  - [7.1. Document editor — exclusive ownership tree](#71-document-editor--exclusive-ownership-tree)
  - [7.2. HTTP server — shared request, observing logger](#72-http-server--shared-request-observing-logger)
  - [7.3. Texture cache — owner gives out non-extending handles](#73-texture-cache--owner-gives-out-non-extending-handles)
- [8. Common Pitfalls](#8-common-pitfalls)
- [9. See Also](#9-see-also)

---

# 1. Quick Decision Guide

| Situation | Use |
|---|---|
| Class owns one heap object exclusively | `std::unique_ptr<T>` |
| Class hides its implementation in a `.cpp` (PIMPL) | `std::unique_ptr<Impl>` |
| Class owns a polymorphic object (`Base*` may be `Derived`) | `std::unique_ptr<Base>` |
| Resource is genuinely co-owned with other code | `std::shared_ptr<T>` |
| Class observes a `shared_ptr`-managed object without owning it | `std::weak_ptr<T>` |
| Class needs to know *of* an object but never use it | raw pointer or reference |

If you're using `shared_ptr` because you're not sure who should own the resource — stop and figure it out. The default answer is `unique_ptr`.

---

# 2. unique_ptr Member

## 2.1. The default for owned resources

```cpp
#include <memory>

class Engine {
    std::unique_ptr<Carburetor> carb_;
public:
    Engine() : carb_(std::make_unique<Carburetor>()) {}
    void rev() { carb_->open(); }
};
```

That's the entire pattern. Construction creates the carburetor; destruction destroys it; the engine *cannot* leak it.

Initialize `unique_ptr` members in the **constructor initializer list**, not the body. Initializer-list construction is exception-safe: if a later member throws during init, already-constructed `unique_ptr`s are properly destroyed.

## 2.2. Rule of Zero — let the compiler do it

A `unique_ptr` member makes the enclosing class **automatically move-only** with the right semantics for free. You don't have to write any of the special members:

```cpp
class Engine {
    std::unique_ptr<Carburetor> carb_;
public:
    Engine() : carb_(std::make_unique<Carburetor>()) {}

    // Compiler-generated:
    // ❌ copy ctor      — deleted (unique_ptr is move-only)
    // ❌ copy assign    — deleted
    // ✅ move ctor      — defaulted, transfers carb_
    // ✅ move assign    — defaulted, transfers carb_
    // ✅ destructor     — defaulted, releases carb_
};
```

> **Don't write `=delete` and `=default` boilerplate** unless you're being explicit for a reason. The compiler does the right thing automatically when all members behave correctly.

If you need the class to be copyable, give it an explicit copy constructor that deep-copies the resource:

```cpp
class Engine {
    std::unique_ptr<Carburetor> carb_;
public:
    Engine()
      : carb_(std::make_unique<Carburetor>()) {}

    Engine(const Engine& other)
      : carb_(std::make_unique<Carburetor>(*other.carb_)) {}   // deep copy

    Engine& operator=(const Engine& other) {
        carb_ = std::make_unique<Carburetor>(*other.carb_);
        return *this;
    }

    // move ops still defaulted — copy/move-and-swap could simplify further
    Engine(Engine&&)            = default;
    Engine& operator=(Engine&&) = default;
};
```

## 2.3. Polymorphic ownership

A class that owns "some Renderer" without knowing the concrete type:

```cpp
class Window {
    std::unique_ptr<Renderer> renderer_;          // Renderer is abstract
public:
    explicit Window(std::unique_ptr<Renderer> r)
      : renderer_(std::move(r)) {}

    void draw() { renderer_->render(); }          // virtual dispatch
};

// elsewhere
auto w = Window(std::make_unique<VulkanRenderer>());
```

Two requirements that bite people:

- `Renderer` must have a **`virtual` destructor**, otherwise `~Window()` won't call `~VulkanRenderer()`.
- The factory pattern (`std::make_unique<Concrete>()` returned as `unique_ptr<Base>`) only works because of `unique_ptr`'s implicit conversion from `unique_ptr<Derived>` to `unique_ptr<Base>` — which itself requires a virtual destructor for safety.

---

# 3. The PIMPL Idiom

**PIMPL** (Pointer to IMPLementation) is the canonical use of `std::unique_ptr` as a member. It's how you keep a class's implementation completely out of its public header.

## 3.1. Why PIMPL

Every `#include` of a class's header drags in everything that header transitively includes. PIMPL solves three problems at once:

1. **Compile-time decoupling** — change the implementation, only the `.cpp` recompiles, not every translation unit that uses the class.
2. **Stable ABI** — adding a private member or changing internal types doesn't change the size or layout of the public class. Critical for shared libraries.
3. **Hide expensive headers** — the public header doesn't need to `#include <thread>`, `<network/socket>`, or your big template library.

## 3.2. The two-file pattern

```cpp
// widget.h
#pragma once
#include <memory>

class Widget {
public:
    Widget();
    ~Widget();                          // declared, defined in .cpp (see §3.3)
    Widget(Widget&&) noexcept;
    Widget& operator=(Widget&&) noexcept;

    void do_thing();

private:
    struct Impl;                        // forward declaration only
    std::unique_ptr<Impl> impl_;        // size of Widget = sizeof(unique_ptr) ≈ ptr
};
```

```cpp
// widget.cpp
#include "widget.h"
#include <vector>
#include <thread>
#include "internal/heavy_thing.h"

struct Widget::Impl {                   // full definition, only the .cpp sees it
    std::vector<int>     data;
    std::thread          worker;
    HeavyThing           thing;
};

Widget::Widget()
  : impl_(std::make_unique<Impl>()) {}

Widget::~Widget()                = default;   // defined here, where Impl is complete
Widget::Widget(Widget&&) noexcept = default;
Widget& Widget::operator=(Widget&&) noexcept = default;

void Widget::do_thing() {
    impl_->thing.use(impl_->data);
}
```

The header has no clue what's inside `Impl`. Anyone using `Widget` doesn't pull in `<vector>`, `<thread>`, or `heavy_thing.h`.

## 3.3. The "destructor in header" gotcha

```cpp
// widget.h
class Widget {
    struct Impl;
    std::unique_ptr<Impl> impl_;
public:
    Widget();
    // ⚠️ defaulted destructor in the header
};
```

This won't compile. `unique_ptr<Impl>`'s destructor needs to call `delete impl_`, which needs the *complete* type of `Impl` — but in the header, `Impl` is only forward-declared.

**Fix:** declare the destructor in the header, define it in the `.cpp` *after* `Impl` is fully defined:

```cpp
// widget.h
~Widget();                              // declared

// widget.cpp
Widget::~Widget() = default;            // defined here, where Impl is complete
```

Same story for the move constructor and move assignment operator if you want them.

---

# 4. shared_ptr Member

## 4.1. When it's actually appropriate

A `shared_ptr` member is right when the class is **one of several co-owners** of the resource — i.e. the resource may legitimately outlive the class instance.

```cpp
class CacheEntry {
    std::shared_ptr<Buffer> buf_;       // also held by other entries pointing
                                        // at the same underlying buffer
public:
    explicit CacheEntry(std::shared_ptr<Buffer> b) : buf_(std::move(b)) {}
};
```

If the buffer's only owner is `CacheEntry`, this should be `unique_ptr<Buffer>` instead. Using `shared_ptr` here just to avoid thinking about ownership is the most common smart-pointer mistake. See [shared_ptr_use_cases.md](shared_ptr_use_cases.md) for cases where shared ownership is genuinely the right model.

## 4.2. Thread safety: pointer vs pointee

A subtle but important distinction:

| Operation | Thread-safe? |
|---|---|
| Two threads each modify *their own copy* of a `shared_ptr` to the same object | ✅ — refcount is atomic |
| Two threads modify *the same `shared_ptr` variable* | ❌ — that's a data race; use `std::atomic<std::shared_ptr<T>>` (C++20) |
| Two threads modify *the pointee* through a `shared_ptr` | ❌ — `shared_ptr` doesn't synchronize the *object*, only the refcount |

```cpp
class Service {
    std::shared_ptr<Config> cfg_;
public:
    // If cfg_->reload() mutates the Config, you still need a mutex around
    // the Config — sharing the pointer doesn't make the pointee thread-safe.
    void reload() {
        std::lock_guard lock(cfg_->mu_);
        cfg_->reload();
    }
};
```

Confusing the two is a recurring bug. The pointer-counting machinery doesn't extend to the data.

---

# 5. weak_ptr Member

A `weak_ptr` is *never* the primary owner. It's an observer — used to break cycles or to cache a reference without extending lifetime.

## 5.1. Breaking parent–child cycles

```cpp
class Tree {
    struct Node {
        std::vector<std::shared_ptr<Node>> children;
        std::weak_ptr<Node>                parent;       // ✅ weak — no cycle
    };
    std::shared_ptr<Node> root_ = std::make_shared<Node>();
};
```

If `parent` were a `shared_ptr`, parent and child would each hold a strong ref to the other, the refcounts would never reach zero, and the tree would leak. With `weak_ptr`, the parent owns its children; children only *observe* their parent.

To traverse upward: `if (auto p = node->parent.lock()) { ... }`.

## 5.2. Cached lookups

```cpp
class Resolver {
    std::weak_ptr<Cache> cache_;
public:
    std::shared_ptr<Cache> get_cache() {
        if (auto c = cache_.lock()) return c;            // still alive — reuse
        auto fresh = std::make_shared<Cache>(load());
        cache_ = fresh;
        return fresh;
    }
};
```

The cache lives only as long as some caller holds onto it. Once everyone is done, the next call pays the rebuild cost. Saves memory at the cost of recomputation.

---

# 6. Exposing the Resource Through Getters

A class with a smart-pointer member needs to decide *how* to expose the pointee. The signature you choose is a contract about lifetime, nullability, and ownership. The smart pointer is an implementation detail; the caller should see *the resource*, not your storage choice.

Use the example class for all snippets in this section:

```cpp
class Engine {
    std::unique_ptr<Carburetor> carb_;
public:
    Engine() : carb_(std::make_unique<Carburetor>()) {}
    // ... getters discussed below
};
```

## 6.1. Read-only access

```cpp
const Carburetor& carb() const { return *carb_; }
```

`const T&` is the cleanest signal: "you can read it, it can't be null, and don't outlive me." No refcount, no nullability, no leaked storage detail.

## 6.2. Mutable access

```cpp
Carburetor& carb() { return *carb_; }
```

Same lifetime contract; caller can mutate. Only do this when mutation through the getter is intentional. Most well-designed classes prefer to expose *operations* (`engine.rev()`) rather than the inner object — exposing the object often hints that the encapsulation is too thin.

## 6.3. Optional access

If the resource may not exist, return a pointer (still bound to `*this`'s lifetime):

```cpp
Carburetor* maybe_carb() { return carb_.get(); }            // may be nullptr
const Carburetor* maybe_carb() const { return carb_.get(); }
```

The raw pointer is the canonical "may be null, non-owning" signal. `std::optional<std::reference_wrapper<Carburetor>>` exists but is rarely worth its noise.

## 6.4. Sharing ownership outward (shared_ptr / weak_ptr)

If the member is `shared_ptr`, you can hand callers their own owning copy or a non-owning observer:

```cpp
class Cache {
    std::shared_ptr<Buffer> buf_;
public:
    std::shared_ptr<Buffer> snapshot() const { return buf_; }   // refcount++
    std::weak_ptr<Buffer>   watch()    const { return buf_; }   // observe only
};
```

Use `snapshot()` when callers must guarantee the buffer stays alive (passing to a worker thread, queuing for async processing). Use `watch()` for long-lived monitors that should *not* keep a short-lived buffer alive.

## 6.5. Don't expose the smart pointer itself

```cpp
const std::unique_ptr<Carburetor>& carb() const { return carb_; }   // ⚠️ leaks storage detail
```

This signature tells callers "you're seeing my unique_ptr." But what they want is access to `Carburetor`. Return `const Carburetor&` (or `Carburetor*` if optional) — the caller doesn't care, and shouldn't care, that you store it via a smart pointer. Same applies to `shared_ptr` getters: return what the caller actually needs, not the box you keep it in.

There is one narrow exception: if your class is part of a system that genuinely passes shared ownership through it (e.g. a registry handing out co-owned resources), returning `std::shared_ptr<T>` is appropriate per §6.4 — the smart pointer **is** the contract there.

---

# 7. Real-World Worked Examples

How the patterns from §1–6 actually combine in code. Each example shows both the class-member ownership choice and the resulting method signatures.

## 7.1. Document editor — exclusive ownership tree

A text editor where a `Document` owns its paragraphs and exposes them by reference:

```cpp
class Paragraph {
    std::string text_;
public:
    explicit Paragraph(std::string t) : text_(std::move(t)) {}
    const std::string& text() const { return text_; }
    void append(std::string_view s) { text_ += s; }
};

class Document {
    std::vector<std::unique_ptr<Paragraph>> paragraphs_;             // §2.1: sole owner
public:
    Paragraph& add(std::string text) {
        paragraphs_.push_back(std::make_unique<Paragraph>(std::move(text)));
        return *paragraphs_.back();                                  // §6.2: mutable getter
    }
    const Paragraph& at(std::size_t i) const {                        // §6.1: read-only getter
        return *paragraphs_.at(i);
    }
    std::size_t size() const { return paragraphs_.size(); }
};

void print(const Document& doc) {                                    // borrow, no ownership
    for (std::size_t i = 0; i < doc.size(); ++i)
        std::cout << doc.at(i).text() << '\n';
}

std::unique_ptr<Document> load(const std::string& path);             // factory: transfers ownership
```

**Why these choices:**
- `vector<unique_ptr<Paragraph>>` — the document is the only thing that should free paragraphs; deletion is automatic when the vector is destroyed.
- Methods return `Paragraph&` / `const Paragraph&` — the caller wants the object, not the box. Lifetime is bound to the document.
- `print(const Document&)` — read-only function takes the object, not the smart pointer (the [passing rules](passing_returning_smart_pointers_to_from_functions.md#1-decision-table)).
- `load` returns `std::unique_ptr<Document>` — factory pattern; ownership passes to the caller.

## 7.2. HTTP server — shared request, observing logger

An async handler chain that may keep a request alive past the original scope, plus a logger that watches without extending lifetime:

```cpp
struct Request { int id; std::string url; std::string body; };

class Handler {
public:
    void handle(std::shared_ptr<Request> r) {                        // co-owns until done
        // may pass r to async I/O, retry queue, worker thread...
    }
};

class Logger {
    std::vector<std::weak_ptr<Request>> watching_;                   // §5: weak observer
public:
    void watch(std::weak_ptr<Request> r) { watching_.push_back(std::move(r)); }
    void report() {
        for (auto& w : watching_)
            if (auto r = w.lock())                                   // §4.2 of passing doc
                std::cout << "still active: " << r->id << '\n';
    }
};

void serve(std::shared_ptr<Request> req, Handler& h, Logger& log) {
    log.watch(req);                                                  // refcount stays at 1
    h.handle(std::move(req));                                        // hand off ownership
}
```

**Why these choices:**
- `Request` is `shared_ptr` because the handler chain may continue after `serve` returns (async I/O, retries, background workers).
- `Logger` holds `weak_ptr` — a forgotten log entry must not keep dead requests alive forever.
- `handle(shared_ptr<Request>)` by value because the handler genuinely co-owns.
- `serve` takes `shared_ptr<Request>` by value because it splits ownership: copy goes to the logger as `weak_ptr`, the original is moved into `handle`.

## 7.3. Texture cache — owner gives out non-extending handles

A cache that hands out lightweight observers and evicts entries no caller is using:

```cpp
class Texture { /* big GPU resource */ };

class TextureCache {
    std::unordered_map<std::string, std::shared_ptr<Texture>> entries_;
public:
    std::weak_ptr<Texture> get(const std::string& path) {            // §6.4: hand out observer
        auto& slot = entries_[path];
        if (!slot) slot = std::make_shared<Texture>(/* load from disk */);
        return slot;
    }
    void evict_unused() {
        for (auto it = entries_.begin(); it != entries_.end();) {
            if (it->second.use_count() == 1)                         // only the cache holds it
                it = entries_.erase(it);
            else
                ++it;
        }
    }
};

void render(std::weak_ptr<Texture> tex) {
    if (auto t = tex.lock()) {                                       // alive for this scope
        // use *t
    }
    // expired: skip silently
}
```

**Why these choices:**
- The cache *wants to be the owner* (it controls eviction) but must expose handles that don't block eviction. `shared_ptr` inside, `weak_ptr` out.
- `get()` returns `weak_ptr<Texture>` — callers can use the texture, but the cache decides when it dies.
- `evict_unused()` checks `use_count() == 1` — if only the cache has a strong reference, no caller is using it and it's safe to drop.
- `render(weak_ptr<Texture>)` — the renderer doesn't extend lifetime; it locks atomically before use, and silently skips a texture that was evicted between frames.

## When to reach for which example

| Your situation | Closest match | Key signal |
|---|---|---|
| One object owns a tree/list of others | §7.1 Document | "Only this owner should free them." |
| Lifetime crosses async boundaries | §7.2 HTTP server | "The work continues after the caller leaves." |
| Owner controls eviction; users observe | §7.3 Texture cache | "I want callers to use it but not keep it alive." |

---

# 8. Common Pitfalls

### Writing `=default`/`=delete` boilerplate by hand

```cpp
class Bad {
    std::unique_ptr<Resource> r_;
public:
    Bad(const Bad&) = delete;            // ⚠️ noise — already deleted by unique_ptr
    Bad& operator=(const Bad&) = delete; // ⚠️ noise
    Bad(Bad&&) = default;                // ⚠️ noise — already defaulted
    Bad& operator=(Bad&&) = default;     // ⚠️ noise
    ~Bad() = default;                    // ⚠️ noise
};
```

The compiler generates exactly these. Writing them yourself is "Rule of Zero" violations and just adds maintenance.

### Sharing a class member with `enable_shared_from_this` via `this`

```cpp
class Worker {
    std::shared_ptr<Worker> hand_off() {
        return std::shared_ptr<Worker>(this);   // ⚠️ creates a SECOND control block
    }
};
```

Use [`std::enable_shared_from_this`](smart_pointers.md#34-enable_shared_from_this).

### Using a `shared_ptr` member that doesn't need to be shared

The single biggest source of unnecessary atomic refcount traffic in modern C++ codebases. If your class is the sole owner, use `unique_ptr`.

### Holding a `unique_ptr<Base>` to a derived type without a virtual destructor

```cpp
struct Base { /* no virtual ~Base() */ };
struct Derived : Base { ~Derived() { /* won't run */ } };

std::unique_ptr<Base> p = std::make_unique<Derived>();   // ⚠️ undefined behavior at destruction
```

If you store a polymorphic type via a base-class smart pointer, the base needs a `virtual` destructor.

---

# 9. See Also

- **[smart_pointers.md](smart_pointers.md)** — the underlying types and their guarantees.
- **[passing_returning_smart_pointers_to_from_functions.md](passing_returning_smart_pointers_to_from_functions.md)** — function-signature rules.
- **[shared_ptr_use_cases.md](shared_ptr_use_cases.md)** — twelve patterns where shared ownership is genuinely the right tool.
- **[pointers.md](pointers.md)** — raw pointers, dangling, AddressSanitizer.
