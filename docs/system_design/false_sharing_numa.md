# False Sharing, Cache Line Padding, NUMA Awareness

These are the "your concurrent code is correct but slow" problems. The fixes are mechanical once you know what to look for.

- [1. Cache Coherence in 60 Seconds](#1-cache-coherence-in-60-seconds)
- [2. False Sharing](#2-false-sharing)
- [3. Detecting False Sharing](#3-detecting-false-sharing)
- [4. `std::hardware_destructive_interference_size`](#4-stdhardware_destructive_interference_size)
- [5. NUMA — What and Why](#5-numa--what-and-why)
- [6. NUMA Allocation and Pinning](#6-numa-allocation-and-pinning)
- [7. NUMA-Aware Data Structures](#7-numa-aware-data-structures)
- [8. Quick Reference](#8-quick-reference)

---

# 1. Cache Coherence in 60 Seconds

A cache line is the unit the CPU moves between memory and caches — typically 64 bytes on x86 and ARM. When core A writes to a line that core B has cached, core B's copy is invalidated; B's next read pays the cost of fetching the latest value. This is the **MESI protocol** in action (or one of its variants).

You only ever pay the cost when *different cores* want the *same line* with at least one of them writing. This is fine and intended for genuinely shared data. It's a problem when the data is logically *not shared* but happens to live on the same line.

# 2. False Sharing

```cpp
#include <atomic>

struct Counters {
  std::atomic<int> a;   // written by thread 1
  std::atomic<int> b;   // written by thread 2 — same cache line!
};
```

Both atomics likely share one cache line. Every increment by thread 1 invalidates thread 2's cached copy of `b`, and vice versa. Throughput collapses by 10–100×.

The fix: pad to the cache-line boundary.

```cpp
#include <atomic>
#include <thread>

struct alignas(64) PaddedCounter {
  std::atomic<int> v;
  char pad[64 - sizeof(std::atomic<int>)];
};

struct Counters {
  PaddedCounter a;
  PaddedCounter b;
};

int main() {
  Counters c{};
  std::thread t1([&] { for (int i = 0; i < 1'000'000; ++i) ++c.a.v; });
  std::thread t2([&] { for (int i = 0; i < 1'000'000; ++i) ++c.b.v; });
  t1.join();
  t2.join();
}
```

Or use `std::hardware_destructive_interference_size` (see §4).

A real-world case: per-thread counters in a hot histogram. If they're stored in `std::vector<std::atomic<int>>`, every `++hist[tid]` also kicks the neighboring counter out of its core's cache. Stride them out.

# 3. Detecting False Sharing

Hardware perf counters are the right tool:

```bash
# Linux, with perf
perf c2c record ./your_binary
perf c2c report
```

`perf c2c` ("cache-to-cache") shows lines that were "HITM" — read by one core after being modified by another. High HITM on a line that you didn't intend to share is false sharing.

Symptoms even without `perf`:
- Throughput drops linearly with thread count instead of scaling.
- A single-threaded run is *faster* than a multi-threaded one.
- `perf stat -e cycle_activity.stalls_l3_miss` is dominated by stalls.

# 4. `std::hardware_destructive_interference_size`

C++17 standardized two constants in `<new>`:

- `std::hardware_destructive_interference_size` — minimum guaranteed-not-to-share offset. Use to pad.
- `std::hardware_constructive_interference_size` — promoted-to-share offset. Use to pack data accessed together.

```cpp
#include <atomic>
#include <new>

struct alignas(std::hardware_destructive_interference_size) PaddedAtom {
  std::atomic<int> v;
};
```

Caveat: support varies; on some toolchains it's hard-coded to 64; others give 128 to cover prefetchers that fetch in pairs. Falling back to `alignas(64)` is fine.

# 5. NUMA — What and Why

On a multi-socket server, RAM isn't uniform. Each socket has its own memory controller and its own DIMMs. Memory attached to socket 0 is "near" cores on socket 0 and "far" from socket 1.

| Access | Latency | Bandwidth |
|---|---|---|
| Local-socket DRAM | ~80 ns | ~30 GB/s |
| Remote-socket DRAM | ~140 ns | ~12 GB/s |

A 2-socket system that ignores NUMA can run at half the throughput of a NUMA-aware one. Even on a desktop, modern AMD CPUs (Threadripper, EPYC) are internally NUMA — the chiplet/CCX boundary acts like a socket.

# 6. NUMA Allocation and Pinning

By default Linux uses *first-touch*: a page is allocated on the node of whichever CPU first writes to it. So if thread 0 (on socket 0) `malloc`s and zero-fills a buffer, then hands it to thread 1 (on socket 1), thread 1 pays remote-access costs for every read.

Tools:
- **`numactl`** — CLI for setting policy at process launch.
  ```bash
  numactl --cpunodebind=0 --membind=0 ./worker     # pin to node 0 cleanly
  numactl --interleave=all ./big_app               # spread pages, average bandwidth
  ```
- **`libnuma`** — programmatic API.
  ```cpp
  #include <numa.h>

  size_t sz = 1 << 20;                    // 1 MiB
  numa_set_preferred(1);                  // prefer node 1 for new pages
  void* p = numa_alloc_onnode(sz, 1);     // allocate on node 1
  numa_free(p, sz);
  ```
- **CPU affinity** — `pthread_setaffinity_np` / `sched_setaffinity` — pin threads to particular cores.
- **`mbind`** / **`move_pages`** syscalls — relocate pages explicitly.

Patterns:
- Per-thread allocators on the thread's local node.
- Replicate read-only data on every node.
- Partition writable data so each node owns a slice.

# 7. NUMA-Aware Data Structures

Two common shapes:

**Sharded by node.** A counter, hash table, or work queue split into N partitions, one per NUMA node. Threads access the local partition. Aggregate on demand.

```cpp
#include <atomic>
#include <vector>
#include <cstdint>

struct NumaShardedCounter {
  std::vector<std::atomic<int64_t>> per_node;  // one element per node, padded
  int64_t total() const {
    int64_t s = 0;
    for (auto& c : per_node) s += c.load();
    return s;
  }
};
```

**Replicated read-only.** Static config, lookup tables: store one copy per node; readers consult their local copy. RCU or epoch-based reclamation handles updates ([lock-free §9.3](lock_free_data_structures.md#93-rcu-and-epoch-based-reclamation)).

# 8. Quick Reference

| Symptom | Likely cause | Fix |
|---|---|---|
| Multi-threaded slower than single | False sharing | `alignas(hardware_destructive_interference_size)` per-thread data |
| Throughput plateaus past N threads on multi-socket box | NUMA cross-traffic | Pin threads, allocate locally |
| `perf c2c` shows HITM on lines you don't share | False sharing | Pad |
| Single big `mmap` with all threads writing | First-touch landed pages on one node | Touch from each thread, or `mbind` |
| Per-CPU counters slow | Atomic on shared line | Per-thread + reduce |

Padding rule of thumb: **anything written concurrently by different threads gets its own cache line.** Reading-only sharing is free; writing is the problem.

# References

- [Lock-Free Data Structures §3 (Cache Lines and False Sharing)](lock_free_data_structures.md#3-cache-lines-and-false-sharing)
- [Cache-Friendly Design](cache_friendly_design.md)
- *What Every Programmer Should Know About Memory*, Ulrich Drepper — the NUMA sections still apply.
- [Linux NUMA documentation](https://www.kernel.org/doc/html/latest/admin-guide/mm/numa_memory_policy.html)
- [Intel Memory Latency Checker](https://software.intel.com/content/www/us/en/develop/articles/intelr-memory-latency-checker.html)
- `perf c2c` man page.
