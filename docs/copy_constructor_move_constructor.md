# Copy and move semantics: constructors, assignment, the Rule of 5

When a class owns a resource — heap memory, a file handle, a socket, a GPU buffer, a CAN bus handle — you need to decide what happens when an object is **copied**, **moved**, or **destroyed**. Get any one of those wrong and you get double-frees, leaks, dangling pointers, or silent data corruption.

This page covers the five special member functions you'll define together, plus the surrounding machinery you can't avoid: shallow vs. deep copies, temporaries and elision, the copy-and-swap idiom, and `noexcept`.

- [1. Setup: a resource-owning class](#1-setup-a-resource-owning-class)
- [2. Shallow copy vs. deep copy](#2-shallow-copy-vs-deep-copy)
- [3. Copy constructor](#3-copy-constructor)
- [4. Copy assignment operator](#4-copy-assignment-operator)
- [5. Move constructor](#5-move-constructor)
- [6. Move assignment operator](#6-move-assignment-operator)
- [7. Rule of 0 / Rule of 3 / Rule of 5](#7-rule-of-0--rule-of-3--rule-of-5)
- [8. Copy-and-swap idiom](#8-copy-and-swap-idiom)
- [9. Temporary objects](#9-temporary-objects)
- [10. `noexcept` on moves — and why it matters](#10-noexcept-on-moves--and-why-it-matters)

---

## 1. Setup: a resource-owning class

A trajectory buffer owning a heap array. Every special-member-function example below uses this:

```cpp
class Trajectory {
public:
    Trajectory() = default;
    explicit Trajectory(std::size_t n)
        : size_{n}, data_{new double[n]} {}

    ~Trajectory() { delete[] data_; }

    // copy, copy-assign, move, move-assign — defined in the sections below

    std::size_t size() const { return size_; }
    double*     data()       { return data_; }

private:
    std::size_t size_ = 0;
    double*     data_ = nullptr;
};
```

Because `Trajectory` manually manages `data_`, the compiler-generated copy/move would do the wrong thing (see §2). We have to write all five.

## 2. Shallow copy vs. deep copy

A **shallow copy** copies pointer values, not what they point to — two objects end up referring to the same buffer.

```
   src                         dst (shallow copy)
   ┌───────────┐               ┌───────────┐
   │ size=3    │               │ size=3    │
   │ data ────►│ [a][b][c]  ◄──│──── data  │
   └───────────┘               └───────────┘
```

When either object's destructor runs, it deletes the buffer — and the other object's `data_` is dangling. Touching it is undefined behavior. The **next** destructor frees the same buffer again → double-free, a crash on glibc.

A **deep copy** allocates a fresh buffer and copies the contents:

```
   src                         dst (deep copy)
   ┌───────────┐               ┌───────────┐
   │ size=3    │               │ size=3    │
   │ data ────►│ [a][b][c]     │ data ────►│ [a][b][c]
   └───────────┘               └───────────┘
```

For raw-owning classes you have to write the deep copy yourself. For classes built from `std::vector`, `std::string`, `std::unique_ptr`, etc., the standard types' copy/move already do the right thing — that's the Rule of 0 (§7).

## 3. Copy constructor

Creates a **new** object from an existing lvalue.

```cpp
Trajectory::Trajectory(const Trajectory& other)
    : size_{other.size_}, data_{new double[other.size_]}
{
    std::copy(other.data_, other.data_ + size_, data_);
}
```

It runs in five canonical situations:

```cpp
Trajectory a(10);

Trajectory b = a;            // 1. copy-initialization
Trajectory c(a);             // 2. direct-initialization
void f(Trajectory t);        // 3. pass-by-value
f(a);                        //    — copy ctor invoked on parameter

std::vector<Trajectory> v;   // 4. container insertion
v.push_back(a);              //    — copy ctor unless we have a non-throwing move

auto lam = [a]() { /*...*/ };// 5. lambda capture by value
```

For return-by-value, copy elision (NRVO/RVO) usually skips the copy — see §9.

## 4. Copy assignment operator

Overwrites an **already-constructed** object with the contents of another.

The straightforward form, with the self-assignment check and resource swap:

```cpp
Trajectory& Trajectory::operator=(const Trajectory& other) {
    if (this == &other) return *this;            // self-assignment guard
    delete[] data_;                              // release current resource
    size_ = other.size_;
    data_ = new double[size_];                   // allocate fresh
    std::copy(other.data_, other.data_ + size_, data_);
    return *this;
}
```

Two problems with this form:
- **Not exception-safe.** If `new double[size_]` throws, you've already `delete[]`-ed the old buffer. The object is left in a half-destroyed state.
- **Easy to get wrong.** The self-assignment check, the order of operations, and the duplicated allocation logic are all easy to mis-author.

The **copy-and-swap idiom** (§8) sidesteps both.

```cpp
Trajectory a(10), b(20);
b = a;                       // copy assignment — b now mirrors a
a = a;                       // safe no-op with the guard above
v[1] = v[0];                 // copy-assignment between container elements
```

## 5. Move constructor

Creates a new object by **stealing** resources from an rvalue. The source is left in a valid but unspecified state — usually empty, so its destructor harmlessly frees nothing.

```cpp
Trajectory::Trajectory(Trajectory&& other) noexcept
    : size_{other.size_}, data_{other.data_}
{
    other.size_ = 0;
    other.data_ = nullptr;     // critical — otherwise both destructors free the same buffer
}
```

```cpp
Trajectory a(10);
Trajectory b = std::move(a);          // explicit move; a is now empty
Trajectory c = Trajectory(20);        // construction from temporary — usually elided to direct construction
Trajectory d = f(Trajectory(30));     // move into parameter, then back out
v.push_back(Trajectory(40));          // move into container
```

The `noexcept` is not decorative — see §10.

## 6. Move assignment operator

Overwrites an existing object using an rvalue's resources.

```cpp
Trajectory& Trajectory::operator=(Trajectory&& other) noexcept {
    if (this == &other) return *this;
    delete[] data_;                  // release current resource
    size_ = other.size_;
    data_ = other.data_;
    other.size_ = 0;
    other.data_ = nullptr;
    return *this;
}
```

```cpp
Trajectory a(10), b(20);
b = std::move(a);                    // b takes a's buffer; a is empty
v[1] = std::move(v[0]);              // move between container slots
```

## 7. Rule of 0 / Rule of 3 / Rule of 5

| Rule | What it says | When it applies |
|---|---|---|
| **Rule of 0** | Define **none** of the five and let the compiler generate them. | Your class is built from RAII types (`std::vector`, `std::string`, `std::unique_ptr`, smart pointers) that already handle their own copy/move/destroy. |
| **Rule of 3** (pre-C++11) | If you define any of {destructor, copy ctor, copy assign}, define **all three**. | Older code; manual resource management. |
| **Rule of 5** (C++11+) | If you define any of {destructor, copy ctor, copy assign, move ctor, move assign}, define **all five**. | A class that owns a non-RAII resource (raw pointer, FILE*, mmap handle, socket fd). |

The deeper intuition: the compiler-generated versions are correct when the **member-wise** copy/move/destroy is correct — and that's the case if every member knows how to copy/move/destroy itself. A raw owning pointer doesn't, so the moment you have one (or a manual destructor), you've broken member-wise correctness and need to provide all five.

**Rule of 0 in practice.** This trajectory class needs no special members:

```cpp
class Trajectory {
    std::vector<double> data_;       // copy/move/destroy already correct
};
```

You get correct copy, move, and destruction for free — and the compiler will be much better at optimizing them than your hand-written versions.

When you must write all five, here's a reference for `Trajectory` from above:

```cpp
class Trajectory {
public:
    Trajectory() = default;
    explicit Trajectory(std::size_t n);
    ~Trajectory();                                       // 1. destructor

    Trajectory(const Trajectory& other);                 // 2. copy ctor
    Trajectory& operator=(const Trajectory& other);      // 3. copy assign
    Trajectory(Trajectory&& other) noexcept;             // 4. move ctor
    Trajectory& operator=(Trajectory&& other) noexcept;  // 5. move assign

private:
    std::size_t size_ = 0;
    double*     data_ = nullptr;
};
```

If you want some but not all of these, be explicit:

```cpp
class NonCopyable {
public:
    NonCopyable(const NonCopyable&)            = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
    NonCopyable(NonCopyable&&) noexcept        = default;
    NonCopyable& operator=(NonCopyable&&) noexcept = default;
};
```

See [default_constructors_=default_0_delete.md](default_constructors_=default_0_delete.md) for the `= default` / `= delete` mechanics.

## 8. Copy-and-swap idiom

A clever rewrite of the copy assignment operator that gets you exception safety, self-assignment correctness, and DRY-ness with the copy constructor — all in three lines.

```cpp
class Trajectory {
public:
    // ... ctors / dtor as before ...

    // unified assignment: takes by value, so the parameter is already a copy
    Trajectory& operator=(Trajectory other) noexcept {     // takes by VALUE
        swap(*this, other);                                // member-wise swap, can't throw
        return *this;
    }                                                       // `other` (holds OLD data) dies here

    friend void swap(Trajectory& a, Trajectory& b) noexcept {
        using std::swap;
        swap(a.size_, b.size_);
        swap(a.data_, b.data_);
    }
};
```

Why this works:

- **Pass-by-value** forces a copy of the rhs *before* the function body runs. If that copy throws (e.g. `bad_alloc`), the call site sees the exception and `*this` is untouched — **strong exception guarantee**.
- The body just swaps pointers — `noexcept`. The old buffer is in `other` after the swap, and `other`'s destructor frees it at function exit.
- One implementation handles **both** copy and move assignment — when called with an rvalue, the parameter is move-constructed (free), then swapped. No duplicated logic.
- Self-assignment is fine: the copy is independent, so swapping doesn't lose anything.

The "traditional" assignment in §4 is shorter to read but harder to get right. Most modern C++ guides recommend copy-and-swap for any non-trivial owning class — and Rule of 0 when you can use it.

## 9. Temporary objects

A **temporary** is an unnamed object the compiler creates while evaluating an expression. They show up:

```cpp
// 1. Function return value
Trajectory make() { return Trajectory(7); }   // temporary returned, destroyed at end of statement
auto t = make();

// 2. Implicit conversion
void take(const Trajectory& t);
take(Trajectory(10));                          // temporary constructed for the call

// 3. Composite expressions
Complex a(1,2), b(3,4), c(5,6);
Complex sum = a + b + c;                       // (a+b) produces a temporary; that + c produces another
```

By default each temporary calls a constructor and a destructor — potentially expensive. Modern C++ erases most of that cost through **copy elision** and **move semantics**:

- **NRVO / RVO.** A function returning a value by name (`return t;`) or by prvalue (`return Trajectory(7);`) constructs the result directly in the caller's storage. No copy, no move, no temporary.
- **Mandatory elision (C++17).** Initializing from a prvalue is guaranteed elision: `Trajectory t = Trajectory(7);` is exactly one constructor call. There **is no** temporary — it's not just optimized away, the standard says it never existed.
- **Move-from-temporary.** When elision can't apply, the temporary is an rvalue, so the move constructor (if available) is invoked instead of the copy constructor.

Pre-C++17, you could observe the elision in a debugger; post-C++17, the language guarantees the program behaves as if elision happened, and a debugger can't tell the difference because there's no copy/move call to break on.

See [copy_elision.md](copy_elision.md) for the full set of cases (NRVO, RVO, mandatory elision, conditional elision).

## 10. `noexcept` on moves — and why it matters

`noexcept` is a promise: if an exception escapes the function, `std::terminate()` is called. For move operations the promise has teeth: **standard library containers will not move your objects unless the move is `noexcept`** — they'll copy instead, to preserve the strong exception guarantee during reallocation.

That means a missing `noexcept` on `Trajectory(Trajectory&&)` silently turns every `vector<Trajectory>` growth into deep copies. On a 10,000-element trajectory log, that's an O(n²) catastrophe instead of O(n).

```cpp
// ✅ Worth marking noexcept — these can't throw:
Trajectory(Trajectory&&) noexcept;            // just steals pointers
Trajectory& operator=(Trajectory&&) noexcept; // ditto
~Trajectory() noexcept;                       // delete[] is non-throwing
friend void swap(Trajectory&, Trajectory&) noexcept;

// ❌ Don't mark noexcept — these can throw bad_alloc:
Trajectory(const Trajectory&);
Trajectory& operator=(const Trajectory&);
explicit Trajectory(std::size_t);             // allocates
```

A simple checklist:

- **Add `noexcept` when** the function doesn't allocate and doesn't call anything that throws.
- **Add `noexcept` on move ops and swap** if you ever put the type in a standard container — this is the single highest-leverage `noexcept` in your codebase.
- **Don't add `noexcept` to copy ops** that allocate; let `bad_alloc` propagate.
- **If unsure, don't.** A wrong `noexcept` turns a recoverable exception into termination.

### Destructors

Destructors are **implicitly `noexcept`** in C++11+ unless something inside is potentially-throwing. You can rely on that without writing it. But:

- If a destructor *does* throw during stack unwinding (e.g. from a nested exception), `std::terminate` runs.
- Don't do throwing work in destructors — release-only operations (close, free, unlock) should be non-throwing by nature.

[code](../src/copy_constructor_move_constructor.cpp), [code](../src/copy-and-swap_idiom.cpp)
