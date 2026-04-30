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
alignas(32) float buffer[8];   // &buffer[0] is a multiple of 32

struct alignas(64) CacheLineSized {
  int data[16];                // whole struct sits in one 64-byte cache line
};

std::cout << alignof(CacheLineSized);   // 64
```

### What does `alignas` apply to?

`alignas(N)` aligns the **whole object** (the array, in the example above) — only the first element gets the guarantee. Individual elements still keep their natural alignment.

For `alignas(32) float buffer[8]`, if `&buffer[0] == 0x1000` (a multiple of 32):

```text
buffer[0]  at 0x1000     ✅ 32-aligned (multiple of 32)
buffer[1]  at 0x1004     ❌ not 32-aligned (just 4-aligned, the natural alignment of float)
buffer[2]  at 0x1008     ❌ not 32-aligned
...
buffer[7]  at 0x101C     ❌ not 32-aligned
[end]      at 0x1020     ← 32 bytes total, fits exactly in one AVX vector
```

That's exactly what AVX wants: a 32-byte block at a 32-aligned address. One `_mm256_load_ps(buffer)` loads all 8 floats with the *aligned* (fast) instruction. Loading `buffer + 1` would need the unaligned variant `_mm256_loadu_ps` — slower on older CPUs, and a crash on architectures that don't tolerate unaligned vector loads.

If you needed *every* element to be 32-aligned, you'd need an array of `alignas(32) float`-wrappers — but that's almost never what you want. The whole point of aligning a buffer is so that *one* SIMD load can pull in the whole batch.

### What `alignas(64)` on a struct gets you

```cpp
struct alignas(64) CacheLineSized {
  int data[16];   // 16 × 4 = 64 bytes
};
```

Three things are happening together:

1. **`alignas(64)`** forces the struct to start at an address that's a multiple of 64. On most modern CPUs the cache line size is 64 bytes, so the struct begins at a cache-line boundary.
2. **`int data[16]`** is exactly 64 bytes.
3. Therefore the **entire struct fits in one cache line** — never split across two.

The CPU reads from RAM in 64-byte chunks (one cache line). If your struct straddled two cache lines, reading it would cost **two** memory fetches instead of one.

With `alignas(64)`:

```text
cache line:  | 0x1000 ─────────── 0x103F | 0x1040 ─────────── 0x107F |
              └─ obj fits here ──────────┘
```

Without it (natural alignment is `alignof(int) == 4`, so the struct could land anywhere 4-aligned):

```text
cache line:  | 0x1000 ─────────── 0x103F | 0x1040 ─────────── 0x107F |
                              └─ obj straddles boundary ─┘
                              ❌ two cache-line fetches needed
```

`alignas` also affects `sizeof`: if your struct's natural size is less than the alignment, the compiler pads it up so an *array* of these stays aligned element-to-element. For `CacheLineSized` it's already 64, so no extra padding — but `struct alignas(64) X { int x; }` would have `sizeof(X) == 64` (60 bytes of trailing padding).

### The false-sharing use case

The most common real-world reason to align a struct to 64 bytes is to keep multi-threaded counters from clobbering each other's cache:

```cpp
struct alignas(64) PerThreadCounter {
  std::atomic<long> value;
  // ~56 bytes of trailing padding, automatically inserted by alignas
};

PerThreadCounter counters[8];   // each on its own cache line
```

Without the `alignas`, two adjacent atomics would share a cache line, and every write by one thread would invalidate the other thread's cached copy — the **false sharing** problem. See [false_sharing_numa.md](system_design/false_sharing_numa.md) for the full story.

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

# Cache Lines

Alignment and cache lines are **related but distinct** concepts:

- **Alignment** is about *where* a value can start: at addresses divisible by N.
- **Cache lines** are the unit the CPU transfers between RAM and cache — typically 64 bytes on modern x86 and ARM.

You can't usefully think about high-performance C++ without understanding cache lines, because they're what makes "alignment" matter in practice.

## What's actually happening when you read memory

The CPU does **not** read individual bytes from RAM. It reads in chunks of 64 bytes (one cache line) into the L1 cache, then serves your individual reads from the cache. Even if your code reads a single `int`, the CPU has loaded the surrounding 64 bytes — they're now "free" to access.

This single fact drives most of cache-aware programming:

```text
Memory                                          L1 Cache
┌───┬───┬───┬───┬───┬───┬───┬───┐              ┌───┬───┬───┬───┬───┬───┬───┬───┐
│   │   │ X │   │   │   │   │   │   ───►       │   │   │ X │   │   │   │   │   │
└───┴───┴───┴───┴───┴───┴───┴───┘              └───┴───┴───┴───┴───┴───┴───┴───┘
                ^                              ^   ^   ^   ^   ^   ^   ^   ^
read int X      │                              all 16 ints in this 64-byte
                read 4 bytes                   line are now cached
```

A read that's already in cache is **~100× faster** than one that has to go to RAM. So:

- **Reading sequentially** through an array is fast: one RAM fetch loads 16 ints, the next 15 reads are cache hits.
- **Reading scattered memory** (linked list, random pointer chasing) is slow: most reads miss the cache.

## Cache lines and `alignas`

Two things go wrong if you ignore cache lines:

### Problem 1: A struct straddling two cache lines

If a 24-byte struct lands at offset 56 of a 64-byte line, it occupies bytes 56–63 of one line and 0–15 of the next. Reading it costs **two** cache-line fetches instead of one.

```cpp
struct Foo {
  int a;
  double b;
  int c;
};                           // sizeof == 24, alignof == 8

Foo f;                       // could land at any 8-aligned address
```

Forcing it onto a 64-byte boundary (or padding it to a multiple of 64 in arrays) keeps each instance in one line:

```cpp
struct alignas(64) Foo {
  int a;
  double b;
  int c;
};                           // sizeof == 64 (40 bytes of trailing padding)
```

Trade-off: arrays of `Foo` now use 64 bytes per element instead of 24. Worth it only if random access dominates and cache misses on this struct are measurable.

### Problem 2: False sharing between threads

Even worse: two threads writing to *different variables that share a cache line* will fight each other. Every write invalidates the other thread's cached copy of the line, forcing it to re-fetch — even though they aren't actually contending on the same bytes.

```cpp
struct Counters {
  std::atomic<long> a;       // ~56 bytes apart in memory, but...
  std::atomic<long> b;       // ...both live on the SAME 64-byte cache line
};

Counters c;
// thread 1: hammers c.a
// thread 2: hammers c.b
// performance is much worse than two separate atomics — that's false sharing
```

Fix: pad each counter onto its own cache line.

```cpp
struct Counters {
  alignas(64) std::atomic<long> a;
  alignas(64) std::atomic<long> b;
};                           // sizeof == 128: each atomic on its own line
```

Now thread 1's writes to `a` don't invalidate thread 2's view of `b`. The dedicated [false_sharing_numa.md](system_design/false_sharing_numa.md) doc has a benchmark showing this can be a 5–10× speedup on a contended workload.

## Detecting the cache line size

The cache line size is a hardware property — typically 64, sometimes 128 (Apple Silicon's L2). You can query it three ways:

### At compile time (C++17)

```cpp
#include <new>

constexpr std::size_t L = std::hardware_destructive_interference_size;
// minimum offset between two objects that won't false-share. Usually 64.

struct Counters {
  alignas(L) std::atomic<long> a;
  alignas(L) std::atomic<long> b;
};
```

This is the portable, future-proof choice for false-sharing prevention.

### At runtime (Linux)

```cpp
#include <unistd.h>
#include <iostream>

int main() {
  std::cout << sysconf(_SC_LEVEL1_DCACHE_LINESIZE) << '\n';   // typically 64
}
```

### From the shell

```bash
getconf LEVEL1_DCACHE_LINESIZE       # Linux
sysctl -n hw.cachelinesize           # macOS
```

## Cache lines vs alignment — the relationship

| Concept | Question it answers | Typical value |
|---|---|---|
| Type alignment (`alignof(T)`) | "Where can `T` legally start?" | 1, 4, 8, 16 |
| Cache line size | "What's the unit of memory transfer?" | 64 |
| `alignas(64)` | "Force this object onto a cache-line boundary" | 64 |

Alignment is a **language-level** rule — the CPU may segfault if you violate it. Cache-line size is a **performance** concern — get it wrong and your code still runs, just slowly. `alignas(64)` is the bridge: a language-level mechanism you use for performance reasons.

For a deeper treatment with benchmarks and NUMA considerations, see [False Sharing and Cache Lines](system_design/false_sharing_numa.md) and [Cache-Friendly Design](system_design/cache_friendly_design.md).

# References

- [cppreference: alignas / alignof](https://en.cppreference.com/w/cpp/language/alignas)
- [Eigen: alignment issues FAQ](https://eigen.tuxfamily.org/dox/group__TopicUnalignedArrayAssert.html)
- [Eigen: passing fixed-size types by value](https://eigen.tuxfamily.org/dox/group__TopicPassingByValue.html)

[source code](../src/align.cpp)
