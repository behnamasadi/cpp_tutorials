# Pipeline and Dataflow Architectures

A pipeline expresses computation as a directed graph of stages connected by queues. Each stage transforms its input and forwards the result. Done well, pipelines give you natural concurrency, backpressure, and composability.

- [1. When to Use a Pipeline](#1-when-to-use-a-pipeline)
- [2. Linear Pipelines](#2-linear-pipelines)
- [3. Branching and Joining](#3-branching-and-joining)
- [4. Backpressure](#4-backpressure)
- [5. Batching and Vectorization](#5-batching-and-vectorization)
- [6. Backpressure-Aware Async — `std::execution` (Senders/Receivers)](#6-backpressure-aware-async--stdexecution-sendersreceivers)
- [7. Throughput Math](#7-throughput-math)
- [8. Production Libraries](#8-production-libraries)
- [9. Anti-patterns](#9-anti-patterns)

---

# 1. When to Use a Pipeline

A pipeline is the right structure when:

- The work decomposes into **stages with different costs** — e.g., I/O read, decode, compute, encode, I/O write. Running stages on different cores overlaps their costs.
- Items are **independent** (no item-to-item dependencies that cross stages).
- You need **backpressure** — slow downstream stages must throttle the upstream.
- You want **composability** — adding a new transformation = adding a stage.

Bad fits: very small workloads (queue overhead dominates), workloads with strong inter-item dependencies, or workloads where the slowest stage is a single thread you can't parallelize.

# 2. Linear Pipelines

Three stages connected by bounded queues:

```cpp
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <thread>
#include <iostream>

class BoundedQueue {
  std::queue<int> q;
  size_t cap;
  std::mutex m;
  std::condition_variable not_full, not_empty;
  bool closed = false;
public:
  BoundedQueue(size_t c) : cap(c) {}

  void push(int v) {
    std::unique_lock<std::mutex> l(m);
    not_full.wait(l, [&]{ return q.size() < cap; });
    q.push(v);
    not_empty.notify_one();
  }

  std::optional<int> pop() {  // returns nullopt at end-of-stream
    std::unique_lock<std::mutex> l(m);
    not_empty.wait(l, [&]{ return !q.empty() || closed; });
    if (q.empty()) return std::nullopt;
    int v = q.front(); q.pop();
    not_full.notify_one();
    return v;
  }

  void close() {
    { std::lock_guard<std::mutex> l(m); closed = true; }
    not_empty.notify_all();
  }
};

int main() {
  BoundedQueue q1(4);  // raw frames
  BoundedQueue q2(4);  // decoded frames

  std::thread reader([&]{
    for (int i = 0; i < 10; ++i) q1.push(i);
    q1.close();
  });
  std::thread decoder([&]{
    while (auto raw = q1.pop()) q2.push(*raw * 2);  // "decode" = double
    q2.close();
  });
  std::thread writer([&]{
    while (auto frame = q2.pop()) std::cout << *frame << '\n';
  });

  reader.join(); decoder.join(); writer.join();
}
```

Each stage runs on its own thread; throughput is bounded by the slowest stage. The queues smooth out short bursts.

# 3. Branching and Joining

Real pipelines are graphs, not lines. A common shape:

```
           ┌─[crop]──┐
[load]────┤          ├──[combine]──[save]
           └─[blur]──┘
```

Implementation hints:
- Each branch has its own queue.
- A join stage reads from N queues. The simplest scheme: round-robin pop, or wait for one item from each.
- For ordering preservation across a fan-out/fan-in, attach a sequence number to every item and the join stage reorders.

# 4. Backpressure

If a downstream stage is slower than the upstream, items pile up — unbounded growth means OOM. **Bounded queues** with blocking writes are the simplest, most reliable backpressure mechanism.

Strategies, in order of typical preference:

1. **Block** the producer until space is free. Lossless; simple. Works when the producer can wait.
2. **Drop newest** — discard incoming items. Useful for live-only feeds (real-time video, telemetry).
3. **Drop oldest** — discard the head. For "latest is best" (sensor readings).
4. **Conflate / coalesce** — merge equivalent items (e.g., keep only the latest update per key).
5. **Spill to disk** — extend the queue with a file-backed buffer when memory fills (logging pipelines).

Reactive frameworks (RxCpp, ReactiveX) make these strategies named operators. In hand-rolled code, decide per queue and document the choice.

# 5. Batching and Vectorization

Per-item synchronization (one push, one pop) has fixed overhead — typically tens to hundreds of nanoseconds for a mutex-based queue. For small items, this dominates. Solution: process batches.

```cpp
std::vector<int> batch;
while (auto x = upstream.pop()) {
  batch.push_back(*x);
  if (batch.size() >= 1024) {
    downstream.push(batch);
    batch.clear();
  }
}
if (!batch.empty()) downstream.push(batch);
```

A 1024-item batch amortizes the synchronization to ~1/1024 per item. Trade-off: latency increases by the time it takes to fill a batch. Use a timeout for low-rate streams (`flush after 10 ms`).

Batching also opens the door to SIMD / vectorized stage implementations — see [Branch Prediction and SIMD](branch_prediction_simd.md).

# 6. Backpressure-Aware Async — `std::execution` (Senders/Receivers)

C++26's `<execution>` (P2300, "Senders/Receivers") provides composable async pipelines without explicit threads:

```cpp
namespace ex = std::execution;

auto pipeline =
    ex::just(input_path)
  | ex::then(load_image)
  | ex::let_value([](auto img) {
      return ex::when_all(ex::then(ex::just(img), crop),
                          ex::then(ex::just(img), blur));
    })
  | ex::then([](auto cb) { return combine(std::get<0>(cb), std::get<1>(cb)); })
  | ex::then(save);

stdexec::sync_wait(pipeline);
```

This is the future direction. Today, libunifex and stdexec implement it on top of C++17/20.

# 7. Throughput Math

Two simple numbers usually predict pipeline performance:

- **Throughput** = `1 / max(stage_cost_i)` — the slowest stage limits everything.
- **Latency** = `Σ stage_cost_i + Σ queue_wait_i` — sum across stages (assuming sequential dependency).

Implications:
- Speeding up a non-bottleneck stage doesn't improve throughput, only latency.
- A stage that takes twice as long needs **two parallel workers** (a stage *pool*) to keep up.
- Queue depth ≈ batch arrival burstiness × downstream cost. Too small → producers block; too large → wasted memory + worse cache behavior.

Measure each stage independently with a benchmark before optimizing.

# 8. Production Libraries

| Library | Style | Notes |
|---|---|---|
| **Intel TBB** | Flow Graph nodes | Mature; work-stealing; closed-source intrinsics for SIMD-friendly nodes |
| **HPX** | Senders/receivers | Distributed scaling |
| **NVIDIA stdexec / libunifex** | C++26 senders | Reference implementations of P2300 |
| **RxCpp** | ReactiveX operators | Push-based; mature backpressure operators |
| **GStreamer** | Plugin-based media pipelines | Domain-specific (audio/video) |
| **Apache Arrow Flight / Substrait** | Columnar dataflow | Big-data analytics |

For embedded systems or tight kernels, hand-roll. For application-level dataflow, reach for a library.

# 9. Anti-patterns

**Unbounded queues.** Will eventually consume all memory under sustained overload. Always bound them.

**One thread per stage even for tiny stages.** Cache-line ping-pong between threads can erase the parallelism gain. Combine cheap stages.

**Per-item allocation.** If each stage allocates a new object for the next, allocator pressure dominates. Pool, reuse, or pass by value (move).

**Hidden serial stage.** A "transform" stage that takes a global mutex turns a pipeline into a single thread plus overhead. Profile.

**Reordering bugs in fan-out/fan-in.** If downstream cares about input order, you must explicitly maintain it (sequence numbers + reorder buffer).

**Backpressure ignored.** Producer pushes "fire and forget" without checking. The first time the consumer is slower, the queue grows without bound.

# References

- [Concurrency Patterns](concurrency_patterns.md)
- [Branch Prediction and SIMD](branch_prediction_simd.md)
- [Lock-Free Data Structures](../lock_free_data_structures.md)
- [Intel TBB Flow Graph](https://oneapi-src.github.io/oneTBB/main/tbb_userguide/Flow_Graph.html)
- [P2300 — `std::execution`](https://wg21.link/p2300)
- *Streaming Systems*, Akidau/Chernyak/Lax — the canonical book on streaming dataflow.
