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
class ThreadPool {
    std::vector<std::jthread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex m_;
    std::condition_variable cv_;
    bool stop_ = false;
public:
    explicit ThreadPool(size_t n) {
        for (size_t i = 0; i < n; ++i) workers_.emplace_back([this](std::stop_token st){ run(st); });
    }
    ~ThreadPool() {
        { std::scoped_lock l(m_); stop_ = true; }
        cv_.notify_all();
    }
    template<class F>
    auto submit(F f) -> std::future<std::invoke_result_t<F>> {
        auto task = std::make_shared<std::packaged_task<std::invoke_result_t<F>()>>(std::move(f));
        auto fut = task->get_future();
        { std::scoped_lock l(m_); tasks_.emplace([task]{ (*task)(); }); }
        cv_.notify_one();
        return fut;
    }
private:
    void run(std::stop_token st) {
        while (true) {
            std::function<void()> job;
            { std::unique_lock l(m_); cv_.wait(l, [&]{ return stop_ || !tasks_.empty(); });
              if (stop_ && tasks_.empty()) return;
              job = std::move(tasks_.front()); tasks_.pop(); }
            job();
        }
    }
};
```

Real production pools add: bounded queues with backpressure, per-thread local queues, work-stealing (see §7), priorities, work-class isolation (don't put I/O and CPU in the same pool).

# 2. Producer–Consumer

Producers add items, consumers remove them, a queue mediates. The bounded version implements **backpressure**: when the queue is full, producers block.

```cpp
template<class T>
class BoundedQueue {
    std::queue<T> q_;
    size_t cap_;
    std::mutex m_;
    std::condition_variable not_full_, not_empty_;
public:
    explicit BoundedQueue(size_t cap) : cap_(cap) {}
    void push(T v) {
        std::unique_lock l(m_);
        not_full_.wait(l, [&]{ return q_.size() < cap_; });
        q_.push(std::move(v));
        not_empty_.notify_one();
    }
    T pop() {
        std::unique_lock l(m_);
        not_empty_.wait(l, [&]{ return !q_.empty(); });
        T v = std::move(q_.front()); q_.pop();
        not_full_.notify_one();
        return v;
    }
};
```

For lock-free variants, see [Lock-Free Data Structures](../lock_free_data_structures.md). For high-throughput SPSC use a ring buffer.

# 3. Active Object

Decouples method invocation from method execution. Callers post requests onto an actor's queue; the actor's own thread executes them serially. This guarantees the actor's state is only ever touched by one thread — no internal locking needed.

```cpp
class FileLogger {
    std::jthread worker_;
    std::queue<std::string> q_;
    std::mutex m_;
    std::condition_variable cv_;
    bool stop_ = false;
public:
    FileLogger() : worker_([this](std::stop_token st){ run(st); }) {}
    ~FileLogger() { { std::scoped_lock l(m_); stop_ = true; } cv_.notify_all(); }

    void log(std::string msg) {
        { std::scoped_lock l(m_); q_.push(std::move(msg)); }
        cv_.notify_one();
    }
private:
    void run(std::stop_token st) {
        std::ofstream out("app.log");
        while (true) {
            std::string msg;
            { std::unique_lock l(m_); cv_.wait(l, [&]{ return stop_ || !q_.empty(); });
              if (stop_ && q_.empty()) return;
              msg = std::move(q_.front()); q_.pop(); }
            out << msg << '\n';
        }
    }
};
```

This is the foundation of the Actor model (Erlang, Akka). The thread-per-actor scaling is poor — real actor systems multiplex many actors onto a thread pool.

# 4. Reactor

A single thread waits on multiple I/O sources; on activity, it dispatches to handlers. The hand-rolled event loop:

```cpp
class Reactor {
    int epoll_;
    std::unordered_map<int, std::function<void(uint32_t)>> handlers_;
public:
    Reactor() : epoll_(::epoll_create1(0)) {}
    void registerFd(int fd, uint32_t events, std::function<void(uint32_t)> h) {
        handlers_[fd] = std::move(h);
        epoll_event ev{events, {.fd = fd}};
        ::epoll_ctl(epoll_, EPOLL_CTL_ADD, fd, &ev);
    }
    void run() {
        std::array<epoll_event, 64> evs;
        while (true) {
            int n = ::epoll_wait(epoll_, evs.data(), evs.size(), -1);
            for (int i = 0; i < n; ++i) handlers_[evs[i].data.fd](evs[i].events);
        }
    }
};
```

Real systems: libuv, Boost.Asio, ACE. Reactor handlers must be **non-blocking** — a single slow handler stalls every other client.

# 5. Proactor

Like Reactor, but the OS performs the I/O *operation* asynchronously and notifies you when complete. Linux `io_uring`, Windows `IOCP`. Boost.Asio uses Proactor on Windows and emulates it on Linux.

```cpp
boost::asio::io_context io;
boost::asio::ip::tcp::socket sock{io};
sock.async_read_some(buffer(buf), [](auto ec, auto n){
    // I/O already complete by the time we're called
});
io.run();
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
int sum(std::span<int> xs) {
    if (xs.size() < 1024) return std::accumulate(xs.begin(), xs.end(), 0);
    auto mid = xs.size() / 2;
    auto left = std::async(std::launch::async, [=]{ return sum(xs.first(mid)); });
    auto right = sum(xs.last(xs.size() - mid));
    return left.get() + right;
}
```

For balanced workloads this is fine; for irregular ones, use a **work-stealing** scheduler so idle threads can poach tasks from busy ones (Intel TBB, Cilk Plus, `std::execution`'s `par_unseq` on some implementations).

# 8. Monitor Object

A class that synchronizes its own state via a member mutex. Each method takes the lock on entry. Simple and safe:

```cpp
class Counter {
    mutable std::mutex m_;
    int n_ = 0;
public:
    void incr() { std::scoped_lock l(m_); ++n_; }
    int  get() const { std::scoped_lock l(m_); return n_; }
};
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
