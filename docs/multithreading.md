# Multithreading in C++

- [1. Overview](#1-overview)
  - [1.1. Process vs Thread](#11-process-vs-thread)
  - [1.2. Shared and Per-Thread State](#12-shared-and-per-thread-state)
- [2. Creating and Terminating Threads](#2-creating-and-terminating-threads)
  - [2.1. Function Pointer](#21-function-pointer)
  - [2.2. Member Function](#22-member-function)
  - [2.3. Function Object (Functor)](#23-function-object-functor)
  - [2.4. Lambda Expression](#24-lambda-expression)
  - [2.5. Passing Arguments by Reference](#25-passing-arguments-by-reference)
  - [2.6. Move-Only Semantics](#26-move-only-semantics)
  - [2.7. Joining Threads](#27-joining-threads)
  - [2.8. Detaching Threads](#28-detaching-threads)
  - [2.9. std::jthread (C++20)](#29-stdjthread-c20)
- [3. Thread Properties](#3-thread-properties)
  - [3.1. Order of Execution](#31-order-of-execution)
  - [3.2. Hardware Concurrency and Oversubscription](#32-hardware-concurrency-and-oversubscription)
  - [3.3. Thread Identity](#33-thread-identity)
  - [3.4. Sleeping](#34-sleeping)
  - [3.5. Yielding](#35-yielding)
- [4. Synchronization](#4-synchronization)
  - [4.1. Race Conditions](#41-race-conditions)
    - [4.1.1. Race Condition vs Data Race](#411-race-condition-vs-data-race)
    - [4.1.2. Example: Lost Update](#412-example-lost-update)
    - [4.1.3. Example: Interleaved Output](#413-example-interleaved-output)
  - [4.2. Mutex](#42-mutex)
    - [4.2.1. Why Not Manual lock()/unlock()](#421-why-not-manual-lockunlock)
    - [4.2.2. std::scoped_lock](#422-stdscoped_lock)
    - [4.2.3. Deadlock](#423-deadlock)
    - [4.2.4. Avoiding Deadlock](#424-avoiding-deadlock)
    - [4.2.5. lock_guard vs scoped_lock vs unique_lock](#425-lock_guard-vs-scoped_lock-vs-unique_lock)
    - [4.2.6. unique_lock — When You Need It](#426-unique_lock--when-you-need-it)
  - [4.3. Semaphores](#43-semaphores)
    - [4.3.1. std::counting_semaphore — Limit Concurrent Access](#431-stdcounting_semaphore--limit-concurrent-access)
    - [4.3.2. std::binary_semaphore — One-Shot Signaling](#432-stdbinary_semaphore--one-shot-signaling)
  - [4.4. Condition Variables](#44-condition-variables)
    - [4.4.1. Spurious Wakeups](#441-spurious-wakeups)
    - [4.4.2. Worked Example: Producer–Consumer](#442-worked-example-producerconsumer)
  - [4.5. Choosing the Right Primitive](#45-choosing-the-right-primitive)
- [5. Async Tasks and std::future](#5-async-tasks-and-stdfuture)
  - [5.1. std::async and Launch Policy](#51-stdasync-and-launch-policy)
  - [5.2. std::packaged_task](#52-stdpackaged_task)
  - [5.3. std::promise](#53-stdpromise)
  - [5.4. std::shared_future](#54-stdshared_future)
  - [5.5. When to Use Which](#55-when-to-use-which)
- [6. std::atomic](#6-stdatomic)
  - [6.1. Atomics vs Mutex](#61-atomics-vs-mutex)
  - [6.2. Common Atomic Operations](#62-common-atomic-operations)
  - [6.3. std::atomic_flag and Spinlocks](#63-stdatomic_flag-and-spinlocks)
  - [6.4. Memory Ordering](#64-memory-ordering)
  - [6.5. compare_exchange and the ABA Problem](#65-compare_exchange-and-the-aba-problem)
- [7. Designing Thread-Safe Classes](#7-designing-thread-safe-classes)
  - [7.1. Combining Read-Then-Modify Operations](#71-combining-read-then-modify-operations)

---

# 1. Overview

A **thread** is the smallest unit of execution scheduled by the operating system. Multiple threads inside the same process run concurrently and share the process's address space, which makes communication cheap but synchronization mandatory.

Threads shine when work is parallelizable across cores or when one thread can make progress while another waits on I/O. They also matter on single-core systems for hiding latency.

If two threads run independently with no synchronization, you cannot predict which one runs faster, in what order their statements interleave, or whether they observe each other's writes promptly.

## 1.1. Process vs Thread

|  | Process | Thread |
|---|---|---|
| Address space | Private | Shared with peers |
| Creation cost | High (new VM, page tables, FDs) | Low |
| Communication | IPC (pipes, sockets, shared memory) | Shared variables |
| Crash isolation | Strong | None — one bad thread can take down the whole process |

## 1.2. Shared and Per-Thread State

Threads in the same process **share**:
- Code (instructions)
- Heap and global/static data
- Open file descriptors
- Signal handlers and current working directory
- User and group IDs

Each thread has its **own**:
- Thread ID
- Registers and stack pointer
- Stack (local variables, return addresses)
- Signal mask, priority
- `errno` (per-thread on POSIX)
- Storage declared `thread_local`

---

# 2. Creating and Terminating Threads

`std::thread` represents a single thread. Construct one with any *callable* and the new thread starts immediately:

```cpp
#include <thread>

void task();
std::thread t(task);   // task() begins running on a new thread
t.join();              // wait for it to finish
```

Callables that work as the thread function:
- A free function or function pointer
- A pointer-to-member function plus an instance
- A function object (a class with `operator()`)
- A lambda expression

## 2.1. Function Pointer

```cpp
#include <iostream>
#include <thread>

void greet(int n) {
    std::cout << "hello from thread, n = " << n << '\n';
}

int main() {
    std::thread t(greet, 42);   // arguments are forwarded after the callable
    t.join();
}
```

## 2.2. Member Function

Pass a pointer-to-member as the callable, then the instance, then the arguments:

```cpp
class Worker {
public:
    void run(int x, char c) { /* ... */ }
};

Worker w;
std::thread t1(&Worker::run, &w, 7, 'a');           // by pointer — no copy
std::thread t2(&Worker::run, w, 7, 'a');            // by value — copies w
std::thread t3(&Worker::run, std::move(w), 7, 'a'); // by move — w is gone after this
```

`std::thread` arguments are *decay-copied* by default, so passing `w` by value really does copy it into the new thread's storage. Use `&w` (raw pointer) or `std::ref(w)` if you mean to share.

## 2.3. Function Object (Functor)

A class with `operator()` is a callable:

```cpp
struct PrintN {
    void operator()(int n) const {
        std::cout << "n = " << n << '\n';
    }
};

std::thread t(PrintN{}, 5);
t.join();
```

> **Most vexing parse.** This line:
>
> ```cpp
> std::thread t(PrintN());   // ⚠️ declares a function, not a thread
> ```
>
> is parsed as a function declaration. Two ways to fix it:
>
> ```cpp
> std::thread t1((PrintN()));   // extra parens
> std::thread t2{PrintN{}};     // brace init
> ```

## 2.4. Lambda Expression

The most common style today:

```cpp
int n = 10;
std::thread t([n] { std::cout << "n = " << n << '\n'; });
t.join();
```

## 2.5. Passing Arguments by Reference

`std::thread` copies arguments into its internal storage, so a reference parameter binds to that *copy*, not your original — a classic surprise:

```cpp
void modify(std::string& msg) { msg = "new value"; }

std::string s = "old";
std::thread t(modify, s);            // ⚠️ compile error in modern compilers,
                                     //    or modifies a copy on older ones
```

To actually share the original, wrap it in `std::ref`:

```cpp
std::thread t(modify, std::ref(s));
t.join();
std::cout << s << '\n';              // "new value"
```

For read-only sharing use `std::cref`.

## 2.6. Move-Only Semantics

`std::thread` cannot be copied — it owns an OS resource. It can be moved:

```cpp
std::thread t1(task);
std::thread t2 = std::move(t1);   // t1 is now empty, t2 owns the thread
// t1.join();  // would throw — t1 is empty
t2.join();
```

Full example: [creating_and_terminating_threads.cpp](../src/multithreading/creating_and_terminating_threads.cpp).

## 2.7. Joining Threads

`t.join()` blocks the calling thread until `t` finishes. You **must** call either `join()` or `detach()` before `t` is destroyed — otherwise its destructor calls `std::terminate()`.

```cpp
std::thread t(task);
// ... do other work ...
t.join();   // wait, then continue
```

Common pattern: launch a fleet of workers, then join them at the end:

```cpp
std::vector<std::thread> pool;
for (int i = 0; i < 4; ++i)
    pool.emplace_back(worker, i);
for (auto& t : pool)
    t.join();
```

## 2.8. Detaching Threads

`t.detach()` gives up ownership; the thread runs on its own and is cleaned up by the runtime when it finishes. After detaching you cannot `join()` it again, and you have no way to communicate with it.

```cpp
std::thread t(background_log_flusher);
t.detach();   // runs in the background; no further control
```

Detaching is rarely the right choice in modern C++. If the parent thread exits before a detached thread finishes, that thread's lifetime is up to the OS — usually it's killed mid-operation. Prefer `join()`, or `std::jthread` for the auto-join behavior.

Always check `t.joinable()` before calling `join()` if there's any chance the thread was already moved-from or detached:

```cpp
if (t.joinable()) t.join();
```

Full example: [join_detach_threads.cpp](../src/multithreading/join_detach_threads.cpp).

## 2.9. std::jthread (C++20)

`std::thread` has two sharp edges: forgetting to `join()` calls `std::terminate()`, and there's no built-in way to ask a thread to stop. `std::jthread` ("joining thread") fixes both:

1. **Auto-joins on destruction** — the destructor calls `join()` for you (RAII).
2. **Cooperative cancellation** — the thread carries a `std::stop_token`; the parent flips it via `request_stop()`.

```cpp
#include <thread>
#include <chrono>
#include <iostream>

void worker(std::stop_token stoken) {
    while (!stoken.stop_requested()) {
        std::cout << "working...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "stopped cleanly\n";
}

int main() {
    std::jthread t(worker);                                // stop_token injected automatically
    std::this_thread::sleep_for(std::chrono::seconds(1));
    t.request_stop();                                      // ask the thread to exit
}                                                          // destructor: request_stop() + join()
```

If the first parameter of the callable is `std::stop_token`, `jthread` passes one in. Otherwise it works just like `std::thread`. The destructor calls `request_stop()` then `join()`, so leaving scope is always safe.

**Rule of thumb:** prefer `std::jthread` over `std::thread` in new C++20 code. Use `std::thread` only when targeting an older standard or when you genuinely need detach semantics.

### How std::stop_token actually works

There are three cooperating types in `<stop_token>`:

| Type | Role |
|---|---|
| `std::stop_source` | The "issuer." Owns a shared state. Calling `request_stop()` on it flips a flag that everyone observing the same state can see. |
| `std::stop_token` | The "observer." Cheap, copyable, hands out a read-only view of the state. The thread checks `stop_requested()` on it. |
| `std::stop_callback` | RAII helper. When the stop is requested, runs a callback you registered. Useful for **waking up** a thread that's blocked on something other than a polling loop. |

Inside `std::jthread`, the framework creates a `stop_source` and passes a `stop_token` from it into your function. The `jthread`'s `request_stop()` is just `stop_source::request_stop()`.

**Cooperative.** Nothing forces the worker to stop — it must check the token itself (or have it wired into a callback). The OS does not interrupt the thread.

**Once-shot.** Once requested, the state stays "stop requested" forever. There's no reset.

**Multiple observers, one source.** You can copy the token freely, hand it to many sub-threads, and any of them can see the request:

```cpp
std::stop_source src;
std::jthread a(worker, src.get_token());
std::jthread b(worker, src.get_token());
// later:
src.request_stop();   // wakes both a and b
```

This is how you'd implement a "stop the whole job" button.

### Example: stop_callback to interrupt a blocking wait

A polling worker is fine, but most real threads block on something — a condition variable, a socket read, a sleep. A naïve `while (!stoken.stop_requested())` loop won't help if the thread is parked inside `cv.wait()`. That's what `stop_callback` is for: it runs a function the moment a stop is requested, and you can use that function to wake the blocked thread.

The standard library hands you the most common case for free: `std::condition_variable_any::wait` has overloads that take a `stop_token` and return early when stop is requested.

```cpp
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <queue>

std::mutex                    mu;
std::condition_variable_any   cv;
std::queue<int>               jobs;

void worker(std::stop_token stoken) {
    while (true) {
        std::unique_lock lock(mu);

        // Returns false if stop was requested AND queue is still empty.
        const bool got = cv.wait(lock, stoken,
                                 [] { return !jobs.empty(); });
        if (!got) {
            std::cout << "worker stopping\n";
            return;
        }

        int job = jobs.front();
        jobs.pop();
        lock.unlock();
        std::cout << "processing " << job << '\n';
    }
}

int main() {
    std::jthread t(worker);

    {
        std::scoped_lock lock(mu);
        jobs.push(1);
        jobs.push(2);
    }
    cv.notify_all();

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // jthread destructor: request_stop() → wakes the cv.wait → join()
}
```

The key line is `cv.wait(lock, stoken, predicate)`. Internally that registers a `stop_callback` so the moment `request_stop()` runs, the callback calls `cv.notify_all()` — which wakes our wait. No polling, no spurious-timeout heuristic, no manual plumbing.

For your own blocking primitive (e.g. interrupting a custom socket read), wire it up by hand:

```cpp
void blocking_worker(std::stop_token stoken) {
    std::stop_callback cb(stoken, [] {
        // runs when request_stop() is called
        cancel_blocking_io();   // your code: signal the I/O to abort
    });

    do_blocking_io();           // returns early when canceled
}
```

The callback runs on whatever thread called `request_stop()`. It must be safe to invoke from any thread.

---

# 3. Thread Properties

## 3.1. Order of Execution

The OS scheduler decides which thread runs and when. Treat the order as **random** unless you've added explicit synchronization. Two consecutive `std::thread` constructions don't guarantee the first one starts first.

## 3.2. Hardware Concurrency and Oversubscription

Creating more threads than the CPU can run simultaneously slows you down: the kernel has to context-switch between them, which costs cache state and a few microseconds per swap.

```cpp
unsigned hw = std::thread::hardware_concurrency();
std::cout << "Cores available: " << hw << '\n';   // 0 means "unknown"
```

A reasonable default for CPU-bound thread pools is `hardware_concurrency()`. For I/O-bound work (where threads spend most of their time waiting), a higher count can be better.

## 3.3. Thread Identity

Each thread has a unique ID:

```cpp
std::cout << "this thread: " << std::this_thread::get_id() << '\n';

std::thread t([] {
    std::cout << "child thread: " << std::this_thread::get_id() << '\n';
});
std::cout << "child's id seen from parent: " << t.get_id() << '\n';
t.join();
```

`std::thread::id` is comparable (`==`, `<`) and hashable, so you can store it in `std::set` / `std::unordered_map` for per-thread bookkeeping.

Full example: [differentiating_between_threads.cpp](../src/multithreading/differentiating_between_threads.cpp).

## 3.4. Sleeping

```cpp
#include <chrono>
#include <thread>

std::this_thread::sleep_for(std::chrono::milliseconds(250));
std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::seconds(1));
```

Prefer the `<chrono>` overloads over the C-style `usleep` — they're type-safe, clearer at the call site, and don't get confused between µs and ms.

Full example: [sleeping_threads.cpp](../src/multithreading/sleeping_threads.cpp).

## 3.5. Yielding

`std::this_thread::yield()` is a hint to the scheduler that this thread has nothing useful to do right now. Other ready threads may run instead:

```cpp
while (!flag.load(std::memory_order_acquire))
    std::this_thread::yield();   // back off rather than burning a core
```

It's a hint, not a guarantee. Don't rely on yield for correctness, only for friendliness.

---

# 4. Synchronization

A **critical section** is code that must not run on two threads at once because it accesses shared mutable state. Without synchronization you get **race conditions**: results that depend on scheduler timing.

Common primitives, roughly cheapest to most expensive:
1. `std::atomic` — single-variable lock-free updates
2. Mutexes (`std::mutex`, `std::shared_mutex`)
3. Semaphores (`std::counting_semaphore`)
4. Condition variables (`std::condition_variable`)

## 4.1. Race Conditions

A race condition occurs when two threads access the same data and at least one writes, with no ordering between them. Outcomes depend on which thread won the race that particular run — making bugs hard to reproduce and even harder to diagnose.

### 4.1.1. Race Condition vs Data Race

These terms get used interchangeably but mean different things:

- **Data race** — *unsynchronized* concurrent access to the same memory location, with at least one write. The C++ standard says this is **undefined behavior**: anything can happen, including torn reads, missing updates, or a compiler optimizing the code based on the assumption a race can't occur.
- **Race condition** — a *logical* bug where the program's correctness depends on thread interleaving, even when every individual access is properly synchronized. (Example: check-then-act sequences like `if (!exists(k)) insert(k, v);` where two threads both see "not exists" before either inserts.)

Fixing data races needs synchronization primitives. Fixing race conditions needs redesigning the API so the dangerous interleaving is impossible (see [§7.1](#71-combining-read-then-modify-operations)).

### 4.1.2. Example: Lost Update

```cpp
class Wallet {
public:
    int balance = 0;
    void deposit(int amount) {
        balance = balance + amount;   // read → add → write: three steps, not one
    }
};
```

The line `balance = balance + amount` is **not atomic**. Compiled, it's at least:

1. Load `balance` into a register.
2. Add `amount`.
3. Store the register back to `balance`.

Two threads can interleave:

| Thread 1 | Thread 2 | balance |
|---|---|---|
| load balance (=10) |  | 10 |
|  | load balance (=10) | 10 |
| add 5 → reg=15 |  | 10 |
|  | add 5 → reg=15 | 10 |
| store 15 |  | 15 |
|  | store 15 | 15 |

Two `+5` deposits, but the final balance is 15 instead of 20 — one update was lost. Fix it with a mutex (§4.2) or `std::atomic<int>` for a counter (§6).

### 4.1.3. Example: Interleaved Output

`std::cout` is itself a shared resource. Two threads writing to it produce garbled output:

```cpp
#include <iostream>
#include <thread>

void log(const std::string& tag) {
    for (int i = 0; i < 5; ++i)
        std::cout << "[" << tag << "] " << i << '\n';
}

int main() {
    std::thread a(log, "A");
    std::thread b(log, "B");
    a.join();
    b.join();
}
```

Possible (messy) output:

```
[A] 0
[B] [A] 10
[A] 2
[B] 1[A] 3
```

The fix is to wrap the print in a mutex — see §4.2.

Full example: [race_condition.cpp](../src/multithreading/race_condition.cpp).

## 4.2. Mutex

A **mutex** (mutual exclusion) guarantees that only one thread at a time holds it. Threads that try to lock an already-held mutex block until it's released.

### 4.2.1. Why Not Manual lock()/unlock()

```cpp
std::mutex mu;

void bad() {
    mu.lock();
    if (some_check())
        return;            // ⚠️ leaks the lock — mutex stays held forever
    do_work();
    mu.unlock();
}
```

If an exception is thrown, or you take an early return, or you forget the `unlock()`, the mutex stays locked and every other thread that wants it blocks forever. **Always use a RAII wrapper.**

### 4.2.2. std::scoped_lock

`std::scoped_lock` (C++17) is the modern default. It locks on construction, unlocks on destruction, and accepts any number of mutexes — when given more than one, it acquires them atomically using a deadlock-avoidance algorithm.

```cpp
#include <iostream>
#include <mutex>
#include <thread>

std::mutex io_mu;

void log(const std::string& tag, int i) {
    std::scoped_lock lock(io_mu);          // locks here
    std::cout << "[" << tag << "] " << i << '\n';
}                                          // unlocks here, even on exception

int main() {
    std::thread a([] { for (int i = 0; i < 5; ++i) log("A", i); });
    std::thread b([] { for (int i = 0; i < 5; ++i) log("B", i); });
    a.join();
    b.join();
}
```

Output is now interleaved by line (the granularity at which we hold the lock), but never garbled within a line.

For multiple mutexes (e.g. transferring between two accounts):

```cpp
struct Account { std::mutex mu; int balance; };

void transfer(Account& from, Account& to, int amount) {
    std::scoped_lock lock(from.mu, to.mu);   // both at once, deadlock-free
    from.balance -= amount;
    to.balance   += amount;
}                                            // both released here
```

Full examples: [mutex.cpp](../src/multithreading/mutex.cpp), [scoped_lock.cpp](../src/multithreading/scoped_lock.cpp).

### 4.2.3. Deadlock

A deadlock happens when two threads each hold a mutex the other one wants. Naively locking two mutexes in different orders is the textbook case:

```cpp
std::mutex mu1, mu2;

// Thread A
void func_a() {
    std::scoped_lock l1(mu1);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::scoped_lock l2(mu2);   // blocks if Thread B already holds mu2
    /* ... */
}

// Thread B
void func_b() {
    std::scoped_lock l1(mu2);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::scoped_lock l2(mu1);   // blocks if Thread A already holds mu1
    /* ... */
}
```

Both threads block waiting on the other — neither can ever proceed.

**Solution:** lock all required mutexes in a single `std::scoped_lock`. The constructor uses a deadlock-avoidance algorithm (`std::lock`) and order no longer matters:

```cpp
void func_a_safe() {
    std::scoped_lock lock(mu1, mu2);   // safe regardless of caller order
    /* ... */
}

void func_b_safe() {
    std::scoped_lock lock(mu2, mu1);   // also safe
    /* ... */
}
```

Full example: [dead_lock.cpp](../src/multithreading/dead_lock.cpp).

### 4.2.4. Avoiding Deadlock

- **Prefer locking a single mutex.** Most class designs can be reduced to one mutex.
- **Don't call user-provided code (callbacks, virtual functions) while holding a lock.** They might re-enter and try to lock again.
- **When you must lock multiple mutexes, use a single `std::scoped_lock`** — never two nested ones.
- **Keep critical sections small** — but not so small that lock/unlock overhead dominates the work.
- **Define a global lock order** if you can't acquire all locks at once. Document it; everyone follows the same order.

### 4.2.5. lock_guard vs scoped_lock vs unique_lock

| Wrapper | # mutexes | Can unlock/relock? | When to use |
|---|---|---|---|
| `std::scoped_lock` (C++17) | one or many | no | **Default choice.** Use everywhere. |
| `std::lock_guard` (C++11) | exactly one | no | Legacy code only — `scoped_lock` is a strict superset. |
| `std::unique_lock` (C++11) | exactly one | yes | When you need `condition_variable::wait`, deferred locking, `try_lock`, or to unlock partway through a scope. Slightly more expensive than `scoped_lock`. |

**Rule of thumb:** use `std::scoped_lock` by default. Reach for `std::unique_lock` only when condition variables or manual unlock/relock are involved. You can ignore `std::lock_guard` in new code.

### 4.2.6. unique_lock — When You Need It

The two cases you'll actually meet `std::unique_lock` are:

1. **`std::condition_variable::wait` requires it** (because `wait` needs to unlock and relock).
2. **You want to unlock and relock several times within one scope** (e.g. reduce contention while doing local work).

```cpp
std::unique_lock<std::mutex> lock(mu, std::defer_lock);   // not yet locked
// ... work that doesn't need the lock ...
lock.lock();
// ... critical section ...
lock.unlock();
// ... more lock-free work ...
lock.lock();
// ... another critical section ...
```

Constructor flags:
- *(default)* — locks immediately.
- `std::defer_lock` — don't lock yet.
- `std::try_to_lock` — non-blocking attempt; check `lock.owns_lock()` afterward.
- `std::adopt_lock` — assume the caller already holds the mutex.

For one-time initialization across threads, prefer `std::call_once` with a `std::once_flag` over rolling your own check-then-lock:

```cpp
std::once_flag flag;
std::call_once(flag, [&]{ file.open("log.txt"); });
```

Full example: [unique_lock.cpp](../src/multithreading/unique_lock.cpp).

## 4.3. Semaphores

A **semaphore** is a counter with two atomic operations: `acquire()` decrements and blocks if zero; `release()` increments and wakes one waiter. Where a mutex enforces *exclusive* ownership by exactly one thread, a semaphore controls *how many* threads can pass at once.

> **Are semaphores actually used?** Yes — but for **specific** problems, not as a general-purpose primitive. The three places they show up most:
>
> 1. **Resource pools** — capping concurrent connections to a database, sockets to a service, file handles, GPU contexts.
> 2. **Rate limiting / throttling** — at most N requests in flight, the rest queue up.
> 3. **Single-shot signaling between threads** — "data is ready," "shutdown started." A binary semaphore is often simpler than a `mutex + condition_variable` pair for this.
>
> Real-world appearances: thread pools (worker count), web crawlers (concurrent fetches), video encoders (GPU/encoder slots), embedded RTOS code (interrupts releasing semaphores that wake tasks).
>
> What they're **not** for: protecting a critical section (use a mutex — semaphores can't tell if the "owner" releases) or waiting for a complex predicate over shared state (use a condition variable). For those, mutex and CV remain the workhorses.

C++20 (`<semaphore>`) provides two flavors:

| Type | Use case |
|---|---|
| `std::counting_semaphore<N>` | Limit *N* concurrent users of a resource (connection pool, rate limiter, bounded buffer slots). |
| `std::binary_semaphore` | Alias for `counting_semaphore<1>`. Used for **signaling** between threads — one waits, another wakes it. |

### 4.3.1. std::counting_semaphore — Limit Concurrent Access

Cap the number of in-flight database queries at three:

```cpp
#include <semaphore>
#include <thread>
#include <vector>

std::counting_semaphore<3> db_slots(3);   // up to 3 concurrent queries

void query(int id) {
    db_slots.acquire();                   // blocks if 3 are already in flight
    do_db_work(id);
    db_slots.release();                   // free a slot
}

int main() {
    std::vector<std::jthread> pool;
    for (int i = 0; i < 10; ++i)
        pool.emplace_back(query, i);
}
```

The first three workers run immediately; the rest queue up at `acquire()` until a slot is released.

### 4.3.2. std::binary_semaphore — One-Shot Signaling

A producer thread prepares some data, and a consumer thread waits for the "go" signal:

```cpp
#include <semaphore>
#include <thread>

std::binary_semaphore data_ready(0);      // starts "empty" — consumer will block

int shared_value = 0;

void producer() {
    shared_value = compute();             // prepare
    data_ready.release();                 // signal: "go"
}

void consumer() {
    data_ready.acquire();                 // block until producer signals
    use(shared_value);
}
```

A binary semaphore and a condition variable can both signal, but `binary_semaphore::release()` doesn't need a held mutex and isn't subject to spurious wakeups.

Full example: [semaphor.cpp](../src/multithreading/semaphor.cpp).

### 4.3.3. Mutex vs Semaphore

They look superficially similar — both can make a thread block until another thread takes some action — but they answer different questions. A mutex answers *"who currently owns this resource?"*; a semaphore answers *"how many slots are left?"*.

| Aspect | `std::mutex` | `std::counting_semaphore<N>` |
|---|---|---|
| **Conceptual model** | Exclusive ownership token | Counter of available slots |
| **Counter range** | 0 or 1 (locked / unlocked) | 0 to *N* |
| **Ownership** | Yes — only the locking thread may unlock | None — *any* thread may `release()` |
| **Acquire/release symmetry** | Must be the same thread | Different threads OK (often the point) |
| **Recursive locking by owner** | UB on plain `std::mutex` (use `recursive_mutex`) | N/A — no concept of owner |
| **Initial state** | Unlocked | You choose: `counting_semaphore<N>(k)` starts with *k* slots free |
| **Primary use** | Protect a critical section | Limit concurrency, signal across threads |
| **Pairs naturally with** | `std::condition_variable` for waiting on predicates | Nothing — `acquire()` is already the wait |
| **Priority inversion mitigation** | Some platforms support priority inheritance | No |

**When to reach for which:**

- **Protecting shared data** — mutex. A semaphore *can* be initialized to 1 and used like a mutex ("binary semaphore as lock"), but it loses ownership tracking: thread A could `acquire()` and thread B could `release()`, silently breaking your invariants. Use a mutex.
- **Capping concurrent users of a resource** (connection pool, GPU slots, bounded buffer) — semaphore. A mutex only gives you *one* user at a time; a `counting_semaphore<N>` gives you *N*.
- **Signaling "event happened" across threads** — `binary_semaphore` is often the simplest option (no mutex, no predicate, no spurious wakeups). Use a condition variable instead when waiters need to re-check a complex predicate over shared state.
- **Producer/consumer queue with backpressure** — typically *both*: a mutex protecting the queue's internals, plus semaphores counting filled slots and empty slots.

**Why "binary semaphore as mutex" is a trap:** the mutex's ownership invariant is what makes higher-level patterns work. Lock hierarchies for deadlock avoidance, RAII guards (`scoped_lock`), `condition_variable::wait` (which requires a `unique_lock<mutex>`), and priority inheritance all assume one specific thread owns the lock. A semaphore has no owner, so none of those apply.

A useful mental shortcut: **mutex = lock, semaphore = counter**. If you find yourself thinking "I want to lock this," reach for a mutex. If you're thinking "I want at most N of these at once" or "I want to wake another thread," reach for a semaphore.

## 4.4. Condition Variables

Without a condition variable, a thread that wants to wait for some predicate (`queue not empty`, `data ready`, `stop requested`) has only two options:

1. **Spin** — keep checking in a loop, burning CPU.
2. **Sleep + retry** — same idea, less wasteful, but sluggish.

A `std::condition_variable` lets a thread sleep efficiently until *another* thread changes the predicate and calls `notify_one()` / `notify_all()`. It's always paired with a mutex that protects the shared state.

The pattern:

- **Waiter:** lock mutex → call `cv.wait(lock, predicate)` → on wakeup, mutex is held again and predicate is true.
- **Notifier:** lock mutex → modify shared state → unlock (or stay locked) → call `cv.notify_one()`.

### 4.4.1. Spurious Wakeups

A thread blocked in `cv.wait(lock)` can wake up *without* anyone calling `notify_*`. The standard allows it, and POSIX implementations exhibit it in practice. This is why you must always re-check the predicate:

```cpp
// ❌ BAD — wakes once and proceeds even if the predicate is still false
cv.wait(lock);
if (!ready) { /* corrupted logic */ }

// ✅ GOOD — explicit while loop
while (!ready)
    cv.wait(lock);

// ✅ BEST — predicate form, equivalent to the loop above
cv.wait(lock, []{ return ready; });
```

Same rule for `wait_for` and `wait_until`: always re-check after they return.

### 4.4.2. Worked Example: Worker Notifies Main

A minimal pattern: main launches a worker, blocks until the worker is done, then continues.

```cpp
#include <condition_variable>
#include <mutex>
#include <thread>
#include <iostream>

std::mutex              mu;
std::condition_variable cv;
bool done = false;

void worker() {
    for (int i = 0; i < 5; ++i)
        std::this_thread::sleep_for(std::chrono::milliseconds(200));   // pretend to work
    {
        std::scoped_lock lock(mu);
        done = true;
    }
    cv.notify_one();              // wake main
}

int main() {
    std::jthread t(worker);
    std::unique_lock lock(mu);
    cv.wait(lock, []{ return done; });
    std::cout << "worker finished\n";
}
```

Four things worth absorbing:

- **The mutex protects `done`.** Both threads touch it; both lock `mu` before reading or writing. Without the mutex it's a data race — undefined behavior — and you also expose yourself to the lost-wakeup race below.
- **`unique_lock` on the wait side, `scoped_lock` on the notify side.** `cv.wait` *requires* `std::unique_lock<std::mutex>` because it needs to atomically unlock the mutex while sleeping and relock it on wake-up. `scoped_lock` doesn't expose lock/unlock, so it can't be used with `cv.wait`. The worker doesn't wait, so `scoped_lock` is fine there.
- **The predicate form of `wait`** (`cv.wait(lock, predicate)`) handles spurious wakeups for free — it re-checks the predicate on every wake-up.
- **`notify_*` outside the lock.** Releasing first is a tiny optimization that avoids the woken thread immediately blocking on the still-held lock. That's why the worker scopes the lock to `{ done = true; }` and calls `notify_one()` *after* the block ends.

**Why the mutex matters even with `notify_one`.** Without the mutex you can hit the classic lost-wakeup race:

1. Main checks `done` → false.
2. Worker sets `done = true` and calls `notify_one()`.
3. Main calls `cv.wait` — and sleeps forever, because the notify already fired.

The mutex serializes "check predicate" (main) and "set predicate + notify" (worker) so this interleaving cannot happen: while main holds `mu` inside `cv.wait`'s predicate check, worker can't set `done`; once main is sleeping inside `wait`, the lock is released and worker can run, set `done`, and notify.

`wait`'s contract:

```cpp
cv.wait(unique_lock<mutex>& lock, Predicate pred);
```

Atomically: unlocks `lock`, blocks the thread, reacquires `lock` on wakeup, re-checks `pred`, returns when `pred` is true.

Full example: [condition_variable.cpp](../src/multithreading/condition_variable.cpp).

## 4.5. Choosing the Right Primitive

| Need | Use |
|---|---|
| Protect a small block of code accessing shared data | `std::mutex` + `std::scoped_lock` |
| One thread signals another that it's done | `std::binary_semaphore` |
| Cap the number of concurrent users of a resource | `std::counting_semaphore<N>` |
| Wait until some condition over shared state holds | `std::mutex` + `std::condition_variable` |
| Atomic counter, flag, or pointer | `std::atomic` |
| One-time initialization | `std::once_flag` + `std::call_once` |

Refs: [Mutex vs semaphore](https://barrgroup.com/embedded-systems/how-to/rtos-mutex-semaphore), [SO: when semaphore over mutex](https://stackoverflow.com/questions/2350544/in-what-situation-do-you-use-a-semaphore-over-a-mutex-in-c), [SO: condition variables vs mutex](https://stackoverflow.com/questions/4742196/advantages-of-using-condition-variables-over-mutex), [Harvard CS61 Synch3](https://cs61.seas.harvard.edu/site/2018/Synch3/).

---

# 5. Async Tasks and std::future

A `std::future<T>` is a *handle* to a value that will be available later. The producer side is one of:

1. `std::async` — launches a task and gives you back a future.
2. `std::packaged_task` — wraps a callable so its return value goes into a future.
3. `std::promise` — manual: you create both ends and set the value yourself.

In all three cases the consumer calls `future.get()` to retrieve the value (blocking if needed). `get()` can be called **only once**; calling it twice throws `std::future_error`.

## 5.1. std::async and Launch Policy

`std::async(f, args...)` runs `f(args...)` and returns a `std::future` for the result. The launch policy controls *how*:

| Policy | Behavior |
|---|---|
| `std::launch::async` | Runs `f` on a new thread immediately. |
| `std::launch::deferred` | Doesn't run until `future.get()` / `wait()` is called — and then synchronously on the *calling* thread. |
| `std::launch::async \| std::launch::deferred` | **Default.** Implementation chooses. **Avoid relying on it** — you can't tell which you got. |

```cpp
#include <future>
#include <iostream>
#include <chrono>

int factorial(int n) {
    int v = 1;
    for (int i = 1; i <= n; ++i) v *= i;
    return v;
}

int main() {
    auto fut = std::async(std::launch::async, factorial, 10);
    // ... do other work ...
    std::cout << "10! = " << fut.get() << '\n';   // blocks if not yet ready
}
```

**Always pass an explicit policy** — usually `std::launch::async`. The default is a portability trap.

Polling for completion:

```cpp
auto fut = std::async(std::launch::async, slow_lookup, "example.com");

while (true) {
    auto status = fut.wait_for(std::chrono::milliseconds(100));
    if (status == std::future_status::ready) break;
    if (status == std::future_status::deferred) { fut.wait(); break; }
    std::cout << "still waiting...\n";
}
std::cout << "result: " << fut.get() << '\n';
```

> **Gotcha:** the destructor of a future returned by `std::async` *blocks* until the task finishes. So `std::async(std::launch::async, slow_thing);` (no variable) launches and immediately waits — defeating the point. Always store the future.

## 5.2. std::packaged_task

A `packaged_task` wraps a callable and exposes its result as a future. Useful when you want to control where/when the task runs (a custom thread, a task queue, a thread pool):

```cpp
#include <future>
#include <thread>
#include <iostream>

int main() {
    std::packaged_task<int(int)> task(factorial);
    std::future<int> fut = task.get_future();

    std::thread t(std::move(task), 10);   // run the task on this thread
    t.join();

    std::cout << "10! = " << fut.get() << '\n';
}
```

It's the building block for thread pools — the pool stores `packaged_task`s in a queue, and worker threads pick them up.

Full example: [packaged_task.cpp](../src/multithreading/packaged_task.cpp).

Refs: [std::promise on SO](https://stackoverflow.com/questions/11004273/what-is-stdpromise), [cppreference](https://en.cppreference.com/w/cpp/thread/packaged_task), [thread pool implementation](https://codereview.stackexchange.com/questions/221617/thread-pool-c-implementation), [video](https://www.youtube.com/watch?v=eWTGtp3HXiw), [Vorbrodt thread pool](https://vorbrodt.blog/2019/02/12/simple-thread-pool/).

## 5.3. std::promise

A `std::promise<T>` is the manual mode: you hand the future to the consumer, then call `set_value(v)` (or `set_exception`) when ready. The consumer's `get()` unblocks.

Pass a value into a child thread *some time after* it has started:

```cpp
#include <future>
#include <thread>
#include <iostream>

int factorial_from_future(std::future<int> in) {
    int n = in.get();             // blocks until promise is fulfilled
    int v = 1;
    for (int i = 1; i <= n; ++i) v *= i;
    return v;
}

int main() {
    std::promise<int> p;
    std::future<int> input  = p.get_future();
    std::future<int> result = std::async(std::launch::async,
                                          factorial_from_future, std::move(input));

    // ... some time later ...
    p.set_value(10);

    std::cout << "10! = " << result.get() << '\n';
}
```

Reporting an error instead of a value:

```cpp
p.set_exception(std::make_exception_ptr(std::runtime_error("input invalid")));
```

The consumer's `get()` will rethrow that exception. Promises and futures are move-only.

## 5.4. std::shared_future

`std::future<T>` can be `get()` only once. If multiple threads need to read the same result, share a `std::shared_future<T>`:

```cpp
std::promise<int> p;
std::shared_future<int> shared = p.get_future().share();

auto a = std::async(std::launch::async, [shared] { return shared.get() + 1; });
auto b = std::async(std::launch::async, [shared] { return shared.get() * 2; });

p.set_value(10);
std::cout << a.get() << ' ' << b.get() << '\n';   // 11 20
```

Each consumer holds its own copy of the `shared_future` and can call `get()` independently.

Full example: [async_future_promise.cpp](../src/multithreading/async_future_promise.cpp).

## 5.5. When to Use Which

| You want… | Use |
|---|---|
| Run a function asynchronously, get its return value | `std::async(std::launch::async, f, args)` |
| Run on *your* thread (custom pool, specific OS thread) | `std::packaged_task` + `std::thread` |
| Deliver a value computed by code that wasn't designed around futures | `std::promise` |
| Multiple readers of one result | `std::shared_future` |

Refs: [SO: when promise over async/packaged_task](https://stackoverflow.com/questions/17729924/when-to-use-promise-over-async-or-packaged-task).

## 5.6. std::future vs std::condition_variable

Both can make one thread wait until another thread does something, so the boundary between them isn't always obvious. The toy worker-notifies-main example from §4.4.2 could be written far more simply as:

```cpp
auto fut = std::async(std::launch::async, []{ /* work */ return 42; });
int result = fut.get();   // blocks until worker is done
```

No mutex, no `done` flag, no CV. **For one-shot "compute a value, hand it back," `future` wins.** Use it.

But futures have a strict shape, and condition variables cover everything outside it:

| | `std::future` (+ `promise`/`async`) | `std::condition_variable` |
|---|---|---|
| **Signaling model** | One-shot: value (or exception) set exactly once | Repeated: signal each time a predicate may have changed |
| **What you wait on** | "The result is ready" | Any predicate over shared state you can write |
| **Number of waiters** | One (`future`) or many (`shared_future`) — but all see the *same* one-time value | Many — `notify_all` wakes them all, each re-checks its own predicate |
| **Direction** | Producer → consumer only | Bidirectional (producer/consumer queue: each side blocks the other) |
| **Carries a value?** | Yes — that's its whole point | No — you read shared state yourself under the lock |
| **Threading** | `async(launch::async)` *spawns* a thread for you | Agnostic — works with any thread you already have |

**Where futures don't fit:**

1. **Repeated signaling.** A 1000-item producer/consumer queue can't be modeled with one future. Every push is a fresh event; CV signals each one.
2. **Predicates over mutable state.** *"Wait until queue size > threshold,"* *"wait until shutdown is requested AND drain is complete,"* *"wait until any of these N flags is set."* The waiter, not the producer, decides what counts as "ready" — a future can only signal "the one value I was created for is now set."
3. **Bidirectional sync.** Bounded buffer: producer blocks when full, consumer blocks when empty. Both sides wait and wake repeatedly. No future shape captures this.
4. **You don't own the thread.** If the work is already happening on an event loop, a worker pool, or a hardware-driven callback, there's no `async` call to make. CV doesn't care who's doing the work.

**The `~future` from `async` gotcha.** A future returned by `std::async(std::launch::async, ...)` blocks in its destructor until the task finishes. If you forget to call `get()`, your "fire and forget" silently turns into "block at end of scope." CV + flag has no such surprise — but you also have to manage the synchronization yourself.

**Rule of thumb:** if the problem reads as *"compute X, give me X"*, use `future`. If it reads as *"wake me when this condition over shared state holds, possibly more than once, possibly with multiple waiters"*, use a condition variable. The §4.4.2 example sits on that boundary — `future` is the cleaner real-world choice for that exact shape; the CV version is there because that section is teaching CVs.

---

# 6. std::atomic

`std::atomic<T>` makes operations on `T` indivisible: no other thread observes a half-written value, and the compiler/CPU is told it must not reorder these operations across this point in ways that would break inter-thread visibility.

## 6.1. Atomics vs Mutex

Mutexes are correct but heavy: lock/unlock costs tens to hundreds of nanoseconds, and contention forces threads to sleep. For a single counter, flag, or pointer swap, an atomic op is often **10–100×** faster and lock-free.

```cpp
#include <atomic>
#include <thread>
#include <vector>
#include <iostream>

std::atomic<int> counter{0};

int main() {
    std::vector<std::jthread> pool;
    for (int i = 0; i < 8; ++i)
        pool.emplace_back([] {
            for (int j = 0; j < 100'000; ++j)
                ++counter;            // atomic read-modify-write — no mutex
        });
    for (auto& t : pool) t.join();
    std::cout << counter << '\n';      // exactly 800000
}
```

A plain `int counter;` here would lose updates non-deterministically. A mutex would be ~10× slower.

**What types are supported?** Anything `TriviallyCopyable` (no user-defined copy/move/destructor). Built-ins (`int`, `bool`, pointers) and small POD structs are typical. Whether the type is *lock-free* depends on hardware:

```cpp
std::atomic<int> a;
std::cout << a.is_lock_free() << '\n';    // usually true for word-sized types

std::atomic<std::array<int, 16>> big;
std::cout << big.is_lock_free() << '\n';  // usually false — uses an internal mutex
```

If `is_lock_free()` is false, the standard library uses an internal mutex behind your back — performance benefit is gone. Stick to word-sized types when you care.

## 6.2. Common Atomic Operations

| Operation | Effect |
|---|---|
| `load()` | Atomically read |
| `store(v)` | Atomically write |
| `exchange(v)` | Atomically swap, return old value |
| `fetch_add(n)` / `fetch_sub(n)` | `+= n` / `-= n`, return old value |
| `fetch_and` / `fetch_or` / `fetch_xor` | Bitwise ops |
| `compare_exchange_weak` / `_strong` | Compare-and-swap (CAS) — see §6.5 |

```cpp
std::atomic<int> x{0};
int old  = x.fetch_add(5);    // x is now 5,  old  = 0
int prev = x.exchange(42);    // x is now 42, prev = 5
```

Atomic types overload arithmetic and compound-assignment operators, so `++x`, `x += 3`, `x &= mask` all do the right thing — but **`x = x + 1` is two operations** and is *not* atomic. Use `++x` or `x.fetch_add(1)`.

## 6.3. std::atomic_flag and Spinlocks

`std::atomic_flag` is the only atomic type **guaranteed lock-free** on every platform. It supports just `test_and_set` and `clear` — enough to build a primitive spinlock:

```cpp
class Spinlock {
    std::atomic_flag locked = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (locked.test_and_set(std::memory_order_acquire))
            std::this_thread::yield();   // back off to be friendly
    }
    void unlock() {
        locked.clear(std::memory_order_release);
    }
};
```

Spinlocks are useful when contention is rare and the critical section is microseconds short. Otherwise prefer `std::mutex` — spinning burns CPU.

## 6.4. Memory Ordering

When a thread writes to memory, *when* other threads see the write — and the order they see it relative to other writes — is governed by the **memory model**. Every atomic operation takes an optional `std::memory_order` argument:

| Order | Cost | Meaning |
|---|---|---|
| `memory_order_relaxed` | cheapest | Atomicity only. No ordering guarantees with surrounding operations. |
| `memory_order_acquire` | medium | On a load: no later read/write in this thread can be reordered before it. Pairs with `release`. |
| `memory_order_release` | medium | On a store: no earlier read/write in this thread can be reordered after it. Pairs with `acquire`. |
| `memory_order_acq_rel` | medium | Both, for read-modify-write ops. |
| `memory_order_seq_cst` | most expensive | **Default.** Sequentially consistent: a single global order all threads agree on. |

### When `relaxed` is enough

A pure counter that's only read at the end:

```cpp
std::atomic<int> hits{0};

void on_request() {
    hits.fetch_add(1, std::memory_order_relaxed);   // no ordering needed
}
```

The increment is still atomic (no torn reads or lost updates), but the compiler is free to reorder surrounding code.

### Acquire/release for a producer–consumer flag

```cpp
std::atomic<bool> ready{false};
int data = 0;

// Producer
data = 42;                                          // (1) plain write
ready.store(true, std::memory_order_release);       // (2) publishes (1)

// Consumer
while (!ready.load(std::memory_order_acquire)) {}   // (3) sees (2) → also sees (1)
assert(data == 42);                                 // (4) guaranteed
```

`release` on (2) means everything *before* it in this thread (including the write to `data`) is visible to any other thread that does an `acquire` load that observes the released value. Without acquire/release pairing, the consumer might see `ready == true` but stale `data`.

### `seq_cst` — the default, the safe choice

If you can't reason about acquire/release pairings, leave the default `seq_cst`. It's slower on weakly-ordered CPUs (ARM, POWER) but correct by construction. Every read and write participates in a single global total order.

> **Beginner advice:** start with the default `seq_cst` everywhere. Only relax to `acquire`/`release`/`relaxed` once you have benchmarks proving it matters and a clear argument for why the weaker ordering is sufficient.

## 6.5. compare_exchange and the ABA Problem

**Compare-and-swap (CAS)** is the building block of lock-free algorithms: "if the value is still what I expected, replace it with this new value, atomically — otherwise tell me what's there now."

```cpp
std::atomic<int> x{10};
int expected = 10;
bool ok = x.compare_exchange_strong(expected, 20);
// if x was 10 → x becomes 20, ok = true,  expected unchanged
// if x was  7 → x unchanged,  ok = false, expected now equals 7
```

Typical usage is a retry loop. `compare_exchange_weak` is the loop-form variant — cheaper, but allowed to fail spuriously (returns `false` even when the comparison would have succeeded), so you must put it in a loop:

```cpp
std::atomic<int> x{0};

void increment_doubled() {
    int old = x.load();
    int next;
    do {
        next = old * 2 + 1;
    } while (!x.compare_exchange_weak(old, next));   // retries with refreshed `old`
}
```

Use `compare_exchange_strong` when you can't loop (e.g. one-shot publication).

### The ABA Problem

CAS only checks that the *value* matches. It can't tell that the value changed and changed *back* while you weren't looking.

> 1. Thread A reads pointer `p`, sees value `X`.
> 2. Thread A is preempted.
> 3. Thread B pops `X` from a list, frees it, allocates a new node — the allocator gives back the same address `X` — and pushes it back.
> 4. Thread A resumes, does `compare_exchange(p, X, …)` — succeeds, even though the structure underneath has completely changed.

Mitigations:

- **Tagged pointers / version counters.** Pack a counter into spare bits and bump it on every update — now `(X, 5)` ≠ `(X, 7)`.
- **Hazard pointers** or **RCU.** Defer reclamation until no thread is observing the old value.
- **`std::atomic<std::shared_ptr<T>>`** (C++20). Reference counting sidesteps ABA for many node-based structures.

Lock-free programming is genuinely hard. The right level for most code is "mutexes by default, atomics for counters and flags, and read a book before writing a lock-free queue." Recommended reading: *C++ Concurrency in Action* by Anthony Williams.

For when you've decided you really do need a lock-free queue or ring buffer, see **[lock_free_data_structures.md](system_design/lock_free_data_structures.md)** — covers SPSC ring buffer (the simple correct one), SPSC/MPSC unbounded queues, the SPMC and MPMC escalation, and reclamation strategies (hazard pointers, RCU, atomic `shared_ptr`).

---

# 7. Designing Thread-Safe Classes

A class is **thread-safe** if it behaves correctly when called concurrently from multiple threads, with no extra synchronization required from the caller.

Adding a mutex to every method gets you *individually* atomic methods, but not necessarily a thread-safe *interface*. The trap:

## 7.1. Combining Read-Then-Modify Operations

A naïve thread-safe stack:

```cpp
template <typename T>
class Stack {
public:
    T    top()           { std::scoped_lock l(mu_); return data_.back(); }
    void pop()           { std::scoped_lock l(mu_); data_.pop_back(); }
    void push(T element) { std::scoped_lock l(mu_); data_.push_back(std::move(element)); }
private:
    std::mutex     mu_;
    std::vector<T> data_;
};
```

Every method is internally atomic — and the class is still broken. Consider:

| Thread 1 | Thread 2 |
|---|---|
| `T a = s.top();` *(reads 6)* |  |
|  | `T b = s.top();` *(also reads 6)* |
| `s.pop();` *(removes 6)* |  |
|  | `s.pop();` *(removes 4 — wrong!)* |

Both threads observed `top()` returning 6, but only one of those 6's is real. The other thread silently popped a different value.

The fix is to **redesign the API so the dangerous interleaving is impossible** — combine the read and the modification into one atomic operation:

```cpp
template <typename T>
class Stack {
public:
    // Pops and returns. std::optional handles "empty" without throwing.
    std::optional<T> pop() {
        std::scoped_lock l(mu_);
        if (data_.empty()) return std::nullopt;
        T value = std::move(data_.back());
        data_.pop_back();
        return value;
    }

    void push(T element) {
        std::scoped_lock l(mu_);
        data_.push_back(std::move(element));
    }
private:
    std::mutex     mu_;
    std::vector<T> data_;
};
```

Notice what disappeared: `top()`. There's no safe way to expose it on a concurrent stack — by the time the caller acts on the returned reference, another thread may have popped it. The only thread-safe peek is "peek and remove together."

Lessons that generalize:

- **Per-method locking ≠ thread safety.** Composed operations need their own atomic step.
- **Avoid returning references or iterators into shared state.** They become dangling the moment the lock is released.
- **Move work *into* the class.** Instead of `if (!q.empty()) q.front();`, expose a `try_pop(T&)` or `pop() -> optional<T>`.
- **Document what's atomic.** "All public methods are individually atomic" vs "Sequences of public method calls are atomic" are very different guarantees.

Full example: [thread_safe.cpp](../src/multithreading/thread_safe.cpp).

Refs: [SO: how to make an application thread-safe](https://stackoverflow.com/questions/5125241/how-to-make-an-application-thread-safe), [Herb Sutter — Lock-Free Programming I](https://www.youtube.com/watch?v=c1gO9aB9nbs), [Herb Sutter — Lock-Free Programming II](https://www.youtube.com/watch?v=CmxkPChOcvw).
