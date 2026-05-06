# `std::atomic<T>` — When and Why

A simple, focused walkthrough of why `std::atomic` exists. For memory ordering, compare-and-swap, the ABA problem, and lock-free data structures, see [multithreading.md §6](multithreading.md#6-stdatomic).

---

## 1. The motivating problem

Imagine a robot has a single counter — total CAN frames received across all worker threads. Two workers each receive and count a million frames. The expected total is 2,000,000:

```cpp
#include <iostream>
#include <thread>

int frames_received = 0;     // shared plain int — no synchronization

int main() {
    auto work = [&] {
        for (int i = 0; i < 1'000'000; ++i)
            ++frames_received;
    };

    {
        std::jthread a(work);
        std::jthread b(work);
    }                                     // both threads joined here

    std::cout << frames_received << '\n';
}
```

Run it a few times and you'll get something like:

```text
1098330
1261226
1617594
1188436
```

Every run is different, and every run is wrong. The counter is randomly losing about a third of the increments.

## 2. Why it goes wrong

`++frames_received` looks like one operation but the CPU executes it as three:

1. **Load** the current value of `frames_received` into a register.
2. **Add 1** to that register.
3. **Store** the register back into `frames_received`.

Two threads can interleave these three steps. Worst case:

| Step | Thread A         | Thread B         | `frames_received` |
| ---- | ---------------- | ---------------- | ----------------- |
| 1    | load → 5         |                  | 5                 |
| 2    |                  | load → 5         | 5                 |
| 3    | add → 6          |                  | 5                 |
| 4    |                  | add → 6          | 5                 |
| 5    | store 6          |                  | 6                 |
| 6    |                  | store 6          | 6                 |

Two increments happened, but `frames_received` only went up by 1. That's a **lost update**. With a million iterations on each side and millions of contention windows per second, thousands of updates get lost on every run — exactly what the random-looking numbers above show.

It's also worse than just "wrong number." Two threads writing the same plain `int` without synchronization is a **data race**, which the C++ standard says is **undefined behavior**. The compiler is free to do anything — today's "wrong number" can become tomorrow's silent corruption when the optimizer changes its mind.

## 3. The fix: `std::atomic<int>`

Wrap the counter in `std::atomic`. That's the only change:

```cpp
#include <iostream>
#include <atomic>
#include <thread>

std::atomic<int> frames_received{0};      // ← only line that changed

int main() {
    auto work = [&] {
        for (int i = 0; i < 1'000'000; ++i)
            ++frames_received;            // ← still looks the same
    };

    {
        std::jthread a(work);
        std::jthread b(work);
    }

    std::cout << frames_received << '\n'; // exactly 2000000, every run
}
```

`std::atomic<int>::operator++` is guaranteed **indivisible**. Under the hood the compiler emits a single hardware-atomic instruction (`lock xadd` on x86, `ldaxr` / `stlxr` loop on ARM) that bundles load-add-store into one uninterruptible step. No interleaving is possible, no updates are lost.

### A note on the `{0}` syntax

You'll notice the fix line uses braces — `std::atomic<int> frames_received{0};` — and not `= 0`. There's a real reason for it.

`std::atomic<T>` **deletes its copy operations**:

```cpp
atomic(const atomic&) = delete;
atomic& operator=(const atomic&) = delete;
```

Because there is no hardware "atomic copy" instruction. Copying an atomic would require a separate load and store, with a window between them where another thread could change the source — so the standard simply forbids it.

That makes anything that visually looks like assignment suspicious. Strictly speaking, in C++17+ this *does* compile:

```cpp
std::atomic<int> x = 0;     // works in C++17+, but only via a subtle language rule
```

`atomic` has a non-explicit converting constructor `atomic(T)`. The line above conceptually creates a temporary `atomic<int>` from `0` and then copy-constructs `x` from it — copy is deleted, so this would fail. But C++17's mandatory prvalue elision removes the copy step entirely, so the line ends up calling `atomic(T)` directly. It compiles, it works, but it relies on a subtle rule that didn't exist before C++17.

The brace form skips all that:

```cpp
std::atomic<int> shared{0};    // direct-list-initialization, calls atomic(T) directly
```

Reasons to prefer `{0}`:

- **No standard-version dependency.** Works on C++11, C++14, C++17, C++20.
- **Disallows narrowing.** `std::atomic<int>{1.5}` is a compile error (good — almost certainly a bug). `std::atomic<int> x = 1.5` silently truncates to `1`.
- **Always initializes.** `std::atomic<int> x;` (no initializer) was undefined behavior to read pre-C++20 — the underlying int was indeterminate. C++20 value-initializes to `0`, but being explicit removes any doubt.
- **Reads as construction, not assignment** — which matches what's actually happening.

A summary of the legal forms:

```cpp
std::atomic<int> a{0};      // ✔ preferred — direct-list-init
std::atomic<int> b(0);      // ✔ classic direct-init, equivalent
std::atomic<int> c = 0;     // ✔ compiles in C++17+ via copy elision
std::atomic<int> d;         // ⚠ pre-C++20: indeterminate value (UB to read)
                            //   C++20+: value-initialized to 0
std::atomic<int> e = a;     // ✗ compile error — copy constructor deleted
```

Default to `{value}` for atomics. The same advice applies to most types where copy semantics are constrained or surprising.

## 4. What `std::atomic<T>` gives you

For any built-in type `T` that's *trivially copyable* (and small enough to fit in a CPU word):

- **Atomic load/store.** `counter = 5` and `int v = counter` are each single uninterruptible operations.
- **Atomic read-modify-write.** `++`, `--`, `+=`, `-=`, `&=`, `|=`, `^=` on integer atomics. Each is one indivisible step.
- **Atomic exchange.** `counter.exchange(new_value)` swaps in a value and returns the old one, atomically.
- **Compare-and-swap.** `compare_exchange_strong` / `compare_exchange_weak` — the building block for lock-free data structures.
- **No mutex needed.** Roughly 10× faster than locking a `std::mutex` around a single-word counter.

Common atomic types in robotics code:

```cpp
std::atomic<int>     frames_received;     // telemetry counter
std::atomic<bool>    shutdown_requested;  // graceful-stop flag
std::atomic<size_t>  buffer_head;         // ring-buffer index
std::atomic<Pose*>   latest_pose;         // pointer to the most recent reading
```

## 5. Atomic vs mutex — when to choose which

Both `std::atomic<int>` and a mutex-protected `int` solve the data-race problem. For a single counter the choice isn't arbitrary — the two have very different cost and applicability profiles.

### The two solutions side by side

```cpp
// Atomic version
std::atomic<int> counter{0};
auto work = [&] {
    for (int i = 0; i < 1'000'000; ++i)
        ++counter;
};
```

```cpp
// Mutex version
int counter = 0;
std::mutex m;
auto work = [&] {
    for (int i = 0; i < 1'000'000; ++i) {
        std::scoped_lock lock(m);
        ++counter;
    }
};
```

### Performance

For a single-word counter, atomic is roughly **10× faster** than the mutex version:

| Operation                          | Cost (rough)                                                                         |
| ---------------------------------- | ------------------------------------------------------------------------------------ |
| `++counter` (atomic)               | ~5–10 ns — one hardware instruction (`lock xadd` on x86)                             |
| Acquire + release a `std::mutex`   | ~20–25 ns uncontended; much more under contention (futex syscall, scheduler involvement) |

The atomic increment is one indivisible CPU instruction. The mutex version does five operations for what should be one — acquire → load → add → store → release. Under contention the gap widens: losing threads can be descheduled by the kernel.

### Feature comparison

| Aspect                                       | `std::atomic<int>` | `std::mutex` + `scoped_lock` |
| -------------------------------------------- | ------------------ | ---------------------------- |
| Removes data race                            | yes                | yes                          |
| Speed (single word)                          | excellent          | mediocre — overkill          |
| Protects one variable                        | yes                | yes                          |
| Protects multiple variables together         | **no**             | yes                          |
| Protects compound state (vector, map, class) | **no**             | yes                          |
| Around I/O or callbacks                      | **no**             | yes                          |
| Pairs with `condition_variable`              | no                 | yes                          |
| Code looks like serial code                  | mostly yes         | no — `lock` is visible       |

### When the mutex *is* the right answer

Reach for a mutex (or `scoped_lock`) when:

- **Multiple variables must update together.** "Set the new pose **and** bump the version counter atomically." Two separate atomics can't do that — another thread can observe the gap between updates. A single mutex around both keeps them consistent.
- **The state isn't a primitive.** `std::vector`, `std::map`, your own classes — `std::atomic<MyClass>` either won't compile (not trivially copyable) or silently falls back to a hidden lock. Use a real mutex and the cost is at least visible.
- **The critical section does I/O or calls user code.** Logging, file writes, network, callbacks — atomics can't help.
- **You need to wait on a condition.** `std::condition_variable` requires a `unique_lock` paired with a mutex.

### When atomic *is* the right answer

Single-word, single-variable updates:

- **Counters** — frames received, samples processed, errors logged.
- **Flags** — `std::atomic<bool> shutdown_requested;`
- **Indices into a buffer** — `std::atomic<size_t> head;`
- **Latest pointer** — `std::atomic<Pose*> latest;`

If the shared state fits in one CPU word and you do one operation on it at a time, atomic is the right tool.

### Decision rule

> If you're protecting a single primitive variable, use `std::atomic`. As soon as the answer involves "and these other things have to update together," switch to a mutex.

A counter of "CAN frames received" is **exactly** what `std::atomic` was designed for. Reaching for a mutex here is like locking a whole room every time you flip a light switch — correct, but wasteful.

If you needed *both* "increment frame count" **and** "store the latest frame in a buffer" together, that's mutex territory.

## 6. Subtler limitations of `std::atomic`

The comparison above covers the common "use a mutex instead" cases. A few sharper edges that don't show up in a quick comparison:

- **Anything bigger than a CPU word silently locks.** `std::atomic<BigStruct>` is legal, but the implementation falls back to a hidden internal mutex. Check at compile time so you don't pay for it by accident:

  ```cpp
  static_assert(std::atomic<MyType>::is_always_lock_free);
  ```

- **A flag plus the data it protects** isn't enough by itself. "I set the flag, then read the data" needs **memory ordering** so the reader doesn't see the flag set before the data is published. That's where `memory_order_acquire` / `memory_order_release` come in. See [multithreading.md §6.4](multithreading.md#64-memory-ordering).

## 7. Where to go next

This doc covers the simplest case: a shared counter. Real lock-free programming gets more involved:

- **Memory ordering** — `memory_order_relaxed` / `acquire` / `release` / `seq_cst`. The default `seq_cst` is correct but often slower than necessary. See [multithreading.md §6.4](multithreading.md#64-memory-ordering).
- **Compare-and-swap and the ABA problem** — [multithreading.md §6.5](multithreading.md#65-compare_exchange-and-the-aba-problem).
- **Lock-free queues, stacks, and reclamation** — [system_design/lock_free_data_structures.md](system_design/lock_free_data_structures.md).

For most code, a counter and a flag are all you need from `<atomic>`. Reach for the deeper material only when a profiler tells you a mutex is the bottleneck.

---

Reference: [Atomic operations in C++ (talk)](https://www.youtube.com/watch?v=ZWjVa1kfE7c)
