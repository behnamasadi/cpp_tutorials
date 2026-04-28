# Branch Prediction, Prefetching, and SIMD

The CPU has a lot of compute it's willing to do for free if you stop blocking it: branch predictors, hardware prefetchers, and wide SIMD pipelines. This page covers how to feed them.

- [1. Branch Prediction](#1-branch-prediction)
- [2. Branchless Code](#2-branchless-code)
- [3. `[[likely]]` and `[[unlikely]]`](#3-likely-and-unlikely)
- [4. Hardware Prefetchers](#4-hardware-prefetchers)
- [5. Software Prefetching](#5-software-prefetching)
- [6. SIMD — What and Why](#6-simd--what-and-why)
- [7. Auto-Vectorization](#7-auto-vectorization)
- [8. Intrinsics](#8-intrinsics)
- [9. Portable SIMD: `std::simd`, ISPC, Highway, xsimd](#9-portable-simd-stdsimd-ispc-highway-xsimd)
- [10. When SIMD Doesn't Help](#10-when-simd-doesnt-help)

---

# 1. Branch Prediction

Modern CPUs guess the outcome of branches before the condition is computed and speculatively execute the predicted path. A **mispredict** flushes the pipeline — typically 10–20 cycles wasted.

Predictors are excellent at:
- **Loop branches** (taken until the last iteration).
- **Strongly biased** branches (98% taken, 2% not).
- **Patterned** branches (TNTNTN...).

They're bad at:
- 50/50 random branches.
- Indirect branches with many targets (virtual calls in a polymorphic loop with mixed types).
- Many branches in a tight loop with subtle data-dependent patterns.

You can usually hit ~99% prediction accuracy on well-structured code. <95% in a hot loop is a flag.

# 2. Branchless Code

Replacing a branch with arithmetic or `cmov`:

```cpp
// Branchy
int abs_a = (a < 0) ? -a : a;

// Branchless (compiler usually emits cmov)
int abs_a = (a ^ (a >> 31)) - (a >> 31);
```

Modern compilers will usually emit `cmov` for simple ternaries themselves. The case where you have to help: data-dependent loop bodies.

```cpp
// Branchy: predicate inside loop, the compiler usually keeps the branch
int sum = 0;
for (int x : v) if (x > threshold) sum += x;

// Branchless: arithmetic mask
int sum = 0;
for (int x : v) sum += (x > threshold) * x;
```

When the predicate is genuinely 50/50 and the body is small, branchless wins; when biased, the branch wins (the unlikely path is essentially free under prediction). **Measure**, do not assume.

# 3. `[[likely]]` and `[[unlikely]]`

C++20 attributes. Hint to the compiler about which branch is predicted to dominate. Affects code layout (the likely path is straight-line; the unlikely path is moved to a cold cache line).

```cpp
if (state == kHotPath) [[likely]] {
    fast_thing();
} else [[unlikely]] {
    slow_diagnostic();
}
```

Useful in error checks (the error path is unlikely) and in fast paths through complex code. **Do not** sprinkle them throughout — wrong hints degrade performance.

GCC/Clang historically used `__builtin_expect`; the C++20 attribute is now portable.

# 4. Hardware Prefetchers

Modern CPUs have multiple prefetcher units:
- **Stream prefetcher** — detects sequential access and pulls ahead.
- **Stride prefetcher** — detects fixed-stride access (e.g., reading every 8th element).
- **Adjacent-line prefetcher** — fetches the next line speculatively.

These work invisibly. You benefit by:
- Sequentially walking arrays. The stream prefetcher has cache lines ready before you read them.
- Avoiding pointer chasing — the prefetcher cannot follow links.

What hardware prefetchers can't do:
- Follow pointer-based linked structures.
- Predict access through index arrays (`a[indices[i]]`).
- Cross page boundaries (TLB-related limits).

# 5. Software Prefetching

When the access pattern is computable but too complex for the hardware:

```cpp
for (size_t i = 0; i < n; ++i) {
    if (i + 16 < n) __builtin_prefetch(&data[indices[i + 16]], 0, 1);
    auto& x = data[indices[i]];
    process(x);
}
```

Arguments to `__builtin_prefetch(addr, rw, locality)`:
- `rw`: 0 = read, 1 = write
- `locality`: 0 = no temporal, 3 = high temporal locality

`_mm_prefetch` is the portable intrinsic. Use software prefetching for:
- Hash-table lookups (the bucket is computable).
- B-tree / BVH traversal (next node computable from current).
- Indirect-indexed loops.

Pitfalls: prefetching too early evicts useful data; prefetching too late doesn't help. Tune by benchmarking with the actual cache size.

# 6. SIMD — What and Why

Single Instruction, Multiple Data — one instruction operates on a register holding several values. On x86:

| ISA | Width | When |
|---|---|---|
| SSE2 | 128 bits (4×float) | Anywhere; baseline x86-64 |
| AVX | 256 bits (8×float) | Sandy Bridge+ (2011) |
| AVX-512 | 512 bits (16×float) | Skylake-X+, server-class |
| ARM NEON | 128 bits | All ARMv8 |
| ARM SVE / SVE2 | Variable (128–2048) | Neoverse, recent ARMv9 |

A loop that does 16 floats in one AVX-512 instruction is — in arithmetic — 16× the throughput. In practice it's often 4–8× because of memory bandwidth and overhead.

# 7. Auto-Vectorization

The compiler will vectorize loops that meet certain conditions:

```cpp
// Vectorizable
for (size_t i = 0; i < n; ++i) c[i] = a[i] + b[i];

// Not vectorizable — loop-carried dependency
for (size_t i = 1; i < n; ++i) a[i] = a[i-1] + 1;

// Not vectorizable — pointer aliasing unknown
void add(float* a, float* b, float* c, size_t n) {
    for (size_t i = 0; i < n; ++i) c[i] = a[i] + b[i];
}
```

Help the compiler:

- `-O3` or `-O2 -ftree-vectorize` (GCC/Clang) — turn it on.
- `-march=native` or specific (`-march=skylake-avx512`) — let it use the widest available ISA.
- `__restrict` on pointer parameters — promise no aliasing.
- Indexed access with simple bounds — no early exits inside the loop.
- `#pragma omp simd` or `#pragma GCC ivdep` — hint when you know it's safe.

Check the result with `-fopt-info-vec` or by reading the asm with [godbolt.org](https://godbolt.org/).

# 8. Intrinsics

When you need to be sure, write SIMD by hand:

```cpp
#include <immintrin.h>

void add_avx(const float* a, const float* b, float* c, size_t n) {
    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);
        __m256 vb = _mm256_loadu_ps(b + i);
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(c + i, vc);
    }
    for (; i < n; ++i) c[i] = a[i] + b[i];
}
```

Pros: explicit, max performance. Cons: ISA-specific, verbose, error-prone (you have to handle masks, tails, alignment).

# 9. Portable SIMD: `std::simd`, ISPC, Highway, xsimd

Writing AVX-512 + AVX2 + SSE + NEON + SVE versions of every kernel is rarely worth it. Portable SIMD libraries:

- **`std::simd`** (C++26) — finally standardized; spec is from `std::experimental::simd` (TS).
- **Google Highway** — production-quality, supports x86 (SSE/AVX/AVX-512) and ARM (NEON/SVE), compile-time dispatch.
- **xsimd** — header-only, smaller scope than Highway.
- **ISPC** — separate compiler, C-like language with explicit `varying` types; excellent code-gen.
- **Eve** — modern C++ vector library with idiomatic API.

```cpp
// std::experimental::simd, illustrative
namespace stdx = std::experimental;
using vec = stdx::native_simd<float>;
for (size_t i = 0; i < n; i += vec::size()) {
    vec va(a + i, stdx::element_aligned);
    vec vb(b + i, stdx::element_aligned);
    (va + vb).copy_to(c + i, stdx::element_aligned);
}
```

# 10. When SIMD Doesn't Help

- **Memory-bound kernels.** If you're already saturating DRAM bandwidth, more compute lanes don't help. Check with `perf stat -e cache-misses`.
- **Branchy code.** Per-lane divergence requires masking; in deeply branchy loops, the masked version may be slower than scalar.
- **Small N.** Setup costs (load, mask, tail) eat the speedup.
- **Pointer-chasing.** SIMD likes contiguous data. Linked lists are dead.
- **Dependent reductions.** Summing 1B floats vectorizes; sum-with-carry-from-previous doesn't (without algebraic reordering).

Performance gains come from compute *and* layout *and* memory together. SIMD on bad layout is a wasted optimization.

# References

- [Cache-Friendly Design](cache_friendly_design.md)
- [HPC Patterns](hpc_patterns.md)
- *Performance Analysis and Tuning on Modern CPUs*, Denis Bakhvalov.
- [Agner Fog's optimization manuals](https://www.agner.org/optimize/) — exhaustive on x86 microarchitecture.
- [Google Highway](https://github.com/google/highway), [ISPC](https://ispc.github.io/), [Eve](https://github.com/jfalcou/eve)
- [Compiler Explorer (godbolt.org)](https://godbolt.org/) — see what the compiler actually emits.
