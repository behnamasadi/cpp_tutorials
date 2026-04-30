# Memory Alignment in C++

## What does "alignment" mean?

The CPU does **not** read memory one byte at a time. It reads in fixed-size chunks (4, 8, or 16 bytes) starting at fixed addresses. For this to work efficiently, each value must sit at an address that's a multiple of its size.

A simple analogy: think of memory as a long row of parking spaces, each 4 bytes wide.

- A 4-byte `int` parks cleanly when it starts at address `0`, `4`, `8`, `12`, ...
- If it starts at address `2`, it straddles the boundary between two spaces. The CPU now needs **two** memory reads instead of one, and on some architectures (ARM, older x86 SIMD) an unaligned access is **illegal** and crashes the program.

This rule — "a type of size N must start at an address that's a multiple of N" — is called the type's **alignment requirement**. For built-in types it's usually equal to the size:

| Type     | `sizeof`   | `alignof` |
|----------|------------|-----------|
| `char`   | 1          | 1         |
| `short`  | 2          | 2         |
| `int`    | 4          | 4         |
| `double` | 8          | 8         |
| `void*`  | 8 (64-bit) | 8         |

## Querying alignment: `alignof`

```cpp
#include <iostream>

int main() {
  std::cout << "char:   " << alignof(char)   << '\n';  // 1
  std::cout << "int:    " << alignof(int)    << '\n';  // 4
  std::cout << "double: " << alignof(double) << '\n';  // 8
}
```

## Why structs are bigger than they look: padding

Because each member must sit at its required alignment, the compiler inserts invisible **padding bytes**.

```cpp
struct Foo {
  char c;   // 1 byte
  int  i;   // 4 bytes — but must start at an offset that's a multiple of 4
};

std::cout << sizeof(Foo);   // 8, not 5
```

Memory layout (`_` = padding):

```
offset:  0  1  2  3  4  5  6  7
        [c][_][_][_][i][i][i][i]
```

The struct's overall alignment is the **max** of its members' alignments (so `alignof(Foo) == 4`). The struct's size is rounded up to a multiple of that, so consecutive elements in an array stay aligned.

### Reordering members reduces padding

```cpp
struct Bad  { char a; int b; char c; int d; };   // sizeof == 16
struct Good { int  b; int d; char a; char c; };  // sizeof == 12
```

`Bad` layout (4 bytes wasted on each `char`):
```
[a][_][_][_][b b b b][c][_][_][_][d d d d]
```

`Good` layout (only 2 bytes of trailing padding):
```
[b b b b][d d d d][a][c][_][_]
```

Rule of thumb: **declare members from largest to smallest** to minimize padding.

## Forcing stricter alignment: `alignas`

`alignas(N)` requests that a variable or type be aligned to **at least** N bytes. `N` must be a power of two.

```cpp
alignas(32) float buffer[8];   // address is a multiple of 32

struct alignas(64) CacheLineSized {
  int data[16];                // whole struct sits in one 64-byte cache line
};

std::cout << alignof(CacheLineSized);   // 64
```

Common reasons to use `alignas`:

- **SIMD intrinsics** — SSE needs 16-byte alignment, AVX needs 32, AVX-512 needs 64.
- **Cache lines** — putting a hot variable on its own 64-byte cache line to avoid *false sharing* between threads.
- **Hardware** — memory-mapped registers and DMA buffers often demand a specific alignment.

# Eigen and alignment

The [Eigen](https://eigen.tuxfamily.org) linear algebra library is the most common place where C++ programmers bump into alignment in practice. Eigen uses SSE/AVX intrinsics for its fixed-size matrix and vector types, and those intrinsics require **16-byte** (or 32-byte for AVX) aligned operands.

## Fixed-size Eigen types are over-aligned

```cpp
#include <Eigen/Dense>
#include <iostream>

int main() {
  std::cout << "Vector4f size:  " << sizeof(Eigen::Vector4f)  << '\n';  // 16
  std::cout << "Vector4f align: " << alignof(Eigen::Vector4f) << '\n';  // 16  (not 4!)
  std::cout << "Matrix4f size:  " << sizeof(Eigen::Matrix4f)  << '\n';  // 64
  std::cout << "Matrix4f align: " << alignof(Eigen::Matrix4f) << '\n';  // 16
}
```

Note that `Vector4f` requires **16-byte alignment** even though its data is just 4 floats. That extra alignment is what lets Eigen use the fast `_mm_load_ps` SSE instruction instead of the slower unaligned load.

## The classic gotcha: structs containing Eigen members

If you put a fixed-size Eigen type inside your own class, the class inherits Eigen's 16-byte alignment requirement:

```cpp
#include <Eigen/Dense>

struct Pose {
  Eigen::Matrix4f rotation;     // needs 16-byte alignment
  Eigen::Vector4f translation;  // needs 16-byte alignment
};

static_assert(alignof(Pose) == 16);
```

That's fine on the stack and in modern C++17 `new`. But on older toolchains (or when stored in a `std::vector` compiled with C++14), `new` only guarantees alignment up to `alignof(std::max_align_t)` — usually 8 — and the over-aligned `Pose` may be misaligned. Eigen offers two helpers for this case:

```cpp
struct Pose {
  Eigen::Matrix4f rotation;
  Eigen::Vector4f translation;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW   // overrides operator new to honor 16-byte alignment
};

// For containers, use Eigen's aligned_allocator:
std::vector<Pose, Eigen::aligned_allocator<Pose>> poses;
```

> **Modern note:** C++17 made `new` alignment-aware, and Eigen 3.4+ exploits this. In modern code these macros are usually unnecessary — but you'll still see them everywhere in older codebases.

## Avoiding the issue: dynamic-size types

If you don't need the SIMD speedup or want to skip the alignment dance, use **dynamic-sized** Eigen types. They allocate on the heap and handle their own alignment internally:

```cpp
Eigen::MatrixXf m(4, 4);   // no alignment fuss
Eigen::VectorXf v(4);
```

Trade-off: slightly slower for tiny matrices because of the heap allocation and missed SIMD opportunities, but completely safe to put inside any struct or container.

## When does Eigen actually crash?

A misaligned `Vector4f` triggers an assertion like:

```
Eigen/src/Core/DenseStorage.h:128: Assertion `(internal::UIntPtr(array) & (sizemask)) == 0`
"this assertion is explained here: http://eigen.tuxfamily.org/dox-devel/group__TopicUnalignedArrayAssert.html"
```

This is Eigen catching the bug for you in debug builds. In release builds (without the assertion), an unaligned SSE load on x86 may silently work but slowly; on ARM with NEON it segfaults.

# Cache lines vs alignment

Alignment and cache lines are **related but distinct** concepts:

- **Alignment** is about *where* a value can start: at addresses divisible by N.
- **Cache lines** are the unit the CPU transfers between RAM and cache — typically 64 bytes.

Aligning to a cache line (`alignas(64)`) is a stronger requirement than the type itself needs. You'd do this to:

1. Keep a hot value alone on its line (avoid false sharing between threads).
2. Ensure SIMD loads don't straddle two cache lines.

For a deeper treatment, see [False Sharing and Cache Lines](system_design/false_sharing_numa.md) and [Cache-Friendly Design](system_design/cache_friendly_design.md).

You can find the cache line size at runtime on Linux:

```cpp
#include <unistd.h>
#include <iostream>

int main() {
  std::cout << sysconf(_SC_LEVEL1_DCACHE_LINESIZE) << '\n';   // typically 64
}
```

Or from the shell: `getconf LEVEL1_DCACHE_LINESIZE`.

# References

- [cppreference: alignas / alignof](https://en.cppreference.com/w/cpp/language/alignas)
- [Eigen: alignment issues FAQ](https://eigen.tuxfamily.org/dox/group__TopicUnalignedArrayAssert.html)
- [Eigen: passing fixed-size types by value](https://eigen.tuxfamily.org/dox/group__TopicPassingByValue.html)

[source code](../src/align.cpp)
