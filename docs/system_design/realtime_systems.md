# Real-Time Systems Design

Real-time isn't "fast" — it's "predictable." A real-time system makes guarantees about *worst-case* latency. Average-case throughput is irrelevant if you ever miss a deadline.

- [1. Hard, Firm, Soft Real-Time](#1-hard-firm-soft-real-time)
- [2. Sources of Non-Determinism](#2-sources-of-non-determinism)
- [3. Memory Allocation in Real-Time Code](#3-memory-allocation-in-real-time-code)
- [4. Scheduling and Priority](#4-scheduling-and-priority)
- [5. Priority Inversion](#5-priority-inversion)
- [6. Exceptions, RTTI, Virtual Dispatch](#6-exceptions-rtti-virtual-dispatch)
- [7. Lock-Free vs Locks](#7-lock-free-vs-locks)
- [8. Worst-Case Execution Time (WCET)](#8-worst-case-execution-time-wcet)
- [9. Linux for Real-Time: PREEMPT_RT](#9-linux-for-real-time-preempt_rt)
- [10. Patterns and Anti-Patterns](#10-patterns-and-anti-patterns)

---

# 1. Hard, Firm, Soft Real-Time

| Class | Missed deadline | Examples |
|---|---|---|
| **Hard** | System fails / unsafe | Pacemaker, brake control, missile guidance |
| **Firm** | Result is useless if late, but system survives | Live video frame, control loop on a robot |
| **Soft** | Quality of service degrades | UI, audio (a glitch is bad but not fatal) |

The engineering effort scales: hard real-time uses certified RTOSes, formal WCET analysis, no dynamic allocation, no exceptions. Soft real-time tolerates regular OSes and most C++ features as long as p99 is good.

# 2. Sources of Non-Determinism

A list of things that can blow your worst-case latency:

- **Dynamic memory allocation** — `malloc` may take ms when the heap is fragmented or when it triggers `mmap`/`munmap`. Use pools or pre-allocate.
- **Page faults** — first touch of a page hits disk or zeros from kernel. `mlockall`/`mlock` to pin.
- **Garbage in the cache** — a non-RT thread pollutes L1/L2; your hot loop misses. Pin threads to dedicated cores; consider [cache partitioning](https://www.kernel.org/doc/html/latest/x86/resctrl.html).
- **TLB misses** — large data sets thrash the TLB. Use huge pages (`MAP_HUGETLB`).
- **Branch mispredicts** — predictable in steady state, but cold-cache start of a deadline costs.
- **System calls** — variable cost; some can block indefinitely. Avoid in the hot path.
- **Interrupts** — can preempt your task. Move IRQs off RT cores via `/proc/irq/*/smp_affinity`.
- **Compiler choices** — exception tables, RTTI lookups, unbounded recursion, dynamic dispatch.
- **Filesystem and network** — fundamentally non-deterministic. Move out of the RT path.
- **Logging** — printing to stderr from inside the deadline. Use a lock-free queue + offline logger thread.

# 3. Memory Allocation in Real-Time Code

The core rule: **no heap allocation in the hot path**. Strategies:

- **Pre-allocate** all buffers at startup.
- **Object pools** — `std::vector` of pre-constructed objects, reused. See [Memory Management Strategies](memory_management.md).
- **Stack arenas** — `alloca`, fixed-size buffer + bump pointer.
- **Specifically avoid**: `std::string` ad-hoc creations, `std::vector::push_back` (may reallocate), `std::unordered_map::insert` (may rehash), `std::shared_ptr::shared_ptr` (allocates control block).

```cpp
struct Item { int value; };

// In init phase
const size_t kMaxItems = 4096;
std::array<Item, kMaxItems> pool;
std::vector<Item*> free_list;
free_list.reserve(kMaxItems);
for (auto& x : pool) free_list.push_back(&x);

// In hot path -- all O(1), no allocation
Item* it = free_list.back();
free_list.pop_back();
it->value = 42;
process(it);
free_list.push_back(it);
```

# 4. Scheduling and Priority

POSIX real-time scheduling:

- **`SCHED_FIFO`** — first-in-first-out at a given priority. Preempts lower-priority threads. Doesn't time-slice with same-priority peers.
- **`SCHED_RR`** — round-robin within a priority level.
- **`SCHED_OTHER`** — the regular CFS scheduler. Not RT.

```cpp
struct sched_param sp{};
sp.sched_priority = 50;
::pthread_setschedparam(::pthread_self(), SCHED_FIFO, &sp);
```

You need `CAP_SYS_NICE` (or root) to set RT priorities.

CPU pinning isolates a core from the OS scheduler:
```cpp
cpu_set_t set; CPU_ZERO(&set); CPU_SET(3, &set);
::pthread_setaffinity_np(::pthread_self(), sizeof set, &set);
```

Pair with kernel boot params `isolcpus=3 nohz_full=3 rcu_nocbs=3` to remove core 3 from the scheduler entirely; only your RT thread runs there.

# 5. Priority Inversion

Classic disaster: a high-priority task waits on a lock held by a low-priority task that's preempted by a medium-priority task. The high-priority task is effectively blocked by the medium-priority one. This took down Mars Pathfinder in 1997.

Mitigations (mutex attributes):
- **Priority inheritance.** While a high-priority task waits on a mutex held by a low-priority one, the holder temporarily inherits the high priority. `pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT)`.
- **Priority ceiling.** The mutex itself has a priority; any holder runs at that priority. `PTHREAD_PRIO_PROTECT`.

`std::mutex` doesn't expose these. For real-time code, use `pthread_mutex_t` directly.

# 6. Exceptions, RTTI, Virtual Dispatch

| Feature | Hard RT | Soft RT |
|---|---|---|
| Exceptions (throwing) | Banned | OK on rare/error paths |
| Exceptions (declaring) | OK if no throw | OK |
| RTTI (`typeid`, `dynamic_cast`) | Often banned | OK |
| Virtual functions | OK (vtable lookup is bounded) | OK |
| Templates | OK | OK |
| Standard library | Mostly banned (allocations) | Selective |

`-fno-exceptions -fno-rtti` is common in hard RT. Standard containers go away; you replace them with allocation-free alternatives (etl, [Embedded Template Library](https://www.etlcpp.com/)).

# 7. Lock-Free vs Locks

For RT, lock-free or wait-free structures shine because a preempted thread can't block others. But lock-free isn't free:

- It still uses atomics, which are predictable in worst case.
- A locked mutex with priority inheritance can be acceptable in many soft-RT contexts.
- A wait-free SPSC ring buffer is simpler and faster than most lock-free MPMC structures — pick the simplest sufficient model.

See [Lock-Free Data Structures](../lock_free_data_structures.md).

# 8. Worst-Case Execution Time (WCET)

Quantifying WCET:

- **Static analysis tools** (aiT, OTAWA) compute upper bounds from object code + cache model.
- **Measurement-based** runs the code many times; takes the worst observed time. Pessimistic but easy.
- **Hybrid** combines both.

Practical rule: measure under the worst conditions you can simulate (cold cache, TLB miss, interrupts hitting), and add margin.

Tools for measurement:
- `perf stat` per-iteration with `-e instructions,cycles`.
- `clock_gettime(CLOCK_MONOTONIC_RAW)` in-loop.
- Cycle-accurate counters (`__rdtsc` on x86, `cntvct_el0` on ARM).
- Latency histogram libraries: [HdrHistogram](https://github.com/HdrHistogram/HdrHistogram_c), [hdr_histogram_cpp](https://github.com/HdrHistogram/HdrHistogram_cpp).

# 9. Linux for Real-Time: PREEMPT_RT

Vanilla Linux is not RT. Even with SCHED_FIFO and pinning, kernel critical sections can preempt your thread for unbounded time.

The PREEMPT_RT patch set (now mainlined for x86/ARM as of 6.12) makes most of the kernel preemptible:
- Spinlocks become preemptible mutexes with PI.
- IRQ handlers run as kernel threads (so they're scheduled).
- Latency drops from milliseconds (worst case) to tens of microseconds.

Tools to verify RT behavior:
- **`cyclictest`** — measures wakeup latency under load.
- **`hwlatdetect`** — finds System Management Interrupts.
- **rt-tests suite** — full set of latency tests.

For sub-microsecond hard RT, use a true RTOS (Zephyr, FreeRTOS, VxWorks) or a co-kernel (Xenomai, RTAI).

# 10. Patterns and Anti-Patterns

**Pattern: Pre-allocate at startup, never allocate in steady state.**

**Pattern: Bounded everything.** Bounded queues, bounded loops, bounded recursion. Anything unbounded is a latency time bomb.

**Pattern: Move slow work off the RT thread.** A lock-free queue from RT thread → logger / persistence / network thread keeps the deadline path clean.

**Pattern: Pre-warm caches.** Touch all the data the RT path will use during init.

**Anti-pattern: `std::shared_ptr` in hot path.** The atomic ref count is OK, but construction allocates and destruction frees. Use raw pointers or `unique_ptr`s passed by reference.

**Anti-pattern: `std::function` in hot path.** Allocates on construction (when storing big captures). Use templates or function pointers.

**Anti-pattern: logging from inside the deadline.** Even one `fprintf` can serialize through a global lock and stall.

**Anti-pattern: assuming wall-clock time is monotonic.** It isn't (NTP, leap seconds). Use `CLOCK_MONOTONIC` for elapsed time, `CLOCK_REALTIME` only for timestamps shown to humans.

**Anti-pattern: trusting "average is fine."** RT is about p99.999. Test with the worst-case load the system will see.

# References

- [Memory Management Strategies](memory_management.md)
- [Lock-Free Data Structures](../lock_free_data_structures.md)
- [Embedded Systems Design](embedded_systems.md)
- *Real-Time Systems Design and Analysis*, Laplante & Ovaska.
- *Hard Real-Time Computing Systems*, Buttazzo.
- [PREEMPT_RT wiki](https://wiki.linuxfoundation.org/realtime/start)
- [cyclictest](https://wiki.linuxfoundation.org/realtime/documentation/howto/tools/cyclictest)
- *The Art of Designing Embedded Systems*, Jack Ganssle.
