# High-Performance Computing (HPC) Patterns

HPC is the discipline of getting close to peak hardware throughput on workloads that justify the engineering. This page covers the recurring structural patterns; the per-instruction tricks live in the other Memory & Performance docs.

- [1. The Three Walls](#1-the-three-walls)
- [2. Embarrassingly Parallel / SPMD](#2-embarrassingly-parallel--spmd)
- [3. Fork–Join](#3-forkjoin)
- [4. Work Stealing](#4-work-stealing)
- [5. Map–Reduce](#5-mapreduce)
- [6. Stencil Computations](#6-stencil-computations)
- [7. Reductions](#7-reductions)
- [8. Roofline Model — Knowing the Ceiling](#8-roofline-model--knowing-the-ceiling)
- [9. Hybrid Models: MPI + OpenMP + GPU](#9-hybrid-models-mpi--openmp--gpu)
- [10. Tooling](#10-tooling)

---

# 1. The Three Walls

Every HPC project hits at least one of:

- **Compute wall** — the algorithm is FLOPS-bound. Solution: vectorize, parallelize across cores/sockets/nodes/GPUs.
- **Memory-bandwidth wall** — moving data between DRAM and cores is the bottleneck. Solution: better layout, blocking/tiling, in-place algorithms, fewer passes.
- **Communication wall** — MPI / network / PCIe traffic dominates. Solution: overlap communication with computation, fewer/larger messages, better partitioning.

Most numerical kernels (BLAS-2 like matvec) are bandwidth-bound. Most BLAS-3 kernels (matmul) can be made compute-bound with blocking. Always characterize where you are before optimizing.

# 2. Embarrassingly Parallel / SPMD

When tasks are fully independent — Monte Carlo trials, image filters, particle updates — split the input and run on N threads:

```cpp
#include <execution>
std::for_each(std::execution::par_unseq, items.begin(), items.end(), update);
```

Or with OpenMP:
```cpp
#pragma omp parallel for
for (size_t i = 0; i < n; ++i) result[i] = compute(input[i]);
```

This is the easiest speedup in HPC and often the largest. Scales linearly until you hit memory bandwidth or coherence costs.

SPMD ("Single Program, Multiple Data") is the same idea expressed differently — every thread runs the *same* code, partitioned by an ID. CUDA, OpenMP, MPI all use this model.

# 3. Fork–Join

Recursive divide-and-conquer. Split a problem, solve each half in parallel, combine.

```cpp
int parallel_sum(std::span<int> v) {
    if (v.size() < 4096) return std::accumulate(v.begin(), v.end(), 0);
    auto mid = v.size() / 2;
    auto fut = std::async(std::launch::async, parallel_sum, v.first(mid));
    int rhs = parallel_sum(v.last(v.size() - mid));
    return fut.get() + rhs;
}
```

Cilk, Intel TBB, and OpenMP `task` provide first-class fork-join with much lower overhead than `std::async`. The grain size (cutoff for serial execution) matters a lot — too small means overhead dominates; too large means underutilization.

# 4. Work Stealing

Each thread has its own deque of tasks. When a thread runs out, it steals from a random victim's deque. This balances irregular workloads automatically.

- TBB, Cilk, Go's runtime, Tokio, .NET ThreadPool — all use work stealing.
- Steals come from the *back* of the victim's deque to minimize cache contention with the owner who pushes/pops the front.

When work-stealing matters: irregular workloads where static partitioning would leave some cores idle. When it doesn't: regular workloads where SPMD with even chunks is optimal and has lower overhead.

See also [Concurrency Patterns: Fork-Join and Work Stealing](concurrency_patterns.md#7-forkjoin-and-work-stealing).

# 5. Map–Reduce

Two-phase: per-element transform (map), then combine (reduce):

```cpp
auto sum_of_squares = std::transform_reduce(
    std::execution::par_unseq,
    v.begin(), v.end(),
    0.0,
    std::plus{},
    [](double x){ return x * x; });
```

Distributed map-reduce (Hadoop, Spark) extends this across machines with shuffle/sort between phases.

# 6. Stencil Computations

Update each cell as a function of itself and neighbors. Used in finite-difference solvers, image filters, cellular automata.

```cpp
for (size_t i = 1; i < n - 1; ++i)
    for (size_t j = 1; j < m - 1; ++j)
        next[i][j] = 0.25 * (cur[i-1][j] + cur[i+1][j] + cur[i][j-1] + cur[i][j+1]);
```

Optimization patterns:
- **Tiling/blocking.** Process the grid in cache-sized tiles to maximize reuse.
- **Time-skewing.** Run multiple time steps within a tile before moving on.
- **Double buffering.** Read from `cur`, write to `next`, swap. Avoids in-place hazards.
- **Vectorization.** Inner loops SIMD-vectorize naturally if data is contiguous.
- **Halo exchange.** When parallelizing across nodes, each rank needs ghost rows/columns from neighbors.

# 7. Reductions

Combining many values into one (sum, max, dot product). Three concerns:

**Parallelization.** Reduce per chunk; combine chunk results:
```cpp
double parallel_sum(std::span<double> v) {
    return std::reduce(std::execution::par, v.begin(), v.end(), 0.0);
}
```

**Numerical stability.** Floating-point sum is non-associative. Different reduction orders give different results. For high precision, use Kahan summation or pairwise reduction.

**False sharing.** Per-thread accumulators must be on separate cache lines (see [False Sharing & NUMA](false_sharing_numa.md)).

# 8. Roofline Model — Knowing the Ceiling

The **roofline** model plots achievable performance against arithmetic intensity (FLOPS per byte loaded). Two limits:

- **Memory bandwidth limit:** `peak_BW * intensity` — slope going up from origin.
- **Peak compute limit:** flat horizontal line.

Your kernel's performance is bounded by `min(peak_compute, peak_BW * intensity)`. Plot both and you immediately see whether you're compute- or memory-bound, and how far below the ceiling you are.

```
GFLOPS
  |               peak compute ─────
  |             /
  |           /  ← matmul lives here (compute-bound)
  |         /
  |       /  ← matvec / stencils (BW-bound)
  |_____/
       arithmetic intensity (FLOPS/byte)
```

Tools: Intel Advisor's roofline view, NVIDIA Nsight Compute, the open-source [Empirical Roofline Toolkit](https://crd.lbl.gov/divisions/amcr/computer-science-amcr/par/research/roofline/software/ert/).

# 9. Hybrid Models: MPI + OpenMP + GPU

Real HPC apps are layered:

- **MPI between nodes.** Explicit message passing, ranks own data partitions. Halo exchanges, allreduces, broadcasts.
- **OpenMP within a node.** Threads share memory, work in parallel on the rank's chunk.
- **CUDA / HIP / SYCL on the GPU.** Offload bandwidth-and-compute-heavy kernels.

Communication patterns to know:
- **Point-to-point.** Direct send/receive between two ranks.
- **Collective.** All-reduce, broadcast, gather, scatter — implemented in MPI as `MPI_Allreduce` etc.
- **Halo exchange.** Each rank sends boundary cells to neighbors, receives ghost cells.
- **Asynchronous overlap.** `MPI_Isend`/`MPI_Irecv` allows compute and comm to run concurrently.

# 10. Tooling

| Tool | Use |
|---|---|
| **`perf` / VTune / uProf** | Per-instruction profiling, cache/branch counters |
| **likwid** | CLI for x86 hardware counters; clean roofline numbers |
| **Tracy** | In-app frame/event profiling (game-style); see [Tracy Profiler](../tracy_profiler.md) |
| **NVIDIA Nsight** | GPU kernel profiling, roofline |
| **Score-P / Vampir / TAU** | MPI + OpenMP trace analysis at HPC scale |
| **HPCToolkit** | Sampling-based whole-app profiling |
| **MAQAO** | Static + dynamic loop analysis |

Rule: instrument *before* optimizing. Most "obvious" hot spots are wrong, and most easy wins are invisible until you measure.

# References

- [Cache-Friendly Design](cache_friendly_design.md)
- [Branch Prediction and SIMD](branch_prediction_simd.md)
- [False Sharing and NUMA](false_sharing_numa.md)
- [Concurrency Patterns](concurrency_patterns.md)
- *Introduction to High Performance Computing for Scientists and Engineers*, Hager & Wellein.
- *Parallel and High-Performance Computing*, Robey & Zamora.
- [The Roofline Model paper](https://crd.lbl.gov/assets/pubs_presos/parlab08-roofline-talk.pdf), Williams/Waterman/Patterson.
