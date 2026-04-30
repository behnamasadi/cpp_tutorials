# Lock-Free Data Structures

This doc builds directly on the atomics and memory-ordering material in [multithreading.md §6](multithreading.md#6-stdatomic). If those concepts aren't comfortable, start there — none of what follows will make sense without them.

> **Realistic warning.** Production lock-free queues take experts months to get right and years to fully verify. The code in this doc is correct for the cases shown but you should not deploy hand-rolled lock-free containers without benchmarking, stress-testing, and reviewing memory-order proofs. Real codebases use libraries — see §10.

- [1. Lock-Free vs Wait-Free vs Blocking](#1-lock-free-vs-wait-free-vs-blocking)
- [2. Producer/Consumer Taxonomy](#2-producerconsumer-taxonomy)
- [3. Cache Lines and False Sharing](#3-cache-lines-and-false-sharing)
- [4. Single-Producer, Single-Consumer (SPSC) Ring Buffer](#4-single-producer-single-consumer-spsc-ring-buffer)
  - [4.1. Why it works](#41-why-it-works)
  - [4.2. Variants and gotchas](#42-variants-and-gotchas)
- [5. Single-Producer, Single-Consumer (SPSC) Unbounded Queue](#5-single-producer-single-consumer-spsc-unbounded-queue)
- [6. Multiple-Producer, Single-Consumer (MPSC) Queue — Vyukov's Intrusive](#6-multiple-producer-single-consumer-mpsc-queue--vyukovs-intrusive)
- [7. Single-Producer, Multiple-Consumer (SPMC) — Chase–Lev Work-Stealing Deque](#7-single-producer-multiple-consumer-spmc--chaselev-work-stealing-deque)
- [8. Multiple-Producer, Multiple-Consumer (MPMC) — Vyukov's Bounded Queue](#8-multiple-producer-multiple-consumer-mpmc--vyukovs-bounded-queue)
- [8b. Note on Unbounded Multiple-Producer, Multiple-Consumer (MPMC) Queues](#8b-note-on-unbounded-multiple-producer-multiple-consumer-mpmc-queues)
- [9. The Reclamation Problem](#9-the-reclamation-problem)
  - [9.1. ABA refresher](#91-aba-refresher)
  - [9.2. Hazard pointers (with Treiber stack)](#92-hazard-pointers)
  - [9.3. RCU and Epoch-Based Reclamation](#93-rcu-and-epoch-based-reclamation)
  - [9.4. Atomic shared_ptr](#94-atomic-shared_ptr)
- [10. Use a Library](#10-use-a-library)
- [11. When Not to Use Lock-Free](#11-when-not-to-use-lock-free)
- [12. References](#12-references)

---

# 1. Lock-Free vs Wait-Free vs Blocking

These terms describe progress guarantees, not "never uses a lock":

| Class | Guarantee | Example |
|---|---|---|
| **Blocking** | A stalled thread can prevent all others from making progress. | Mutex-based queue — if the holder is descheduled, no one else can `push` or `pop`. |
| **Lock-free** | At least *one* thread is always making progress, system-wide. Individual threads may starve. | CAS-loop counter, Treiber stack, Michael–Scott queue. |
| **Wait-free** | *Every* thread completes in a bounded number of steps regardless of other threads. | Single-producer ring buffer (per-side, no contention). |

Lock-free is the practical sweet spot. Wait-free is desirable but rare and usually slower in the common case.

What lock-free buys you:
- **Resilience to descheduling** — preempting one thread doesn't freeze the system. Critical for real-time and low-latency code.
- **Forward progress under contention** — no priority inversion, no deadlock.
- **Often better tail latency** than mutex-based code, because no thread pays a sleep/wake cost.

What it costs you:
- **Much harder to reason about and test.** Wrong code may "work" for years before the right interleaving surfaces.
- **More expensive in the uncontended case** than a mutex (which is essentially free when uncontended).
- **Memory reclamation is hard** — you can't just `delete` a node another thread might still be reading. See §9.

# 2. Producer/Consumer Taxonomy

Most lock-free queues are specialized for a particular access pattern. Picking the right one matters — a more general queue (e.g. MPMC) is always slower than a specialized one (e.g. SPSC) on the same workload.

| Acronym | Producers | Consumers | Difficulty | Typical use |
|---|---|---|---|---|
| **SPSC** | 1 | 1 | Easy | Audio thread → render thread; logger; feed one worker. |
| **SPMC** | 1 | many | Medium | Broadcast a stream to N workers; work-stealing read side. |
| **MPSC** | many | 1 | Medium | Many threads log to one writer; event aggregation. |
| **MPMC** | many | many | Hard | General work queue; thread pool. |

Rule of thumb: identify the *most-restricted* model that fits your workload. SPSC is dramatically simpler and faster than MPMC.

# 3. Cache Lines and False Sharing

A modern CPU moves memory in 64-byte chunks called **cache lines**. When two threads write to different variables that happen to share a line, the hardware coherence protocol shuffles that line back and forth between cores — even though the variables are logically independent. This is **false sharing**, and it can slow lock-free code down by an order of magnitude or worse.

The classic case in a queue:

```cpp
struct BadQueue {
    std::atomic<size_t> head;   // producer writes
    std::atomic<size_t> tail;   // consumer writes — same cache line as head!
    // ...
};
```

Producer and consumer are bouncing the same cache line between their cores on every push/pop.

The fix is to put each "hot" atomic on its own cache line:

```cpp
struct GoodQueue {
    alignas(std::hardware_destructive_interference_size) std::atomic<size_t> head;
    alignas(std::hardware_destructive_interference_size) std::atomic<size_t> tail;
    // ...
};
```

`std::hardware_destructive_interference_size` (C++17) is the implementation's recommended alignment to avoid false sharing — typically 64 or 128. If your toolchain doesn't define it, hard-code `64` (with a comment).

Pair with `std::hardware_constructive_interference_size` for the *opposite* problem: things you *want* on the same cache line because they're always accessed together.

> **Validation.** `perf c2c` (Linux) and Intel VTune both surface false-sharing hotspots. Don't trust intuition here.

# 4. Single-Producer, Single-Consumer (SPSC) Ring Buffer

The simplest correct lock-free queue. One thread pushes, another pops, indices into a fixed-size circular buffer, two atomic counters. Wait-free on each side.

```cpp
#include <atomic>
#include <array>
#include <new>          // hardware_destructive_interference_size

template <typename T, std::size_t Size>
class SpscRingBuffer {
    static_assert((Size & (Size - 1)) == 0, "Size must be a power of two");

#ifdef __cpp_lib_hardware_interference_size
    static constexpr std::size_t LineSize = std::hardware_destructive_interference_size;
#else
    static constexpr std::size_t LineSize = 64;
#endif

    alignas(LineSize) std::atomic<std::size_t> head_{0};   // written by producer only
    alignas(LineSize) std::atomic<std::size_t> tail_{0};   // written by consumer only
    alignas(LineSize) std::array<T, Size>      buffer_;

    static constexpr std::size_t mask_ = Size - 1;

public:
    bool push(T value) {
        const auto h    = head_.load(std::memory_order_relaxed);
        const auto next = (h + 1) & mask_;
        if (next == tail_.load(std::memory_order_acquire))
            return false;                                  // full
        buffer_[h] = std::move(value);
        head_.store(next, std::memory_order_release);     // publishes the write to buffer_[h]
        return true;
    }

    bool pop(T& out) {
        const auto t = tail_.load(std::memory_order_relaxed);
        if (t == head_.load(std::memory_order_acquire))
            return false;                                  // empty
        out = std::move(buffer_[t]);
        tail_.store((t + 1) & mask_, std::memory_order_release);
        return true;
    }
};
```

## 4.1. Why it works

- **Each index has exactly one writer.** `head_` is only ever stored to by the producer; `tail_` only by the consumer. There's never a write/write race on either, so no CAS is needed.
- **Acquire/release pairing publishes the data.** The producer writes `buffer_[h]` *then* `head_.store(release)`. The consumer's `head_.load(acquire)` synchronizes with that release: if the consumer sees the new `head_`, it's guaranteed to also see the write to `buffer_[h]`.
- **The relaxed self-loads are safe.** A side reading its own counter never sees a stale value of *its own* writes — that's the program-order rule.
- **Power-of-two size + bitmask** is faster than `% Size` and dodges the integer-division pipeline stall. Also avoids the "is the buffer full" ambiguity (we leave one slot empty so head==tail uniquely means "empty").
- **Per-line alignment** kills the false-sharing bounce on `head_`/`tail_` (§3).

## 4.2. Variants and gotchas

- **Wasting one slot.** The "leave one empty" trick means a `Size`-sized buffer holds at most `Size - 1` items. To use the full capacity, track full/empty with a separate flag or a "lap counter" in the high bits of head/tail.
- **Caching the other side's index.** Hot SPSC implementations cache `tail_` locally on the producer side and only refresh it when the cached value indicates "full" — saves an atomic load on the fast path.
- **`T` must be cheap to move.** This buffer stores by value; if `T` is heavy, store `unique_ptr<T>` instead.
- **Single producer, single consumer means *exactly one of each*.** Calling `push()` from two threads concurrently is undefined behavior here. Even with no contention, the algorithm relies on each side having one writer.

The "cache the opposite index" optimization is significant under load and almost always worth it. See [Folly's `ProducerConsumerQueue`](https://github.com/facebook/folly/blob/main/folly/ProducerConsumerQueue.h) for a battle-tested version of this pattern.

# 5. Single-Producer, Single-Consumer (SPSC) Unbounded Queue

The ring buffer's main drawback is its fixed capacity. If you need an unbounded SPSC queue, you switch to a linked list — the producer appends nodes, the consumer pops from the head:

```cpp
template <typename T>
class SpscUnboundedQueue {
    struct Node {
        std::atomic<Node*> next{nullptr};
        T                  value;
    };

    alignas(64) std::atomic<Node*> head_;     // producer side
    alignas(64) Node*              tail_;     // consumer side (no atomic — single consumer)

public:
    SpscUnboundedQueue() {
        Node* dummy = new Node{};            // sentinel: avoids head==tail edge cases
        head_.store(dummy, std::memory_order_relaxed);
        tail_ = dummy;
    }

    void push(T value) {
        Node* n = new Node{};
        n->value = std::move(value);
        Node* prev = head_.exchange(n, std::memory_order_acq_rel);
        prev->next.store(n, std::memory_order_release);
    }

    bool pop(T& out) {
        Node* next = tail_->next.load(std::memory_order_acquire);
        if (!next) return false;             // empty
        out   = std::move(next->value);
        Node* old_tail = tail_;
        tail_ = next;
        delete old_tail;                     // safe: only the consumer touches old nodes
        return true;
    }
};
```

The single-consumer property makes deletion trivial: only one thread ever touches "old" nodes, so we can `delete` immediately. Multi-consumer queues lose this property and need the reclamation strategies in §9.

This implementation:
- Allocates per-push (typically a per-thread node pool fixes that).
- Is unbounded — runaway producers will OOM you.
- Has no overflow handling — that's by design for an *unbounded* queue.

# 6. Multiple-Producer, Single-Consumer (MPSC) Queue — Vyukov's Intrusive

Multiple producers, single consumer. The canonical algorithm is **Dmitry Vyukov's intrusive MPSC queue** — used in actor systems and event loops where many threads send messages to one processor.

```cpp
struct Node {
    std::atomic<Node*> next{nullptr};
    // ... payload ...
};

class MpscQueue {
    alignas(64) std::atomic<Node*> head_;   // producer side
    alignas(64) Node*              tail_;   // consumer side
    Node                           stub_;   // sentinel, never deleted

public:
    MpscQueue() : head_{&stub_}, tail_{&stub_} {}

    void push(Node* n) {
        n->next.store(nullptr, std::memory_order_relaxed);
        Node* prev = head_.exchange(n, std::memory_order_acq_rel);
        prev->next.store(n, std::memory_order_release);
    }

    Node* pop() {                                 // single-consumer only
        Node* tail = tail_;
        Node* next = tail->next.load(std::memory_order_acquire);
        if (tail == &stub_) {
            if (!next) return nullptr;            // empty
            tail_ = next;
            tail  = next;
            next  = next->next.load(std::memory_order_acquire);
        }
        if (next) { tail_ = next; return tail; }

        Node* head = head_.load(std::memory_order_acquire);
        if (tail != head) return nullptr;         // inconsistent — producer mid-push

        push(&stub_);
        next = tail->next.load(std::memory_order_acquire);
        if (next) { tail_ = next; return tail; }
        return nullptr;
    }
};
```

Key properties:

- **Producer side is wait-free** — a single `exchange` + a single `store`, no loops.
- **Consumer side is lock-free, not wait-free** — under heavy contention, the consumer can observe a transient inconsistent state (a producer has done the `exchange` but not the `next.store` yet) and must retry.
- **Intrusive** — the queue doesn't allocate; the caller embeds `Node` in their payload.
- **Stub node** trick keeps the algorithm's edge cases simple. The stub is never freed.

This is genuinely tricky. Read [Vyukov's original page](https://www.1024cores.net/home/lock-free-algorithms/queues/intrusive-mpsc-node-based-queue) before deploying anything based on it.

# 7. Single-Producer, Multiple-Consumer (SPMC) — Chase–Lev Work-Stealing Deque

One producer, many consumers. The textbook algorithm is the **Chase–Lev work-stealing deque** — used in every modern work-stealing scheduler (Cilk, TBB, Go, Rust's Rayon, Java's `ForkJoinPool`).

The structure is asymmetric:
- The **owner** (producer) pushes and pops at the *bottom*. No CAS needed on its fast path.
- **Thieves** (consumers) steal at the *top*, contending with each other and occasionally the owner via CAS.

```cpp
#include <atomic>
#include <array>
#include <cstdint>
#include <new>

template <typename T, std::size_t Size>
class ChaseLevDeque {
    static_assert((Size & (Size - 1)) == 0, "Size must be a power of two");

#ifdef __cpp_lib_hardware_interference_size
    static constexpr std::size_t LineSize = std::hardware_destructive_interference_size;
#else
    static constexpr std::size_t LineSize = 64;
#endif

    alignas(LineSize) std::atomic<int64_t>      top_{0};       // thieves
    alignas(LineSize) std::atomic<int64_t>      bottom_{0};    // owner
    alignas(LineSize) std::array<std::atomic<T>, Size> buf_;

    static constexpr int64_t mask_ = Size - 1;

public:
    // Owner only. Wait-free.
    bool push(T v) {
        const int64_t b = bottom_.load(std::memory_order_relaxed);
        const int64_t t = top_.load(std::memory_order_acquire);
        if (b - t >= static_cast<int64_t>(Size)) return false;     // full
        buf_[b & mask_].store(std::move(v), std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_release);       // publish slot before bottom
        bottom_.store(b + 1, std::memory_order_relaxed);
        return true;
    }

    // Owner only. Lock-free (one CAS in the contended last-element case).
    bool pop(T& out) {
        int64_t b = bottom_.load(std::memory_order_relaxed) - 1;
        bottom_.store(b, std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_seq_cst);       // barrier vs steal
        int64_t t = top_.load(std::memory_order_relaxed);

        if (t > b) {                                               // empty
            bottom_.store(b + 1, std::memory_order_relaxed);
            return false;
        }
        out = buf_[b & mask_].load(std::memory_order_relaxed);
        if (t != b) return true;                                   // not last — uncontested

        // Last element: race the thieves for it.
        bool won = top_.compare_exchange_strong(
            t, t + 1, std::memory_order_seq_cst, std::memory_order_relaxed);
        bottom_.store(b + 1, std::memory_order_relaxed);
        return won;
    }

    // Any thief. Lock-free (one CAS).
    bool steal(T& out) {
        int64_t t = top_.load(std::memory_order_acquire);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        int64_t b = bottom_.load(std::memory_order_acquire);
        if (t >= b) return false;                                  // empty

        out = buf_[t & mask_].load(std::memory_order_relaxed);
        return top_.compare_exchange_strong(
            t, t + 1, std::memory_order_seq_cst, std::memory_order_relaxed);
    }
};
```

Why this works:

- **Owner's `push` is wait-free.** It's the only writer of `bottom_`, and the order "store slot → fence → store bottom" guarantees that any thief seeing the new `bottom_` also sees the slot data.
- **Owner's `pop` decrements `bottom_` first**, then fences, then reads `top_`. The fence prevents the read of `top_` from being reordered before the write to `bottom_` — that's what makes the race against thieves correct.
- **Thieves only update `top_`.** Multiple thieves race via CAS; at most one wins.
- **When the deque has exactly one element**, owner-pop and thief-steal both want it. The owner's CAS on `top_` resolves the race: either it wins (and the thieves see an empty deque) or it loses (and the value goes to whichever thief CASed first).

Limitations of the version above:
- **Bounded.** Real Chase–Lev grows the underlying array dynamically; that adds a "current array" atomic pointer and a retire-old-array policy. The bounded variant is enough for most thread-pool workloads.
- **`T` must fit in a `std::atomic` slot.** For larger types, store `T*` and manage lifetime separately (or use the unbounded version with hazard pointers on the array).

> Practical advice: if you don't need stealing semantics, a fan-out of N independent SPSC queues (one per consumer, dispatcher round-robins) is usually simpler and faster than a single SPMC queue.

# 8. Multiple-Producer, Multiple-Consumer (MPMC) — Vyukov's Bounded Queue

Many producers, many consumers. The well-known algorithm is **Vyukov's bounded MPMC queue** — the basis of moodycamel's `ConcurrentQueue`, rigtorp's `MPMCQueue`, and most other production implementations.

The trick: each cell has its own **sequence number** that tracks "what's the next operation expected on this cell?" Producers and consumers compete only via two CAS-able position counters; per-cell synchronization is done via the sequence numbers, which sidesteps ABA entirely.

```cpp
#include <atomic>
#include <array>
#include <cstddef>
#include <cstdint>
#include <new>

template <typename T, std::size_t Size>
class MpmcBoundedQueue {
    static_assert((Size & (Size - 1)) == 0, "Size must be a power of two");
    static_assert(Size >= 2, "Size must be at least 2");

#ifdef __cpp_lib_hardware_interference_size
    static constexpr std::size_t LineSize = std::hardware_destructive_interference_size;
#else
    static constexpr std::size_t LineSize = 64;
#endif

    struct Cell {
        std::atomic<std::size_t> seq;
        T                        data;
    };

    alignas(LineSize) std::array<Cell, Size>     buf_;
    alignas(LineSize) std::atomic<std::size_t>   enq_pos_{0};
    alignas(LineSize) std::atomic<std::size_t>   deq_pos_{0};

    static constexpr std::size_t mask_ = Size - 1;

public:
    MpmcBoundedQueue() {
        for (std::size_t i = 0; i < Size; ++i)
            buf_[i].seq.store(i, std::memory_order_relaxed);
    }

    bool enqueue(T value) {
        Cell* cell;
        std::size_t pos = enq_pos_.load(std::memory_order_relaxed);
        for (;;) {
            cell = &buf_[pos & mask_];
            std::size_t seq  = cell->seq.load(std::memory_order_acquire);
            std::intptr_t diff = (std::intptr_t)seq - (std::intptr_t)pos;
            if (diff == 0) {
                // Cell is ready for an enqueue at position `pos`. Claim it.
                if (enq_pos_.compare_exchange_weak(
                        pos, pos + 1, std::memory_order_relaxed))
                    break;
            } else if (diff < 0) {
                return false;                                   // full
            } else {
                // Another producer already claimed `pos`. Reload and retry.
                pos = enq_pos_.load(std::memory_order_relaxed);
            }
        }
        cell->data = std::move(value);
        cell->seq.store(pos + 1, std::memory_order_release);    // hand off to consumer
        return true;
    }

    bool dequeue(T& out) {
        Cell* cell;
        std::size_t pos = deq_pos_.load(std::memory_order_relaxed);
        for (;;) {
            cell = &buf_[pos & mask_];
            std::size_t seq  = cell->seq.load(std::memory_order_acquire);
            std::intptr_t diff = (std::intptr_t)seq - (std::intptr_t)(pos + 1);
            if (diff == 0) {
                // Cell holds an item produced for position `pos`. Claim it.
                if (deq_pos_.compare_exchange_weak(
                        pos, pos + 1, std::memory_order_relaxed))
                    break;
            } else if (diff < 0) {
                return false;                                   // empty
            } else {
                pos = deq_pos_.load(std::memory_order_relaxed);
            }
        }
        out = std::move(cell->data);
        cell->seq.store(pos + Size, std::memory_order_release); // ready for next lap
        return true;
    }
};
```

How the sequence numbers work:

- Cell `i` starts at `seq == i`. That means "ready to be enqueued at position `i`."
- A producer claims `pos` when `cell->seq == pos`. After writing the data, it stores `seq = pos + 1` — meaning "ready to be dequeued at position `pos`."
- A consumer claims `pos` when `cell->seq == pos + 1`. After reading, it stores `seq = pos + Size` — meaning "ready to be enqueued one full lap later."

The key insight: the *sequence number is what advances*, not the data pointer. There's no node to recycle, so no ABA window. Producers and consumers can intermix freely — no producer needs to know which consumer "owns" the cell next, only that the seq number says so.

Properties:
- **Both sides are lock-free**, not wait-free — the CAS on `enq_pos_`/`deq_pos_` can be lost to other producers/consumers.
- **Bounded.** `Size` must be a power of two. Choose it large enough that producers don't see "full" too often.
- **Per-cell false sharing matters.** For best performance, pad each `Cell` to a cache line:

```cpp
struct alignas(LineSize) Cell { ... };
```

Otherwise neighboring cells thrash the cache when multiple producers/consumers operate at adjacent positions.

This algorithm is fast and correct, but the proofs are dense. Don't modify it without reading [Vyukov's original write-up](https://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue) and the formal correctness arguments. For unbounded MPMC (Michael–Scott + reclamation), see §9.

# 8b. Note on Unbounded Multiple-Producer, Multiple-Consumer (MPMC) Queues

The classic unbounded MPMC algorithm is **Michael–Scott**, a linked-list queue with separate `head` and `tail` atomics and a "dummy node" trick. It's about 50 lines, but the safe-reclamation glue (you can't `delete` a node while another thread might still be traversing it) makes the *complete* implementation 200+ lines once you bolt on hazard pointers (§9.2) or epoch-based reclamation.

Unless you genuinely need unbounded growth, prefer Vyukov's bounded queue — it's faster, simpler, and avoids the reclamation problem entirely. If you do need unbounded, **use a library** (§10) rather than rolling your own.

# 9. The Reclamation Problem

The deepest difficulty in non-trivial lock-free data structures: when can you safely free a node?

Mutex-based code doesn't have this problem — only one thread is in the critical section, so when it removes a node it can free it immediately. Lock-free code has *N* threads simultaneously traversing the structure; some of them may still hold pointers to a node you want to free.

## 9.1. ABA refresher

Already covered in [multithreading.md §6.5](multithreading.md#65-compare_exchange-and-the-aba-problem) — CAS only checks the value, not whether the value has been recycled in between. Reclamation strategies prevent that recycling from being observable.

## 9.2. Hazard pointers

Each thread publishes "I am currently reading node X" by writing X to its own *hazard pointer slot* (a known atomic location). A thread that wants to free X first scans every other thread's hazard pointer slots; if any of them point at X, the free is deferred to a retire list and the scan retries later.

```
Thread A: reads node X, sets hazard[A] = X
Thread B: wants to delete X
          scans all hazards
          sees hazard[A] == X → puts X on retire list, doesn't delete
Thread A: finishes, sets hazard[A] = nullptr
Thread B: next scan, no hazard claims X → deletes X
```

Below is a minimal working implementation — small enough to read in one sitting, complete enough to use as the basis of a Treiber stack. Production hazard-pointer libraries are more involved (per-thread retire lists, batch reclamation, dynamic slot allocation) but the core idea is here.

```cpp
#include <atomic>
#include <array>
#include <thread>
#include <vector>

constexpr std::size_t MaxThreads     = 64;
constexpr std::size_t MaxRetiredSize = 2 * MaxThreads;       // scan threshold

class HazardManager {
    struct Slot {
        std::atomic<std::thread::id> owner{};
        std::atomic<void*>           ptr{nullptr};
    };

    std::array<Slot, MaxThreads> slots_;

    Slot& my_slot() {
        thread_local Slot* cached = nullptr;
        if (cached) return *cached;

        std::thread::id me = std::this_thread::get_id();
        std::thread::id none{};
        for (auto& s : slots_) {
            std::thread::id expected = none;
            if (s.owner.compare_exchange_strong(expected, me)) {
                cached = &s;
                return s;
            }
        }
        std::abort();   // exceeded MaxThreads — bump the constant
    }

public:
    // Mark `p` as in-use by this thread. Caller must re-validate `p` afterwards.
    void protect(void* p) {
        my_slot().ptr.store(p, std::memory_order_release);
    }

    void clear() {
        my_slot().ptr.store(nullptr, std::memory_order_release);
    }

    bool is_hazardous(void* p) const {
        for (auto& s : slots_)
            if (s.ptr.load(std::memory_order_acquire) == p)
                return true;
        return false;
    }

    // Per-thread retire list. When it gets large, scan and free what's safe.
    template <typename T>
    void retire(T* p) {
        thread_local std::vector<void*> retired;
        retired.push_back(p);
        if (retired.size() < MaxRetiredSize) return;

        std::vector<void*> still_hazardous;
        for (void* q : retired) {
            if (is_hazardous(q)) still_hazardous.push_back(q);
            else                 delete static_cast<T*>(q);
        }
        retired = std::move(still_hazardous);
    }
};
```

The classical use case: a **Treiber stack** — the simplest lock-free DS that needs reclamation.

```cpp
template <typename T>
class TreiberStack {
    struct Node {
        T     value;
        Node* next;
    };

    std::atomic<Node*> head_{nullptr};
    HazardManager      hp_;

public:
    void push(T value) {
        Node* n = new Node{std::move(value), nullptr};
        Node* h = head_.load(std::memory_order_relaxed);
        do {
            n->next = h;
        } while (!head_.compare_exchange_weak(
                     h, n, std::memory_order_release, std::memory_order_relaxed));
    }

    bool pop(T& out) {
        Node* h;
        for (;;) {
            h = head_.load(std::memory_order_acquire);
            if (!h) return false;

            hp_.protect(h);                          // publish hazard
            if (head_.load(std::memory_order_acquire) != h)
                continue;                            // h was popped between load & protect

            // Now h is safely protected: any reclaimer will see our hazard.
            if (head_.compare_exchange_weak(
                    h, h->next, std::memory_order_acquire, std::memory_order_relaxed))
                break;
        }
        out = std::move(h->value);
        hp_.clear();
        hp_.retire(h);                               // freed eventually, when no hazards remain
        return true;
    }
};
```

The dance to remember:
1. Load `head_` into `h`.
2. **Publish** `h` as a hazard (release store).
3. **Re-validate**: load `head_` again. If it changed, our hazard could be on a node already retired — restart.
4. Now CAS to swing `head_` to `h->next`. If it succeeds, we own `h`.
5. Clear our hazard, then retire `h`. The retire deferred-free will skip any node still listed as someone's hazard.

Step 3 — the re-validation after publishing the hazard — is what makes the protocol correct. Without it, a reclaimer that scanned hazards *before* you published yours would happily delete the node you're about to use.

Strengths: bounded memory overhead per thread; works for any lock-free DS where you can identify "the pointer being followed."

Weaknesses: every read pays a hazard-pointer publish + re-validation; tricky to get the memory ordering right; you must enumerate every pointer that might be followed (multi-hop traversals need multiple hazard slots per thread).

C++26 is on track to ship `std::hazard_pointer` and friends. Until then: Folly's `folly/synchronization/Hazptr.h` is the production reference.

## 9.3. RCU and Epoch-Based Reclamation

**Read-Copy-Update**: readers traverse the structure with no per-read overhead; writers don't free nodes immediately, instead waiting for a "grace period" — a point at which all readers that might have been reading the old node have finished.

A simplified user-space variant — **epoch-based reclamation (EBR)** — captures the same idea without the kernel infrastructure. Each thread enters a "read-side critical section" by recording the current global epoch; reclaimers retire nodes tagged with the epoch they were unlinked in; a node is safe to free once the global epoch has advanced *past every active reader's recorded epoch*.

```cpp
#include <atomic>
#include <array>
#include <vector>
#include <thread>

constexpr std::size_t MaxReaders = 64;

class EpochManager {
    std::atomic<uint64_t>                    global_epoch_{1};
    std::array<std::atomic<uint64_t>, MaxReaders> active_;       // 0 = not in CS

    std::size_t my_index() {
        thread_local std::size_t idx = [] {
            static std::atomic<std::size_t> next{0};
            return next.fetch_add(1);
        }();
        return idx;
    }

public:
    EpochManager() { for (auto& a : active_) a.store(0, std::memory_order_relaxed); }

    // Mark this thread as "reading at the current epoch."
    uint64_t enter() {
        uint64_t e = global_epoch_.load(std::memory_order_acquire);
        active_[my_index()].store(e, std::memory_order_release);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        return e;
    }

    void exit() {
        active_[my_index()].store(0, std::memory_order_release);
    }

    // Retire a node. Free it when no reader is in an epoch ≤ retirement epoch.
    template <typename T>
    void retire(T* p) {
        struct Retired { void* ptr; uint64_t epoch; void (*deleter)(void*); };
        thread_local std::vector<Retired> bin;

        uint64_t e = global_epoch_.fetch_add(1, std::memory_order_acq_rel);
        bin.push_back({p, e, [](void* x) { delete static_cast<T*>(x); }});

        // Scan: free everything whose retirement epoch is older than every active reader.
        uint64_t safe = e;
        for (auto& a : active_) {
            uint64_t r = a.load(std::memory_order_acquire);
            if (r != 0 && r < safe) safe = r;
        }
        std::vector<Retired> still_unsafe;
        for (auto& r : bin) {
            if (r.epoch < safe) r.deleter(r.ptr);
            else                still_unsafe.push_back(r);
        }
        bin = std::move(still_unsafe);
    }
};

// Reader-side RAII guard:
class EpochGuard {
    EpochManager& m_;
public:
    explicit EpochGuard(EpochManager& m) : m_(m) { m_.enter(); }
    ~EpochGuard() { m_.exit(); }
    EpochGuard(const EpochGuard&)            = delete;
    EpochGuard& operator=(const EpochGuard&) = delete;
};
```

Use it like this:

```cpp
EpochManager ebr;

// Reader
{
    EpochGuard guard(ebr);
    Node* n = head.load();             // safe to follow `n` for the duration of `guard`
    use(n);
}                                      // exit() — n may now be reclaimed at some point

// Writer
Node* old = head.exchange(new_node);
ebr.retire(old);                       // freed when all readers have left their epoch
```

Strengths: reads pay one atomic store + one load — cheaper than hazard pointers' per-pointer publish + re-validation. Scales beautifully when reads dominate.

Weaknesses: a single stuck reader pins the global epoch, deferring *all* reclamation until it exits. Memory pressure can grow unboundedly if your read sections are long.

The full Linux RCU kernel implementation handles this with quiescent-state tracking, multiple grace-period algorithms, and tiered reclamation. User-space RCU (`liburcu`) ports much of it. For C++ projects, `crossbeam` (Rust) and `cds::urcu` (C++) are good references.

## 9.4. Atomic shared_ptr

`std::atomic<std::shared_ptr<T>>` (C++20) sidesteps reclamation entirely: the shared_ptr's reference count holds nodes alive while readers hold them.

```cpp
std::atomic<std::shared_ptr<Node>> head;

// reader
auto node = head.load();        // bumps refcount; node stays alive while we use it

// writer
auto fresh = std::make_shared<Node>(...);
head.store(fresh);              // old head released; refcount drops when last reader exits
```

Strengths: simple; uses the type system to make reclamation automatic.

Weaknesses: every load is an atomic refcount bump (typically two atomic ops on x86, more on weak-memory machines); roughly an order of magnitude slower than hazard pointers in read-heavy workloads.

Worth it for low-frequency configuration-style data; not for hot paths.

# 10. Use a Library

For anything beyond an SPSC ring buffer, reach for a battle-tested implementation:

| Library | License | What it gives you |
|---|---|---|
| **moodycamel::ConcurrentQueue** | Simplified BSD | Header-only MPMC queue, very fast, well-tested. |
| **moodycamel::ReaderWriterQueue** | Simplified BSD | Header-only SPSC queue. |
| **Folly** (`folly::MPMCQueue`, `folly::ProducerConsumerQueue`) | Apache 2.0 | Industrial-grade, but a heavy dependency. |
| **Boost.Lockfree** | Boost | `boost::lockfree::queue`, `spsc_queue`, `stack`. |
| **rigtorp::SPSCQueue / MPMCQueue** | MIT | Small header-only implementations of well-known algorithms. |
| **TBB `concurrent_queue`** | Apache 2.0 | Part of Intel TBB / oneTBB. |

A 30-line hand-rolled queue almost always loses to these in throughput, latency, and (especially) correctness.

# 11. When Not to Use Lock-Free

Lock-free is not always faster than a well-designed mutex queue. Consider mutex-based first if:

- **Contention is rare.** Uncontended mutexes are extremely cheap on Linux (futex). Lock-free pays atomic-op overhead on every operation.
- **The critical section is large.** Lock-free wants tiny operations; large ones force long retry loops.
- **You want bounded latency above all.** A mutex with priority inheritance can give you that more directly than CAS-loop fairness arguments.
- **You don't have time to verify the code.** "Pretty sure it works" is not enough for lock-free; bugs surface only under specific timing.

> Profile first. If a mutex isn't the bottleneck, a lock-free queue won't make your system faster — it'll just make it harder to debug.

# 12. References

- **Books**
  - *C++ Concurrency in Action* (Anthony Williams) — chapter 7 covers lock-free queues with full proofs.
  - *The Art of Multiprocessor Programming* (Herlihy & Shavit) — the academic foundation.
- **Papers**
  - Michael & Scott, *Simple, Fast, and Practical Non-Blocking and Blocking Concurrent Queue Algorithms* (1996).
  - Maged Michael, *Hazard Pointers: Safe Memory Reclamation for Lock-Free Objects* (2004).
- **Sites**
  - [1024cores (Dmitry Vyukov)](https://www.1024cores.net/) — definitive practitioner reference for queue algorithms.
  - [Preshing on Programming](https://preshing.com/) — clear posts on memory ordering and lock-free.
- **Code**
  - [Folly synchronization](https://github.com/facebook/folly/tree/main/folly/synchronization)
  - [moodycamel queues](https://github.com/cameron314/concurrentqueue)
  - [rigtorp SPSC/MPMC](https://github.com/rigtorp)
- **Related docs**
  - [multithreading.md](multithreading.md) — atomics, memory ordering, ABA, threads.
  - [smart_pointers.md §7](smart_pointers.md#7-atomic-smart-pointers-c20) — atomic shared_ptr.
