# Copy Elision and std::move

When a function returns a `std::vector<int>` of a million elements, two things *could* happen: the compiler copies the entire array into the caller, or it doesn't. Modern C++ has two distinct mechanisms for "doesn't":

- **Copy elision** — the compiler skips the copy/move entirely by constructing the value directly in the caller's storage. **Free at runtime.** Either the standard *requires* elision (since C++17) or *allows* it.
- **`std::move`** — when elision can't happen, the next-best thing: turn the would-be copy into a move, transferring ownership of internal resources (heap pointers, file handles) instead of duplicating them.

This doc covers both. They solve overlapping problems but are not interchangeable.

- [1. Why It Matters: A Concrete Example](#1-why-it-matters-a-concrete-example)
- [2. Value Categories Recap](#2-value-categories-recap)
- [3. Return Value Optimization (RVO)](#3-return-value-optimization-rvo)
- [4. Named Return Value Optimization (NRVO)](#4-named-return-value-optimization-nrvo)
  - [4.1. When NRVO fails](#41-when-nrvo-fails)
  - [4.2. The `return std::move(x)` anti-pattern](#42-the-return-stdmovex-anti-pattern)
- [5. C++17 Guaranteed Copy Elision](#5-c17-guaranteed-copy-elision)
  - [5.1. Returning immovable types](#51-returning-immovable-types)
  - [5.2. Pre-C++17 vs C++17 — what's mandatory](#52-pre-c17-vs-c17--whats-mandatory)
- [6. Copy Elision in Exception Handling](#6-copy-elision-in-exception-handling)
- [7. std::move — When Elision Can't Help](#7-stdmove--when-elision-cant-help)
  - [7.1. Transferring ownership of a large array](#71-transferring-ownership-of-a-large-array)
  - [7.2. Move semantics in one paragraph](#72-move-semantics-in-one-paragraph)
  - [7.3. Rules of thumb](#73-rules-of-thumb)
- [8. Verifying What the Compiler Does](#8-verifying-what-the-compiler-does)
- [9. See Also](#9-see-also)

---

# 1. Why It Matters: A Concrete Example

The example class below has copy/move constructors that print, so we can *see* what runs:

```cpp
#include <iostream>
#include <vector>

struct Tracer {
    std::vector<int> data;

    Tracer()                                  : data(1'000'000)         { std::cout << "  default\n"; }
    Tracer(const Tracer& o)                   : data(o.data)            { std::cout << "  copy ctor\n"; }
    Tracer(Tracer&& o) noexcept               : data(std::move(o.data)) { std::cout << "  move ctor\n"; }
    Tracer& operator=(const Tracer& o)        { data = o.data;            std::cout << "  copy assign\n"; return *this; }
    Tracer& operator=(Tracer&& o) noexcept    { data = std::move(o.data); std::cout << "  move assign\n"; return *this; }
};
```

Three differently-coded "produce a Tracer and bind it to `t`" patterns:

```cpp
Tracer make_a()           { return Tracer{};      }   // ① RVO — prvalue return
Tracer make_b()           { Tracer t; return t;   }   // ② NRVO — named local return
Tracer make_c(Tracer src) { return src;           }   // ③ neither — function parameter

Tracer t1 = make_a();            // ① "default" only — no copy or move
Tracer t2 = make_b();            // ② usually "default" only (NRVO)
Tracer t3 = make_c(Tracer{});    // ③ "default" + "move ctor" (param consumed)
```

Output on a typical compiler with default optimization (`-O0` is enough for the C++17-mandated cases):

```
①  default
②  default
③  default       (the temp passed in)
   move ctor    (return statement: param → return slot)
```

The `1'000'000`-element `std::vector` allocation never duplicates — neither for ① nor ②. ③ pays for one move (which is cheap: it transfers the vector's heap pointer; no element-by-element copy).

That's copy elision and `std::move` doing their respective jobs.

# 2. Value Categories Recap

The C++ standard splits expressions into **value categories** that determine what can be copied, moved, or elided:

- **lvalue** — has a name and identifiable address (`int x; x;`).
- **prvalue** ("pure rvalue") — a temporary or literal (`42`, `Tracer{}`, `make_a()`'s return value).
- **xvalue** ("expiring") — an lvalue someone said was OK to move from (`std::move(x)` produces one).

The core C++17 change: a **prvalue is no longer a temporary object**. It's a *recipe* for constructing one. The recipe is only "executed" (the object materialized) at the point it's bound to a name or reference. Until then, the compiler can route the same recipe directly into the caller's storage — that's the mechanism behind C++17's mandatory elision. See [§5](#5-c17-guaranteed-copy-elision).

For more on lvalue/rvalue references and `std::move`, see [references.md §3](references.md#3-lvalue-and-rvalue-references).

# 3. Return Value Optimization (RVO)

RVO applies when a function returns a **prvalue** — a temporary built right at the `return` statement.

```cpp
Tracer make() {
    return Tracer{};     // prvalue
}

Tracer x = make();       // C++17: guaranteed no copy or move
```

The compiler constructs the `Tracer` directly in the storage that `x` will occupy. Even before C++17, every mainstream compiler did this; since C++17 it's required by the standard.

This works for *any* prvalue return, including conditional ones:

```cpp
Tracer make(bool flag) {
    return flag ? Tracer{1} : Tracer{2};   // both branches yield prvalues
}
```

# 4. Named Return Value Optimization (NRVO)

NRVO applies when a function returns a **named local variable** instead of an unnamed temporary.

```cpp
Tracer make() {
    Tracer local;
    // ... maybe modify local ...
    return local;        // NRVO target
}
```

The compiler can construct `local` *itself* in the caller's return slot — no copy, no move. This is **permitted** but **not required** by the standard, even in C++17.

## 4.1. When NRVO fails

Modern compilers apply NRVO aggressively but it can be defeated. Common patterns that disable it:

```cpp
// ❌ Multiple return paths returning different named locals
Tracer make(bool flag) {
    Tracer a, b;
    return flag ? a : b;            // compiler can't pre-pick a single slot
}

// ❌ Returning a function parameter
Tracer make(Tracer src) {
    return src;                      // src isn't a local — it's a parameter slot
}                                    // → compiler emits a move, not elision

// ❌ Returning a member of an aggregate
Tracer make() {
    struct Holder { Tracer t; } h;
    return h.t;                      // not a "complete" local
}
```

In all three the compiler falls back to a move (cheap) or, if the type is non-movable, a copy. None of these are *wrong* — they just don't get the free elision.

## 4.2. The `return std::move(x)` anti-pattern

A surprisingly common mistake:

```cpp
Tracer make() {
    Tracer local;
    return std::move(local);     // ⚠️ disables NRVO, forces a move
}
```

`std::move(local)` is an xvalue, not the named lvalue `local`. The compiler is no longer in NRVO territory — it must perform an actual move. You've turned "free elision" into "guaranteed move," which is strictly worse.

Without the `std::move`:

```cpp
Tracer make() {
    Tracer local;
    return local;                // ✅ NRVO eligible — possibly zero ops
}
```

Compilers usually warn (`-Wpessimizing-move`) when they spot this, but it still slips into many codebases.

> **Rule:** never write `return std::move(local_variable);`. Returning a named local is already optimal.
>
> The exception: when the function returns a *different* type than the local, you may need an explicit conversion that requires `std::move`. Even then, prefer `return Other{std::move(local)};` so NRVO can still target the constructed `Other`.

# 5. C++17 Guaranteed Copy Elision

C++17's [P0135R1](https://wg21.link/P0135R1) reframed value categories so that returning or initializing from a **prvalue** is *guaranteed* not to copy or move — even with optimizations off, even in debug mode.

## 5.1. Returning immovable types

The most striking practical consequence: you can return types that have **deleted** copy *and* move constructors, as long as you return them as prvalues.

```cpp
struct Immovable {
    int value;
    Immovable() : value(42) {}

    Immovable(const Immovable&)            = delete;
    Immovable(Immovable&&)                 = delete;
    Immovable& operator=(const Immovable&) = delete;
    Immovable& operator=(Immovable&&)      = delete;
};

Immovable make() {
    return Immovable{};        // ✅ C++17: legal, no copy or move needed
}

int main() {
    Immovable x = make();      // ✅ also legal — direct-construction
    std::cout << x.value;
}
```

Pre-C++17 this was a hard error (the return statement formally required an accessible move constructor, even if elision would skip the actual call). C++17 changed the rule: when initializing from a prvalue, no constructor *call* is part of the model at all — the object is constructed in place.

This unlocks factory functions for types like `std::lock_guard`, `std::scoped_lock`, mutexes, file streams, RAII handles — anything you genuinely don't want copyable or movable.

NRVO does **not** get the same treatment: returning a named local of an immovable type is still ill-formed, because the standard models that as an implicit move (which is then deleted).

## 5.2. Pre-C++17 vs C++17 — what's mandatory

| Pattern | Pre-C++17 | C++17+ |
|---|---|---|
| `T x = T{};` (init from prvalue) | Permitted, not required | **Mandatory** — no constructor call |
| `return T{};` (RVO from prvalue) | Permitted, not required | **Mandatory** |
| `return local;` (NRVO from named local) | Permitted | Permitted (still not mandatory) |
| `throw T{};` / `catch (T x)` | Permitted | Permitted |

"Permitted" means a conforming compiler may insert the copy/move; "mandatory" means it can't even pretend to. The practical difference is that C++17-mandatory cases work without `-O2` and with deleted copy/move constructors.

# 6. Copy Elision in Exception Handling

When you throw and catch a typed exception, the standard permits eliding the copy of the exception object:

```cpp
try {
    throw Tracer{};         // (1) prvalue exception object
} catch (Tracer e) {        // (2) catch-by-value — initializes e from the thrown object
    use(e);
}
```

Two elisions are possible:
- The construction of the exception object at the throw site (the compiler may build it directly in the runtime's exception storage).
- The initialization of the catch parameter `e` from the exception storage.

Both are *permitted*, neither is mandated. Catching by reference is the conventional C++ practice and sidesteps the question entirely:

```cpp
catch (const Tracer& e) {   // no copy, no elision needed
    use(e);
}
```

Catch by reference unless you genuinely need to mutate the local.

# 7. std::move — When Elision Can't Help

Elision needs a return statement (or a prvalue initialization). Many real situations have neither: you're handing an object to another function, storing it in a container, or taking ownership of a parameter mid-function.

That's where `std::move` fits.

## 7.1. Transferring ownership of a large array

```cpp
#include <vector>
#include <utility>

class Buffer {
    std::vector<int> data_;          // could be megabytes
public:
    explicit Buffer(std::vector<int> v) : data_(std::move(v)) {}
    //                                   ^^^^^^^^^^^^^^^^^^
    // If we wrote `data_(v)` we'd copy every element.
    // std::move turns v into an xvalue, the vector picks the move ctor,
    // and only the heap pointer is transferred — O(1) instead of O(N).
};

std::vector<int> huge(1'000'000);
Buffer b(std::move(huge));           // huge is now empty but still valid
```

Two things to internalize:

- **`std::move` doesn't move anything.** It's a `static_cast` to rvalue reference. It changes the *value category* of the expression so that the *next* operation (constructor, assignment) chooses the move overload instead of the copy overload.
- **The moved-from object is in a "valid but unspecified" state.** You can destroy it, assign to it, or call functions with no preconditions — but reading its value is meaningless.

For a `std::vector<int>` of a million elements:
- Copy: allocates a million ints, copies them all. Microseconds, plus a heap allocation.
- Move: swaps three pointers (`begin`, `end`, `capacity`). Nanoseconds, no allocation.

## 7.2. Move semantics in one paragraph

A class that owns heap (or any external) resources should have a **move constructor** that *steals* the resource pointer from the source and a **move assignment** that does the same. The compiler synthesizes both for free if every member is moveable, which is why types built from standard containers + smart pointers "just work" without you writing anything.

```cpp
struct Owns {
    std::unique_ptr<int> p;
    std::vector<int>     v;
    // implicit move ctor: transfers p, transfers v. Nothing else needed.
};
```

When you *do* write a move constructor by hand, mark it `noexcept` — `std::vector` and other containers will use the move only if it's `noexcept`, falling back to copy otherwise (the strong exception guarantee).

## 7.3. Rules of thumb

- **Function return** of a local — return by value, no `std::move`. NRVO/RVO handles it.
- **Function parameter** by value, then storing it — `std::move` it into the member: `member_(std::move(param))`.
- **Container insert** of an existing object you're done with — `vec.push_back(std::move(obj));`.
- **Don't `std::move` a `const` object** — the cast is silently undone, and you get a copy. You lose move ergonomics without warning.
- **Don't `std::move` a return value of a non-move-aware type** — for trivially copyable `int`/`double`/etc., `std::move` is a no-op.
- **Don't write `T x = std::move(T{});`** — `T{}` is already a prvalue; `std::move` here just disables elision.

# 8. Verifying What the Compiler Does

To actually see when elision happens, turn it off and watch what would otherwise have been called:

```
g++   -std=c++17 -fno-elide-constructors prog.cpp -o prog
clang -std=c++17 -fno-elide-constructors prog.cpp -o prog
```

With the flag, the C++17-mandatory cases still elide (the standard requires them) but the *permitted-but-not-required* ones (NRVO especially) will emit their move/copy constructors. Run your `Tracer`-instrumented program with and without the flag — the diff is exactly the set of optional elisions your compiler chose.

Also useful:
- `-Wpessimizing-move` — flags `return std::move(local)`-style mistakes.
- `-Wreturn-std-move` (Clang) — suggests `std::move` for cases where it *would* help (rare).
- `-fno-elide-constructors` does not affect mandatory C++17 elision — that's by design.

# 9. See Also

- **[references.md §3](references.md#3-lvalue-and-rvalue-references)** — lvalue/rvalue references, the basis of move semantics.
- **[copy_constructor_move_constructor.md](copy_constructor_move_constructor.md)** — writing your own copy/move ctors.
- **[class_special_member_functions.md](class_special_member_functions.md)** — Rule of Five, Rule of Zero.
- **Source:** [`src/RVO_NRVO_copy_elision.cpp`](../src/RVO_NRVO_copy_elision.cpp).

References:
- [P0135R1 — Wording for guaranteed copy elision through simplified value categories](https://wg21.link/P0135R1)
- [cppreference: copy elision](https://en.cppreference.com/w/cpp/language/copy_elision)
- [C++ Core Guidelines F.45 — Don't return an `rvalue ref`](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#f45-dont-return-a-t)
