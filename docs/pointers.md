# Raw Pointers and Memory Safety

- [1. Pointer Size and Architecture](#1-pointer-size-and-architecture)
- [2. Dereferencing](#2-dereferencing)
- [3. Null Pointers](#3-null-pointers)
  - [3.1. nullptr vs NULL vs 0](#31-nullptr-vs-null-vs-0)
- [4. Wild Pointers](#4-wild-pointers)
- [5. Dangling Pointers](#5-dangling-pointers)
- [6. Memory Safety](#6-memory-safety)
  - [6.1. Common Causes of Segfaults](#61-common-causes-of-segfaults)
  - [6.2. AddressSanitizer](#62-addresssanitizer)
- [7. See Also](#7-see-also)

> This doc covers raw pointers and the bugs they enable. For ownership semantics and modern memory management see **[smart_pointers.md](smart_pointers.md)**. For aliases that share an address with another variable see **[references.md](references.md)**.

---

# 1. Pointer Size and Architecture

A pointer's size depends on the CPU's address width, **not** on what it points to:

- 32-bit machine: every pointer is 4 bytes.
- 64-bit machine: every pointer is 8 bytes.

```cpp
std::cout << sizeof(char*)   << '\n';   // 8 on x86-64
std::cout << sizeof(int*)    << '\n';   // 8
std::cout << sizeof(double*) << '\n';   // 8
std::cout << sizeof(void*)   << '\n';   // 8
```

# 2. Dereferencing

Dereferencing means "follow the address and access the value there." Three equivalent ways to read the second character of a string:

```cpp
const char* p = "abc";
char c1 = *(p + 1);   // pointer arithmetic, then dereference
char c2 = p[1];       // sugar for *(p + 1)
char c3 = *++p;       // advance p first, then dereference  ('b')
```

`p + 1` advances the address by `sizeof(*p)` bytes — `1` for `char`, `4` for `int*`, etc.

# 3. Null Pointers

A null pointer is a sentinel value meaning "doesn't point at anything." Dereferencing it is undefined behavior (typically a segfault).

```cpp
int* p = nullptr;
if (p) { /* skipped */ }
```

## 3.1. nullptr vs NULL vs 0

`NULL` is a macro that expands to `0` (or `(void*)0` in C). Pre-C++11, `NULL` was the idiomatic null pointer constant. It's a problem in overload resolution:

```cpp
void f(int);
void f(char*);

f(NULL);      // ⚠️ ambiguous on some compilers — NULL is just 0, which is an int
f(nullptr);   // ✅ unambiguous — nullptr has type std::nullptr_t
```

`nullptr` (C++11) has its own type `std::nullptr_t`, implicitly convertible to any pointer type but **not** to integer types. Always prefer `nullptr` in C++.

# 4. Wild Pointers

A *wild* pointer is one that was never initialized. Reading or writing through it accesses whatever happened to be on the stack at that address — usually garbage, sometimes a valid object you didn't mean to touch.

```cpp
int* p;        // ⚠️ wild — points wherever
*p = 10;       // undefined behavior: crash, silent corruption, or "works"
```

Always initialize:

```cpp
int* p = nullptr;
```

Then any accidental dereference fails loudly (segfault) instead of silently corrupting memory.

# 5. Dangling Pointers

A dangling pointer is a non-null pointer to memory that has already been freed.

**Heap case:**

```cpp
Object* a = new Object();
Object* b = a;          // b also points at the heap object

delete a;
a = nullptr;            // good: a is now safe to test
                        // bad: b is dangling and we have no way to know
```

**Stack case** — returning the address of a local:

```cpp
Object* make() {
    Object o;
    return &o;          // ⚠️ o is destroyed when make() returns
}

Object* p = make();
p->foo();               // undefined behavior
```

Modern compilers warn on this with `-Wreturn-stack-address`.

The structural fix is to use ownership-aware types — see [smart_pointers.md](smart_pointers.md). `std::weak_ptr` in particular gives a way to *detect* a dangling reference at runtime instead of guessing.

# 6. Memory Safety

## 6.1. Common Causes of Segfaults

A segmentation fault means the program touched a page the kernel didn't grant it. Typical causes:

1. Dereferencing a null, wild, or dangling pointer.
2. Writing to read-only memory (string literals, the code segment).
3. Reading or writing past the end of an allocation (heap or stack overflow).
4. Calling through a freed function pointer or vtable.

Note that C/C++ **don't bounds-check array indices**. An out-of-range write that happens to land on a still-mapped page won't crash — it'll silently corrupt data and surface much later.

## 6.2. AddressSanitizer

`-fsanitize=address` (ASan) instruments every load/store and reports use-after-free, buffer overflow, leaks, and stack overflow with full stack traces. Turn it on for debug builds:

```cmake
target_compile_options(my_target PRIVATE -fsanitize=address -fno-omit-frame-pointer)
target_link_options   (my_target PRIVATE -fsanitize=address)
```

Or globally:

```cmake
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
```

Slowdown is ~2× — perfectly fine for tests. Pair with `-fsanitize=undefined` (UBSan) to catch undefined-behavior bugs that don't trigger a segfault.

# 7. See Also

- **[smart_pointers.md](smart_pointers.md)** — `unique_ptr`, `shared_ptr`, `weak_ptr`, custom deleters, breaking cycles.
- **[shared_ptr_use_cases.md](shared_ptr_use_cases.md)** — twelve concrete patterns where shared ownership is the right tool.
- **[passing_returning_smart_pointers_to_from_functions.md](passing_returning_smart_pointers_to_from_functions.md)** — function-signature rules.
- **[smart_pointers_class_member.md](smart_pointers_class_member.md)** — owning resources from class members.
- **[references.md](references.md)** — references, lvalue/rvalue, `reference_wrapper`, `std::ref`/`std::cref`.

[source code](../src/pointers/pointers.cpp)
