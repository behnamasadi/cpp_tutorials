# Exception Safety Guarantees

Exception safety is a contract about what your code promises *if an exception is thrown*. There are four widely used levels, codified by Abrahams in the late 1990s and adopted by the C++ standard library.

- [1. The Four Levels](#1-the-four-levels)
- [2. Why It Matters Even Without Exceptions](#2-why-it-matters-even-without-exceptions)
- [3. Achieving the Strong Guarantee — Copy-and-Swap](#3-achieving-the-strong-guarantee--copy-and-swap)
- [4. `noexcept` and the Move Question](#4-noexcept-and-the-move-question)
- [5. Common Failure Modes](#5-common-failure-modes)
- [6. Quick Reference](#6-quick-reference)

---

# 1. The Four Levels

| Level | Promise on exception |
|---|---|
| **No-throw** | Operation will not throw, ever. Marked `noexcept`. |
| **Strong** | Operation either succeeds, or rolls back to the prior state — no observable change. |
| **Basic** | No leaks, no broken invariants, but state may have changed. |
| **None** | All bets off; usually a bug. |

Examples from the standard library:

| Operation | Guarantee |
|---|---|
| `std::vector<T>::push_back` (with throwing copy) | Strong |
| `std::vector<T>::push_back` (with throwing move) | Basic |
| `std::vector<T>::push_back` (with `noexcept` move) | Strong |
| `std::swap` of trivially-movable types | No-throw |
| `std::map::insert` | Strong |
| `std::sort` | Basic (with throwing comparator) |
| Destructors (everywhere) | No-throw (mandated by convention) |

# 2. Why It Matters Even Without Exceptions

If you build with `-fno-exceptions`, you might assume this is moot. It isn't:

- "Strong" generalizes to *any* failure path. Replace "throws" with "returns error" or "aborts" — the same reasoning applies to writing code that fails cleanly.
- Standard containers may still call `std::terminate` on allocation failure, which has the same observable effect on your invariants.
- Exception-safety analysis surfaces **invariant-management bugs** that exist whether or not exceptions are enabled.

# 3. Achieving the Strong Guarantee — Copy-and-Swap

The classic recipe: do the work on a copy, then swap. If anything fails, the original is untouched.

```cpp
#include <algorithm>
#include <cstddef>

class Buffer {
    char* data = nullptr;
    std::size_t size = 0;
public:
    void assign(const char* src, std::size_t n) {
        char* tmp = new char[n];        // can throw — original untouched
        std::copy_n(src, n, tmp);       // can throw — but we'd leak tmp here
        delete[] data;                  // commit point — no-throw from here
        data = tmp;
        size = n;
    }
    ~Buffer() { delete[] data; }
};

int main() {
    Buffer b;
    b.assign("hello", 5);
}
```

The pattern: **all throwing work first, then a no-throw commit step**. Any exception during the work leaves the original state intact. See [Copy-and-Swap idiom](../copy-and-swap_idiom.md).

# 4. `noexcept` and the Move Question

`noexcept` is part of a function's type, not just a hint. Standard containers *test* whether your move constructor is `noexcept` and behave differently:

```cpp
// Inside std::vector, conceptually:
void vector_push_back_grow() {
    if (std::is_nothrow_move_constructible_v<T>) {
        // move elements into new buffer — strong guarantee preserved
    } else {
        // copy elements (slower!) to keep the strong guarantee
    }
}
```

**A non-`noexcept` move constructor silently makes your code slower.** Mark moves `noexcept` whenever you can — and you almost always can if your members are.

```cpp
class Connection {
public:
    Connection() = default;
    Connection(Connection&&) noexcept = default;            // good
    Connection& operator=(Connection&&) noexcept = default; // good
    ~Connection() = default;                                // already noexcept implicitly
};
```

Pitfall: `noexcept` lies are checked at runtime. If a `noexcept` function does throw, the program calls `std::terminate`. Don't add it to functions you haven't actually verified.

# 5. Common Failure Modes

**Throw from a destructor.** During stack unwinding, a second exception calls `std::terminate`. Destructors must be no-throw. Period.

**Two-step construction without rollback.** `obj_ = new X; init(obj_);` — if `init` throws, `obj_` leaks. Use RAII (`std::unique_ptr`) or do the work in the constructor.

**Self-assignment in copy assignment.**
```cpp
T& operator=(const T& other) {
    delete data;                  // oops — if &other == this, we just freed the source
    data = new int(*other.data);
    return *this;
}
```
Use copy-and-swap or check `&o != this`.

**Holding a mutex through a throwing operation without RAII.** Use `std::scoped_lock`/`std::lock_guard`, never `mtx.lock()`/`mtx.unlock()` by hand.

**Throwing while iterating an STL container.** If your comparator or predicate throws, the container is *valid* but the operation may have done partial work. Document or test which guarantee your callbacks need to provide.

**Mixing exceptions with `errno` / C APIs.** A C function that returns -1 + sets `errno` will not unwind. Wrap it in an RAII translator:

```cpp
#include <cerrno>
#include <cstdio>
#include <system_error>

struct File {
    FILE* fp;
    File(const char* path) : fp(std::fopen(path, "r")) {
        if (!fp) throw std::system_error(errno, std::generic_category(), path);
    }
    ~File() { if (fp) std::fclose(fp); }
};

int main() {
    try {
        File f("/etc/hostname");   // throws std::system_error if missing
    } catch (const std::system_error& e) {
        // handle error
    }
}
```

# 6. Quick Reference

| Want | Do |
|---|---|
| Strong guarantee | Copy-and-swap, or do throwing work on a temporary, then no-throw commit |
| No-throw guarantee | Mark `noexcept`, prove it, never call throwing code from inside |
| Container speed | Make moves and `swap` `noexcept` |
| Survive partial work | RAII for every owning resource |
| Translate C errors | RAII wrapper that throws `std::system_error` in constructor |

# References

- *Exceptional C++*, Herb Sutter — the foundational reference.
- *Generic<Programming>: Exception-Safety Analysis*, Andrei Alexandrescu.
- [Copy-and-Swap idiom](../copy-and-swap_idiom.md)
- [RAII](../RAII.md)
- [Stack Unwinding](../stack_unwinding.md)
- [Error Handling Strategies](error_handling_strategies.md)
