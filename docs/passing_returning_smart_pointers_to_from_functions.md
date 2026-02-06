
# Passing and Returning Smart Pointers in C++

This document explains **when and how to pass or return smart pointers** in C++, with correct ownership semantics and interview-safe rules.

> **Key rule (C++ Core Guidelines)**
> A function should take a smart pointer **only if it needs to express ownership or lifetime semantics**.
> Otherwise, pass the object itself (`T&`, `const T&`, or `T*`).

Reference:[isocpp](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#r30-take-smart-pointers-as-parameters-only-to-explicitly-express-lifetime-semantics)

---

## Example type

```cpp
#include <iostream>

struct S {
    int m_id;

    explicit S(int id) : m_id(id) {
        std::cout << "ctor " << m_id << std::endl;
    }

    ~S() {
        std::cout << "dtor " << m_id << std::endl;
    }

    S(S&&) = default;
    S& operator=(S&&) = default;

    S(const S&) = delete;
    S& operator=(const S&) = delete;
};
```

---

## std::unique_ptr

`std::unique_ptr<T>` expresses **exclusive ownership**.

### Good Practices

### 1. Pass the object, not the smart pointer, if ownership is not involved

❌ Bad (leaks ownership semantics):

```cpp
void useResource(std::unique_ptr<S>& ptr);
```

✅ Correct:

```cpp
void useResource(S& s) {
    // uses the object, no ownership semantics
}
```

Nullable case:

```cpp
void useResource(const S* s) {
    if (!s) return;
}
```

---

### 2. Transfer ownership explicitly with `std::move`

```cpp
void takeOwnership(std::unique_ptr<S> ptr) {
    // now owns the resource
}

int main() {
    auto p = std::make_unique<S>(10);
    takeOwnership(std::move(p));
    // p is now empty
}
```

This clearly communicates **ownership transfer**.

---

### 3. Modify the pointer itself via reference (rare but valid)

```cpp
void reset(std::unique_ptr<S>& ptr) {
    ptr.reset();
}
```

Use this **only** when the function must change the pointer.

---

### Returning `std::unique_ptr`

Returning a `std::unique_ptr` is the **standard factory pattern**.

```cpp
std::unique_ptr<S> createResource() {
    return std::make_unique<S>(10);
}

int main() {
    auto ptr = createResource();
}
```

Notes:

* The managed object is heap-allocated
* Ownership is transferred safely
* No `std::move` is required
* Copy elision / move semantics are handled by the language

❌ **Never** do this:

```cpp
std::unique_ptr<S> bad() {
    S local(10);
    return std::unique_ptr<S>(&local); // undefined behavior
}
```

---

### Raw pointers alongside `unique_ptr`

```cpp
void observe(const S* s);

int main() {
    auto ptr = std::make_unique<S>(10);
    observe(ptr.get()); // OK if pointer is not stored or deleted
}
```

This is safe **only if** the raw pointer:

* does not escape
* is not deleted
* does not outlive the `unique_ptr`

---

## `std::shared_ptr`

`std::shared_ptr<T>` expresses **shared ownership**.

### Good Practices

### 1. Pass by value to share ownership

```cpp
#include <memory>

void process(std::shared_ptr<S> p) {
    std::cout << p.use_count() << std::endl;
}

int main() {
    auto sp = std::make_shared<S>(42);
    process(sp);
}
```

This explicitly extends the object’s lifetime.

---

### 2. Pass the object if ownership is not needed

❌ Bad:

```cpp
void read(std::shared_ptr<S> p);
```

✅ Correct:

```cpp
void read(const S& s);
```

---

### Returning `std::shared_ptr`

```cpp
std::shared_ptr<S> createShared() {
    return std::make_shared<S>(100);
}

int main() {
    auto sp = createShared();
}
```

Notes:

* No extra reference count increment is required
* Copy elision applies
* Do **not** write `return std::move(ptr);`

❌ Anti-pattern:

```cpp
return std::move(ptr); // disables NRVO, no benefit
```

---

### Bad Practices

### 1. Modifying a `shared_ptr` via reference

```cpp
void dangerous(std::shared_ptr<S>& p) {
    p = std::make_shared<S>(20);
}
```

This silently replaces ownership and is usually surprising.

---

### 2. Creating `shared_ptr` from stack objects

```cpp
std::shared_ptr<S> bad() {
    S local(10);
    return std::shared_ptr<S>(&local); // undefined behavior
}
```

Correct approach:

```cpp
return std::make_shared<S>(10);
```

---

### 3. Multiple `shared_ptr`s from the same raw pointer

```cpp
S* raw = new S(10);
std::shared_ptr<S> p1(raw);
std::shared_ptr<S> p2(raw); // double delete
```

Always create **one** owning `shared_ptr`.

---

## `std::weak_ptr`

`std::weak_ptr<T>` is a **non-owning observer** of a `shared_ptr`-managed object.

### Good Practices

### 1. Pass by value

```cpp
void observe(std::weak_ptr<S> wp) {
    if (auto sp = wp.lock()) {
        std::cout << sp->m_id << std::endl;
    }
}
```

* `lock()` returns a `shared_ptr`
* The object is guaranteed alive during that scope

---

### 2. Return `weak_ptr` to avoid extending lifetime

```cpp
std::weak_ptr<S> getWeak(const std::shared_ptr<S>& sp) {
    return sp;
}
```

---

### Bad Practices

### 1. Using `weak_ptr` without locking

❌ Bad:

```cpp
*weakPtr.lock(); // ignoring result validity
```

✅ Correct:

```cpp
if (auto sp = weakPtr.lock()) {
    // safe
}
```

---

### 2. Treating `weak_ptr` as a permanent reference

A `weak_ptr` **does not guarantee existence**.
It must be checked every time it is used.

---

## Summary Cheat Sheet

| Intent                 | Use                      |
| ---------------------- | ------------------------ |
| Use object only        | `T&`, `const T&`, `T*`   |
| Transfer ownership     | `std::unique_ptr<T>`     |
| Share ownership        | `std::shared_ptr<T>`     |
| Observe without owning | `std::weak_ptr<T>`       |
| Factory function       | return smart pointer     |
| Just reading           | never pass smart pointer |

---

