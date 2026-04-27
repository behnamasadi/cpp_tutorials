# References

- [1. Reference vs Pointer](#1-reference-vs-pointer)
- [2. Initialization Rules](#2-initialization-rules)
- [3. Lvalue and Rvalue References](#3-lvalue-and-rvalue-references)
- [4. reference_wrapper](#4-reference_wrapper)
- [5. std::ref and std::cref](#5-stdref-and-stdcref)
- [6. Type Traits: remove_reference, remove_pointer](#6-type-traits-remove_reference-remove_pointer)

---

# 1. Reference vs Pointer

A reference is an alias for an existing variable — it shares the same address.

```cpp
int foo = 2;
int& ref = foo;      // alias for foo
int* ptr = &foo;     // pointer to foo
foo++;
//          foo  ref  *ptr
// values:   3    3    3
// addresses: &foo == &ref == ptr
```

| | Reference | Pointer |
|---|---|---|
| Can be null | No | Yes |
| Can be re-bound | No (locked at init) | Yes (`p = &other`) |
| Must be initialized | Yes | No (but you should) |
| Indirection syntax | None — use like the original | `*p`, `p->x` |
| Pointer arithmetic | No | Yes |

Use a reference whenever the alias is non-null and never needs to point elsewhere. Use a pointer when null is meaningful or you may need to rebind.

# 2. Initialization Rules

A reference must bind to something at the moment it's declared, and it can never rebind:

```cpp
int x = 1, y = 2;
int& r = x;          // r aliases x
r = y;               // ✅ this assigns y's value into x — does NOT rebind r
```

Errors that look like rebinding are actually assignments to the referent.

You can't bind a non-`const` lvalue reference to a temporary:

```cpp
int& r = 200;        // ❌ 200 is an rvalue
const int& cr = 200; // ✅ const lvalue ref can extend a temporary's lifetime
```

A reference must be initialized at declaration:

```cpp
int& r;              // ❌ uninitialized reference — compile error
```

# 3. Lvalue and Rvalue References

An **lvalue** is something with an identifiable memory location — variables, dereferenced pointers, things you can take the address of. An **rvalue** is a temporary or literal: `42`, `a + b`, `std::string("hi")`.

| Reference type | Binds to |
|---|---|
| `T&` (lvalue ref) | lvalues only |
| `const T&` | lvalues, rvalues, temporaries (extends lifetime of the temporary) |
| `T&&` (rvalue ref) | rvalues only — used for move semantics |

```cpp
int x = 10;

int&        a = x;          // ✅ lvalue → lvalue ref
const int&  b = x;          // ✅
const int&  c = 42;         // ✅ const ref extends lifetime of 42
int&&       d = 42;         // ✅ rvalue ref binds to literal
int&&       e = x;          // ❌ rvalue ref can't bind to lvalue
int&&       f = std::move(x); // ✅ std::move turns x into an rvalue
```

Rvalue references enable **move semantics** — you can "steal" the resources of a temporary that's about to be destroyed anyway:

```cpp
std::string a = "hello";
std::string b = std::move(a);   // b takes a's buffer; a is empty but valid
```

# 4. reference_wrapper

Containers can't hold raw references directly:

```cpp
std::vector<int&> v;          // ❌ won't compile — references aren't assignable
```

Two reasons references can't be in containers: (a) every element must be initialized at construction, but vectors append later; (b) `vector` requires assignment (`v[0] = v[1]`), but references can't be reassigned.

`std::reference_wrapper<T>` (`<functional>`) is a class that *behaves* like `T&` but is copyable and assignable, so it works in containers:

```cpp
#include <functional>
#include <vector>

int x = 1, y = 2, z = 3;
std::vector<std::reference_wrapper<int>> v{x, y, z};

v[0].get() = 100;     // .get() exposes the underlying T&
std::cout << x;       // 100 — modified through the wrapper
```

# 5. std::ref and std::cref

`std::ref(x)` and `std::cref(x)` (`<functional>`) are factory functions that build a `reference_wrapper`. They're the only way to actually bind a reference through machinery that decay-copies its arguments — `std::bind`, `std::thread`, `std::async`:

```cpp
void modify(int& n) { ++n; }

int x = 0;
std::thread t1(modify, x);            // ⚠️ binds to a *copy* of x
std::thread t2(modify, std::ref(x));  // ✅ binds to x itself
t1.join();
t2.join();
// x has been incremented exactly once
```

`std::cref` is the const variant. Used the same way when you only want to read:

```cpp
void inspect(const int& n);
auto bound = std::bind(inspect, std::cref(huge_object));
```

Worked example with `std::bind`:

```cpp
void inc(int a, int& b, const int& c) {
    std::cout << "in fn: " << a << ' ' << b << ' ' << c << '\n';
    ++a;   // local copy
    ++b;   // affects caller's variable
}

int a = 1, b = 10, c = 100;
auto f = std::bind(inc, a, std::ref(b), std::cref(c));
//                   ^   ^^^^^^^^^^^^^   ^^^^^^^^^^^^^^
//                   |   modify caller's b   read caller's c
//                   a is captured by value at bind time

a = 5; b = 50; c = 500;       // a's snapshot is frozen at 1; b and c are live refs
f();
// in fn: 1 50 500     (a is the frozen snapshot, b and c are current)
// after: a=5  b=51  c=500
```

Without `std::ref`/`std::cref`, every parameter is captured by value at bind time.

# 6. Type Traits: remove_reference, remove_pointer

`<type_traits>` lets you peel pointers and references off types in template code:

```cpp
#include <type_traits>

static_assert(std::is_same_v<int, std::remove_pointer_t<int*>>);
static_assert(std::is_same_v<int, std::remove_reference_t<int&>>);
static_assert(std::is_same_v<int, std::remove_reference_t<int&&>>);
```

The `_t` suffix is the C++14 alias for `typename ::type`. Use it.

Common companions: `std::decay_t<T>` (removes reference + cv-qualifiers and decays arrays/functions to pointers — gives you the "as-if-passed-by-value" type), `std::add_lvalue_reference_t<T>`, `std::is_reference_v<T>`.

References:
- [std::ref and std::reference_wrapper use cases](https://www.nextptr.com/tutorial/ta1441164581/stdref-and-stdreference_wrapper-common-use-cases)
- [lvalue vs rvalue references](https://www.geeksforgeeks.org/lvalues-references-and-rvalues-references-in-c-with-examples)
