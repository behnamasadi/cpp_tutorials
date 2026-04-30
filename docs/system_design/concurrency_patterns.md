# Concurrency Patterns

Reusable structures for concurrent programs. They sit a level above raw threads and atomics — you compose them out of [`std::thread`](../multithreading.md), `std::mutex`, condition variables, and futures.

- [1. Thread Pool](#1-thread-pool)
- [2. Producer–Consumer](#2-producerconsumer)
- [3. Active Object](#3-active-object)
- [4. Reactor](#4-reactor)
- [5. Proactor](#5-proactor)
- [6. Pipeline / Stages](#6-pipeline--stages)
- [7. Fork–Join and Work Stealing](#7-forkjoin-and-work-stealing)
- [8. Monitor Object](#8-monitor-object)
- [9. Read–Copy–Update (RCU)](#9-readcopyupdate-rcu)
- [10. Choosing Between Them](#10-choosing-between-them)

---

# 1. Thread Pool

A fixed set of worker threads consuming tasks from a queue. The most common concurrency primitive in real systems.

```cpp
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
  std::vector<std::thread> workers;
  std::queue<std::function<void()>> tasks;
  std::mutex m;
  std::condition_variable cv;
  int remaining;  // demo: stop after this many tasks have been pulled
public:
  ThreadPool(int n, int total_tasks) : remaining(total_tasks) {
    for (int i = 0; i < n; ++i)
      workers.emplace_back([this] { run(); });
  }
  ~ThreadPool() { for (auto& w : workers) w.join(); }

  void submit(std::function<void()> f) {
    { std::scoped_lock l(m); tasks.push(f); }
    cv.notify_one();
  }
private:
  void run() {
    while (true) {
      std::function<void()> job;
      {
        std::unique_lock l(m);
        cv.wait(l, [&] { return !tasks.empty() || remaining == 0; });
        if (remaining == 0) return;
        job = tasks.front();
        tasks.pop();
        --remaining;
      }
      job();
    }
  }
};

int main() {
  ThreadPool pool(3, 5);
  for (int i = 0; i < 5; ++i)
    pool.submit([i] { std::cout << "task " << i << '\n'; });
}
```

Real production pools add: bounded queues with backpressure, per-thread local queues, work-stealing (see §7), priorities, work-class isolation (don't put I/O and CPU in the same pool).

# 2. Producer–Consumer

Producers add items, consumers remove them, a queue mediates. The bounded version implements **backpressure**: when the queue is full, producers block.

```cpp
#include <condition_variable>
#include <mutex>
#include <queue>

class BoundedQueue {
  std::queue<int> q;
  std::size_t cap;
  std::mutex m;
  std::condition_variable not_full, not_empty;
public:
  BoundedQueue(std::size_t cap) : cap(cap) {}

  void push(int v) {
    std::unique_lock l(m);
    not_full.wait(l, [&] { return q.size() < cap; });
    q.push(v);
    not_empty.notify_one();
  }

  int pop() {
    std::unique_lock l(m);
    not_empty.wait(l, [&] { return !q.empty(); });
    int v = q.front();
    q.pop();
    not_full.notify_one();
    return v;
  }
};
```

For lock-free variants, see [Lock-Free Data Structures](../lock_free_data_structures.md). For high-throughput SPSC use a ring buffer.

## 2.1 Backpressure vs. drop: choosing the policy

If the producer is faster than the consumer, you must decide what happens to the excess. There is no third option — either you drop data, or you slow the producer down.

| Policy | Producer when full | Data loss | Use when |
|---|---|---|---|
| **Bounded queue** (above) | blocks | none | every item is meaningful (transactions, log lines, jobs) |
| **Latest-value / mailbox** | overwrites | drops stale | only the freshest matters (camera frame, sensor reading, GPS, mouse) |
| **Last-N ring (overwrite-on-full)** | overwrites oldest | drops oldest | consumer wants recent history (audio buffer, telemetry, RL replay) |
| **Triple buffer** | swaps | drops stale | latest-value, but copying is expensive (full frame, lock-free) |

The decision is about the *semantics of the data*, not the rate gap:

```
Is each item individually meaningful?
├── Yes → bounded queue (backpressure, no loss)
└── No, only the freshest matters → latest-value or last-N ring
```

A subtle point: "consumer faster than producer" needs none of this — the queue stays empty and the consumer waits. The interesting case is always *producer faster than consumer*.

## 2.2 Latest-value (mailbox)

For trivially-copyable types, one atomic suffices:

```cpp
std::atomic<int> latest = -1;
latest.store(frame);            // producer (e.g. 60 Hz camera frame number)
int snapshot = latest.load();   // consumer — intermediate values silently dropped
```

For non-trivial types, use an atomic `shared_ptr` so old values are reclaimed safely:

```cpp
std::atomic<std::shared_ptr<Frame>> slot;
slot.store(std::make_shared<Frame>(captured));   // producer
auto current = slot.load();                       // consumer
```

With a 60 Hz producer and a 10 Hz consumer, expect to see frames `0, 6, 12, 18 …` — five frames silently skipped between reads, which is exactly what you want when stale data is useless.

## 2.3 Worked example: bounded queue vs. latest-value

A minimal, self-contained program showing the two policies running back-to-back:

```cpp
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

class BoundedQueue {
public:
  explicit BoundedQueue(std::size_t capacity) : capacity_(capacity) {}

  void push(int value) {
    std::unique_lock lock(mutex_);
    not_full_.wait(lock, [this] { return queue_.size() < capacity_; });
    queue_.push(value);
    not_empty_.notify_one();
  }

  int pop() {
    std::unique_lock lock(mutex_);
    not_empty_.wait(lock, [this] { return !queue_.empty(); });
    int value = queue_.front();
    queue_.pop();
    not_full_.notify_one();
    return value;
  }

private:
  std::size_t capacity_;
  std::queue<int> queue_;
  std::mutex mutex_;
  std::condition_variable not_full_, not_empty_;
};

int main() {
  std::cout << "--- bounded queue (capacity=4, no loss) ---\n";
  {
    BoundedQueue queue(4);
    std::jthread producer([&] {
      for (int i = 0; i < 10; ++i) queue.push(i);
    });
    std::jthread consumer([&] {
      for (int i = 0; i < 10; ++i)
        std::cout << "consumed " << queue.pop() << '\n';
    });
  }

  std::cout << "--- latest value (60Hz producer, 10Hz consumer) ---\n";
  {
    std::atomic<int> latest = -1;
    std::jthread producer([&] {
      for (int frame = 0; frame < 30; ++frame) {
        latest.store(frame);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
      }
    });
    std::jthread consumer([&] {
      for (int i = 0; i < 5; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "consumed frame " << latest.load() << '\n';
      }
    });
  }
}
```

Sample output (ordering of `consumed` lines varies — `std::cout` is not synchronized):

```
--- bounded queue (capacity=4, no loss) ---
consumed 0
consumed 1
...
consumed 9
--- latest value (60Hz producer, 10Hz consumer) ---
consumed frame 6
consumed frame 12
consumed frame 18
consumed frame 24
consumed frame 29
```

The bounded-queue consumer sees every value because the producer was throttled by `not_full_.wait`. The latest-value consumer skips ~6 frames between reads because the producer is 6× faster — exactly the right behavior for a sensor feed where stale data is useless.

The example uses `std::jthread` (C++20) for RAII auto-join: the inner `{ … }` scopes ensure the first demo finishes before the second starts.

## 2.4 Cross-process: same patterns, different transport

You can't share `std::queue` or `std::atomic` across processes, but the **patterns** carry over directly. Only the transport changes:

| In-process | Cross-process |
|---|---|
| Bounded queue + mutex/cv | OS message queue, Unix socket, ROS2 topic with `RELIABLE` QoS |
| `std::atomic<T>` (latest-value) | ROS2 with `BEST_EFFORT` + `KEEP_LAST(1)`, shared-memory atomic |
| Last-N ring | ROS2 `KEEP_LAST(N)`, lock-free shared-memory ring (iceoryx) |

ROS2's QoS settings map straight onto the policy table above:

- `Reliability::RELIABLE` → bounded queue (retry, backpressure)
- `Reliability::BEST_EFFORT` → drop on congestion
- `History::KEEP_LAST(N)` → last-N ring
- `History::KEEP_ALL` → unbounded queue (paired with `RELIABLE`)

Typical configurations match the in-process patterns:

```python
# Camera publisher: stale frames useless, drop is fine
QoSProfile(reliability=BEST_EFFORT, history=KEEP_LAST, depth=1)

# Control commands: every command must arrive
QoSProfile(reliability=RELIABLE, history=KEEP_LAST, depth=10)
```

When both nodes are on the same machine, DDS implementations (FastDDS, Cyclone DDS, **iceoryx**) often use a **shared-memory ring buffer** for true zero-copy transfer — the same in-memory ring buffer pattern, just sitting in `/dev/shm` and accessed by both processes via `mmap`. See also [Inter-Process Communication and Shared Memory](ipc_shared_memory.md).

# 3. Active Object

Decouples method invocation from method execution. Callers post requests onto an actor's queue; the actor's own thread executes them serially. This guarantees the actor's state is only ever touched by one thread — no internal locking needed.

```cpp
#include <condition_variable>
#include <fstream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

class FileLogger {
  std::queue<std::string> q;
  std::mutex m;
  std::condition_variable cv;
  int remaining;  // demo: write this many messages, then exit
  std::thread worker;
public:
  FileLogger(int total) : remaining(total), worker([this] { run(); }) {}
  ~FileLogger() { worker.join(); }

  void log(std::string msg) {
    { std::scoped_lock l(m); q.push(msg); }
    cv.notify_one();
  }
private:
  void run() {
    std::ofstream out("app.log");
    while (remaining > 0) {
      std::string msg;
      {
        std::unique_lock l(m);
        cv.wait(l, [&] { return !q.empty(); });
        msg = q.front();
        q.pop();
        --remaining;
      }
      out << msg << '\n';
    }
  }
};

int main() {
  FileLogger logger(3);
  logger.log("hello");
  logger.log("from");
  logger.log("active object");
}
```

This is the foundation of the Actor model (Erlang, Akka). The thread-per-actor scaling is poor — real actor systems multiplex many actors onto a thread pool.

# 4. Reactor

A single thread waits on multiple I/O sources; on activity, it dispatches to handlers. The hand-rolled event loop:

```cpp
#include <sys/epoll.h>
#include <array>
#include <functional>
#include <unordered_map>

class Reactor {
  int epoll_fd;
  std::unordered_map<int, std::function<void()>> handlers;
public:
  Reactor() : epoll_fd(epoll_create1(0)) {}

  void register_fd(int fd, std::function<void()> h) {
    handlers[fd] = h;
    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
  }

  void run() {
    std::array<epoll_event, 64> evs;
    while (true) {
      int n = epoll_wait(epoll_fd, evs.data(), evs.size(), -1);
      for (int i = 0; i < n; ++i) handlers[evs[i].data.fd]();
    }
  }
};
```

Real systems: libuv, Boost.Asio, ACE. Reactor handlers must be **non-blocking** — a single slow handler stalls every other client.

# 5. Proactor

Like Reactor, but the OS performs the I/O *operation* asynchronously and notifies you when complete. Linux `io_uring`, Windows `IOCP`. Boost.Asio uses Proactor on Windows and emulates it on Linux.

```cpp
#include <boost/asio.hpp>
#include <array>

int main() {
  boost::asio::io_context io;
  boost::asio::ip::tcp::socket sock(io);
  std::array<char, 1024> buf;

  sock.async_read_some(boost::asio::buffer(buf),
    [](boost::system::error_code ec, std::size_t n) {
      // I/O already complete by the time we're called
    });

  io.run();
}
```

Proactor is generally a better fit for high-concurrency I/O: no readiness-then-syscall round trip.

# 6. Pipeline / Stages

A long-running operation is split into stages; each stage runs on its own thread (or pool) and feeds the next via a queue. See [Pipeline and Dataflow](pipeline_dataflow.md).

```
[reader] → q1 → [parser] → q2 → [writer]
```

Throughput is bounded by the slowest stage (Amdahl's law applied at design time). Useful when stages are heterogeneous (CPU-bound vs I/O-bound).

# 7. Fork–Join and Work Stealing

Recursive decomposition: split a task, run halves in parallel, combine results.

```cpp
#include <future>
#include <iostream>
#include <numeric>
#include <span>
#include <vector>

int sum(std::span<int> xs) {
  if (xs.size() < 1024) return std::accumulate(xs.begin(), xs.end(), 0);
  auto mid = xs.size() / 2;
  auto left = std::async(std::launch::async,
                         [=] { return sum(xs.first(mid)); });
  int right = sum(xs.last(xs.size() - mid));
  return left.get() + right;
}

int main() {
  std::vector<int> xs(10000, 1);
  std::cout << sum(xs) << '\n';  // 10000
}
```

For balanced workloads this is fine; for irregular ones, use a **work-stealing** scheduler so idle threads can poach tasks from busy ones (Intel TBB, Cilk Plus, `std::execution`'s `par_unseq` on some implementations).

# 8. Monitor Object

A class that synchronizes its own state via a member mutex. Each method takes the lock on entry. Simple and safe:

```cpp
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class Counter {
  mutable std::mutex m;
  int n = 0;
public:
  void incr() { std::scoped_lock l(m); ++n; }
  int  get() const { std::scoped_lock l(m); return n; }
};

int main() {
  Counter c;
  std::vector<std::thread> ts;
  for (int i = 0; i < 4; ++i)
    ts.emplace_back([&] { for (int k = 0; k < 1000; ++k) c.incr(); });
  for (auto& t : ts) t.join();
  std::cout << c.get() << '\n';  // 4000
}
```

Drawbacks: lock granularity is the whole object; reentrant calls deadlock; doesn't compose (calling another monitor while holding yours risks deadlock). Suits objects with simple invariants.

# 9. Read–Copy–Update (RCU)

Readers walk a data structure with no locks. Writers atomically swap in a new version, then defer reclamation of the old version until all readers have finished. Throughput on read-heavy workloads is excellent. See [Lock-Free Data Structures §9.3](../lock_free_data_structures.md#93-rcu-and-epoch-based-reclamation).

# 10. Choosing Between Them

| Need | Pattern |
|---|---|
| "Run N tasks concurrently" | Thread Pool |
| "One thread produces, another consumes" | Producer–Consumer |
| "Object accessed by many threads, must serialize" | Active Object |
| "Many clients, mostly waiting on I/O" | Reactor / Proactor |
| "Heterogeneous stages" | Pipeline |
| "Recursive parallel algorithm" | Fork–Join + work stealing |
| "Read-mostly shared data" | RCU |
| "Simple shared object, infrequent contention" | Monitor |

Don't use multiple patterns for one problem unless you've measured. Each adds queueing, synchronization, and reasoning load.

# References

- [Multithreading](../multithreading.md)
- [Event handling / Concurrency / Thread Design Patterns](../thread_design_pattern.md)
- [Lock-Free Data Structures](../lock_free_data_structures.md)
- [Pipeline and Dataflow](pipeline_dataflow.md)
- *Pattern-Oriented Software Architecture, Vol. 2*, Schmidt et al. (Reactor, Proactor, Active Object, Monitor).
- *C++ Concurrency in Action*, Anthony Williams.
