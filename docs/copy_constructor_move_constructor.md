```cpp
struct wallet {
  int m_id = 0;
  int m_size = 0;
  double* m_data = nullptr;
  };
```

# Copy Constructor

## When the **copy constructor** is called

The copy constructor is invoked when **a new `wallet` object is created from an existing one**.

---

## 1) Copy-initialization

```cpp
wallet a(1, 10);
wallet b = a;              // copy ctor
```

---

## 2) Direct-initialization

```cpp
wallet a(1, 10);
wallet b(a);               // copy ctor
```

---

## 3) Pass-by-value into a function

```cpp
wallet f(wallet w) {
  return w;
}

wallet a(1, 10);
wallet b = f(a);           // copy ctor into parameter
```

---

## 4) Return-by-value (copy elision may apply)

```cpp
wallet make() {
  wallet t(7, 70);
  return t;                // elided or move ctor
}
```

---

## 5) Lambda capture by value

```cpp
wallet a(1, 10);

auto lam = [a]() { };      // copy ctor into closure
```

---

## 6) Copying into containers

```cpp
std::vector<wallet> v;
wallet a(1, 10);

v.push_back(a);            // copy ctor
```

---

# Copy Assignment Operator

The copy assignment operator is used when an **already-existing object** is overwritten.

---

## 1) Simple assignment

```cpp
wallet a(1, 10);
wallet b(2, 20);

b = a;                     // copy assign
```

---

## 2) Self-assignment

```cpp
wallet a(1, 10);
a = a;                     // safe no-op
```

---

## 3) Assignment inside containers

```cpp
std::vector<wallet> v;
v.emplace_back(1, 10);
v.emplace_back(2, 20);

v[1] = v[0];               // copy assign
```

---

# Move Constructor

The move constructor creates a **new object by stealing resources** from an rvalue.

---

## 1) Explicit move

```cpp
wallet a(1, 10);
wallet b = std::move(a);   // move ctor
```

---

## 2) Constructing from a temporary

```cpp
wallet b = wallet(2, 20);  // elided or move ctor
```

---

## 3) Pass-by-value with rvalue

```cpp
wallet b = f(wallet(3, 30)); // move ctor
```

---

## 4) Containers

```cpp
std::vector<wallet> v;
v.push_back(wallet(4, 40)); // move ctor
```

---

# Move Assignment Operator

Move assignment overwrites an **existing object** using an rvalue.

---

## 1) Assigning from `std::move`

```cpp
wallet a(1, 10);
wallet b(2, 20);

b = std::move(a);          // move assign
```

---

## 2) Container element move assignment

```cpp
v[1] = std::move(v[0]);    // move assign
```

---

# Easy Rules to Remember

- ✅ Copy constructor → new object from lvalue
- ✅ Copy assignment → overwrite existing object
- ✅ Move constructor → new object from rvalue
- ✅ Move assignment → overwrite existing object from rvalue

---

# Rule of Five

If you **define any one** of the following, you should define **all five**:

* destructor
* copy constructor
* copy assignment
* move constructor
* move assignment

Otherwise, define **none** and rely on defaults.

---



### Core rules for `noexcept` (practical, Rule-of-5 context)

`noexcept` is a **promise**: if an exception escapes, the program calls `std::terminate()`. So you add it when you are confident the function cannot throw, and when it improves behavior/performance (especially in the standard library).

---

## Where you should almost always add `noexcept` in your `wallet`-style class

### 1) Destructor: `~wallet() noexcept`

* **Why**: Throwing from a destructor during stack unwinding is fatal (terminates). Also, standard library types assume destructors don’t throw.
* In your class, `delete[]` does not throw, and `std::cout` *can* throw if exceptions are enabled on the stream, so if you keep `std::cout` in destructors, it weakens the “can’t throw” guarantee.

**Rule**: In production code, keep destructors non-throwing and avoid throwing operations inside them. Mark them `noexcept` (or just default them).

### 2) Move constructor: `wallet(wallet&&) noexcept`

* **Why (big one)**: Containers like `std::vector` will prefer **moving during reallocation** only if the move ctor is `noexcept` (or if copying is not available). Otherwise they may copy to preserve the strong exception guarantee.
* If your move is basically “steal pointers, null out rhs”, it’s naturally non-throwing.

### 3) Move assignment: `wallet& operator=(wallet&&) noexcept`

* **Why**: Same reason: enables faster reallocation/moves and better container behavior.
* Also signals your move assignment is safe to use in many generic contexts.

### 4) `swap` (if you had one) and other low-level operations

* If you provide a `swap`, make it `noexcept`. It’s commonly used by algorithms and can affect optimizations.

---

## Where you usually **should not** add `noexcept` in this class

### 1) Copy constructor and copy assignment

They allocate memory (`new[]`). Allocation can throw `std::bad_alloc`.

So generally:

* `wallet(const wallet&)` ❌ not `noexcept`
* `wallet& operator=(const wallet&)` ❌ not `noexcept`

(Unless you use a custom allocator or design that truly cannot throw, which is uncommon.)

### 2) Regular constructors that allocate

Same reason: `new` can throw.

---

## A simple checklist you can apply

### Add `noexcept` when:

1. The function **does not allocate** and does not call anything that can throw.
2. It’s a **move operation** or **destructor** or **swap** for a type that might go into containers.
3. You want to enable standard library optimizations and strong guarantees during reallocation.

### Don’t add `noexcept` when:

1. The function may allocate (`new`, `std::vector` growth, etc.).
2. It calls user-provided callbacks / virtual functions / code you don’t control.
3. You’re not sure. (Because a wrong `noexcept` turns exceptions into termination.)

---

## In your `wallet` context: what should be `noexcept`?

### ✅ Good:

```cpp
wallet(wallet&&) noexcept;
wallet& operator=(wallet&&) noexcept;
~wallet() noexcept;
```

### ❌ Usually not:

```cpp
wallet(const wallet&);            // allocates
wallet& operator=(const wallet&); // allocates
wallet(int id, int size);         // allocates
```

---

## “Destructor is `noexcept` implicitly, right?”

Yes, with an important nuance:

* In modern C++ (since C++11), destructors are **implicitly `noexcept(true)` by default** *unless* something in the destructor’s exception specification makes it potentially throwing.
* So `~wallet()` is effectively non-throwing by default.

But two practical points:

1. **If an exception escapes a destructor**, the program terminates anyway. So “implicitly noexcept” doesn’t mean “safe to throw”; it means “you must not let exceptions escape.”
2. If you put code inside the destructor that can throw (like I/O with exceptions enabled), then:

   * Either the compiler may treat it as potentially throwing (depending on what you call),
   * Or you still end up with `std::terminate()` if an exception escapes.

**Best practice**:

* In teaching code, printing in destructors is fine.
* In production code, don’t do throwing work in destructors; keep them simple and `noexcept`.

---

## One subtle but important standard-library behavior (why you care)

When `std::vector<wallet>` grows, it must relocate elements. It chooses between:

* **move elements** if `wallet`’s move constructor is `noexcept`
* otherwise it may **copy** (slower) to preserve guarantees

So for performance and behavior in containers, this is the critical one:

✅ `wallet(wallet&&) noexcept` is often the difference between fast moves vs deep copies during vector growth.

---
