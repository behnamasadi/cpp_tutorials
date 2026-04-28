# Cache-Friendly Data Structures and Data-Oriented Design

Modern CPUs are bottlenecked on memory, not compute. A cache miss to DRAM costs ~200–300 cycles — enough time to do hundreds of arithmetic ops. The single most impactful performance lever for most code is **stop missing the cache**.

- [1. The Memory Hierarchy](#1-the-memory-hierarchy)
- [2. Data-Oriented Design (DOD)](#2-data-oriented-design-dod)
- [3. AoS vs SoA](#3-aos-vs-soa)
- [4. Hot/Cold Splitting](#4-hotcold-splitting)
- [5. Contiguous Containers](#5-contiguous-containers)
- [6. Indices Beat Pointers](#6-indices-beat-pointers)
- [7. Sentinels and Branch-Free Iteration](#7-sentinels-and-branch-free-iteration)
- [8. Prefetching](#8-prefetching)
- [9. Measuring](#9-measuring)
- [10. Quick Reference](#10-quick-reference)

---

# 1. The Memory Hierarchy

Approximate latencies on a modern x86 server (Skylake-X class):

| Level | Latency | Size |
|---|---|---|
| L1 cache | ~4 cycles (~1 ns) | 32 KB / core |
| L2 cache | ~12 cycles (~4 ns) | 256 KB–1 MB / core |
| L3 cache | ~40 cycles (~12 ns) | 8–64 MB shared |
| DRAM | ~200–300 cycles (~80 ns) | GBs |
| NVMe SSD | ~10 μs | TBs |
| Network (same DC) | ~100 μs | — |

Two implications:
1. The price of an L1 hit vs an L3 miss is ~80×. *Layout dwarfs algorithmic constants below ~1000 elements.*
2. Branch mispredicts are ~15 cycles, function calls add overhead — but a single L3 miss costs more than dozens of branches.

# 2. Data-Oriented Design (DOD)

DOD is a counter-philosophy to OOP. Instead of "what does this object do," ask: "what is this transformation, and what's the layout that lets the CPU run through it fast?" Popularized in game engines (Mike Acton's *Data-Oriented Design and C++* talk is the foundational text).

The mental model:

> The CPU is a loop on contiguous arrays of bytes. Code is a way to describe what to do at each iteration. Make the data shape match the access shape.

# 3. AoS vs SoA

Two ways to store a million particles, each with `position`, `velocity`, `color`, `mass`:

**Array of Structures (AoS):**
```cpp
struct Particle { vec3 pos, vel; vec4 color; float mass; };
std::vector<Particle> particles;     // every read pulls all fields into cache
```

**Structure of Arrays (SoA):**
```cpp
struct Particles {
    std::vector<vec3>  positions;
    std::vector<vec3>  velocities;
    std::vector<vec4>  colors;
    std::vector<float> masses;
};
```

If you only need `positions` and `velocities` (e.g., physics step), SoA loads only those fields — typically 2–4× the throughput. AoS is friendlier when you always touch the whole record.

Modern compilers (with ISPC, or pragma directives) can vectorize SoA loops trivially; AoS often falls out of vectorization because the strides don't fit SIMD lanes.

# 4. Hot/Cold Splitting

Most types have a few "hot" fields (touched in tight loops) and many "cold" fields (touched rarely). Split them:

```cpp
// Bad — every cache line that holds a player's HP also drags in inventory + name + ...
struct Player {
    float hp;
    Inventory inventory;
    std::string name;
    QuestLog quests;
};

// Better — hot data is dense.
struct PlayerHot { float hp; uint16_t cold_index; };
struct PlayerCold { Inventory inv; std::string name; QuestLog q; };

std::vector<PlayerHot>  hot;
std::vector<PlayerCold> cold;
```

A loop that updates HP touches only `hot[]` — 2 bytes per player vs hundreds. The cold table is paid for only on the rare access.

# 5. Contiguous Containers

In ascending order of cache-friendliness:

| Container | Layout | When to use |
|---|---|---|
| `std::vector` | One block | Default; almost always |
| `std::array` | One block on stack | Known fixed size |
| `std::deque` | Chunked blocks | Push/pop both ends |
| `std::flat_map` (C++23) | Sorted vector | Small map, mostly reads |
| `std::list` | Pointer-chained nodes | Almost never (O(1) splice excluded) |
| `std::map` / `std::set` | Tree nodes, scattered | Avoid for hot paths — use sorted vector |
| `std::unordered_map` | Buckets + chains | OK for big tables; chains hurt locality |

The lesson: **default to `std::vector`**. Other containers should justify themselves with a measured win.

# 6. Indices Beat Pointers

In a `Node*`-chained structure, the CPU pointer-chases through unpredictable memory. Indices into a contiguous backing array are predictable and pre-fetchable:

```cpp
// Pointer-chasing — slow
struct Node { Node* next; int v; };

// Indexed — fast, also smaller (4 bytes vs 8 for the link)
std::vector<int> values;
std::vector<int> next_index;   // -1 for end
```

Bonus: indices are stable across reallocation (relative offsets) and can be 32-bit even on 64-bit machines.

# 7. Sentinels and Branch-Free Iteration

Branch mispredicts cost cycles. Sentinels remove "did we run off the end?" checks:

```cpp
// Branchy
for (size_t i = 0; i < n; ++i) { if (a[i] == target) return i; }

// Sentinel-based
a[n] = target;            // ensure a hit
size_t i = 0;
while (a[i] != target) ++i;
return i;                 // == n if not found
```

Modern compilers can sometimes do this for you; sometimes they can't. Profile.

# 8. Prefetching

When the next access is computable but far ahead, hint the CPU to start loading it:

```cpp
for (size_t i = 0; i < n; ++i) {
    __builtin_prefetch(&data[i + 16], 0, 1);   // GCC/Clang
    process(data[i]);
}
```

`_mm_prefetch` is the portable intrinsic. Useful for index-driven structures (hash table lookups, BVH traversal, B-tree walks). Useless if you're already memory-bound on bandwidth, not latency.

# 9. Measuring

Don't guess. Tools:

- **`perf stat`** (Linux): cycles, instructions, cache-references, cache-misses, branch-misses.
  ```
  perf stat -e cache-references,cache-misses,L1-dcache-load-misses ./bench
  ```
- **`perf record` + `perf report`**: which lines are missing the cache.
- **`likwid-perfctr`**: cleaner CLI for x86 perf counters.
- **VTune** (Intel), **uProf** (AMD): GUI views of memory hierarchy events.
- **Cachegrind** (Valgrind): simulated cache miss rates, no hardware counters needed.

Headline numbers to watch:
- IPC (instructions per cycle). >2 is good, <1 usually means memory-bound.
- L1/L2/L3 miss rates. Each level miss adds latency proportional to next-level cost.
- Branch miss rate. >5% is a flag.

# 10. Quick Reference

- **Default to `std::vector`** of dense structs. Reach elsewhere with cause.
- **SoA** when hot loops touch only a subset of fields.
- **Hot/cold split** when types have many fields touched at very different frequencies.
- **Contiguous over linked**, indices over pointers.
- **Pack hot data into cache-line-sized structs** (64 bytes typical).
- **Avoid sharing cache lines between threads** — see [False Sharing & NUMA](false_sharing_numa.md).
- **Prefetch only when measured to help**; the compiler is often already doing it.
- **Measure**, don't intuit. The first cache-line you save almost always points to ten more.

# References

- *Mike Acton's CppCon 2014 talk: Data-Oriented Design and C++* — required watching.
- *What Every Programmer Should Know About Memory*, Ulrich Drepper.
- [False Sharing and NUMA](false_sharing_numa.md)
- [Branch Prediction and SIMD](branch_prediction_simd.md)
- [Memory Management Strategies](memory_management.md)
- *Computer Architecture: A Quantitative Approach*, Hennessy & Patterson — the textbook.
