# C++20 Coroutines

A **coroutine** is a function that can pause itself in the middle, hand control back to its caller, and resume later from exactly where it left off — with all its local variables still alive. Regular functions can only `return` once and lose all their state; coroutines can suspend an arbitrary number of times.

That single capability covers a surprising range of use cases:

- **Generators** — produce a sequence one value at a time without building the whole thing in memory.
- **Async I/O** — write `auto data = co_await read(socket);` instead of nested callbacks or `.then(...)` chains.
- **Cooperative tasks** — fibers, state machines, parsers driven by input chunks.

The common thread: control flow that *naturally* wants to pause.

For a higher-level intro to async in C++ see [asynchronous_programming.md](asynchronous_programming.md). For where coroutines fit among the other "callable" things in C++ see [callable_callbacks.md](callable_callbacks.md).

---

## The three keywords

A function becomes a coroutine the moment its body uses any of these:

| Keyword       | Meaning                                                          |
|---------------|------------------------------------------------------------------|
| `co_await e`  | Suspend until `e` is ready, then continue with its result.       |
| `co_yield v`  | Suspend and emit `v` to the caller (used by generators).         |
| `co_return v` | Finish the coroutine, optionally producing a final value.        |

```cpp
MyTask example() {
  std::cout << "before\n";
  co_await some_awaitable;     // <-- this turns example() into a coroutine
  std::cout << "after\n";
  co_return;
}
```

The compiler sees `co_await` and rewrites the function: the locals get moved into a heap-allocated frame, each suspension point becomes a state in a state machine, and the function returns a *handle-shaped object* (`MyTask` above) to the caller immediately, even before the body runs to completion.

---

## The promise / handle / awaitable trio

To author your own coroutine return type, you need to understand three cooperating pieces. (You'll rarely write all three from scratch — a library will. But knowing the names helps when you read the docs.)

### 1. The promise type

For every coroutine return type `T`, the compiler looks up `std::coroutine_traits<T>::promise_type` and constructs one inside the coroutine frame. The promise tells the compiler *how the coroutine behaves at its boundaries*:

```cpp
struct promise_type {
  T get_return_object();              // builds the T returned to the caller
  std::suspend_always initial_suspend();  // suspend before running, or run eagerly?
  std::suspend_always final_suspend() noexcept;  // what to do at co_return
  void return_void();                 // (or return_value(...) if you co_return a value)
  void unhandled_exception();         // called if an exception escapes the body
};
```

`std::suspend_always` and `std::suspend_never` are the two trivial awaitables baked into the standard library — pick one to choose lazy vs eager startup.

### 2. The coroutine handle

`std::coroutine_handle<promise_type>` is a non-owning pointer-like type that lets you control a paused coroutine from outside it:

```cpp
h.resume();   // continue execution from the last suspension point
h.done();     // has it run past its final_suspend?
h.destroy();  // free the coroutine frame
h.promise();  // access the promise object inside the frame
```

### 3. Awaitables and awaiters

`co_await expr` doesn't operate directly on `expr`; it goes through an **awaiter** with three hooks:

```cpp
bool await_ready();                                // if true, skip suspension
void await_suspend(std::coroutine_handle<> h);     // arrange resumption later
T    await_resume();                               // value of the co_await expression
```

If `await_ready()` returns false, the coroutine suspends and `await_suspend(h)` is called with the handle. The awaiter is responsible for calling `h.resume()` *eventually* — typically when an I/O completion fires, a timer expires, etc. When resumed, `await_resume()` runs and its return value becomes the result of the `co_await` expression.

That's the entire surface area. Everything else — generators, tasks, `when_all`, channels — is built on top.

---

## A minimal generator (the Fibonacci demo)

This is the canonical "see, coroutines aren't scary" example. We'll build a barebones `Generator<T>` that lets us write Fibonacci as a single linear function:

```cpp
#include <coroutine>
#include <iostream>
#include <optional>

template <class T>
struct Generator {
  struct promise_type {
    T current;

    Generator get_return_object() {
      return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
    }
    std::suspend_always initial_suspend() noexcept { return {}; }
    std::suspend_always final_suspend()   noexcept { return {}; }
    std::suspend_always yield_value(T v)  noexcept { current = v; return {}; }
    void return_void()           noexcept {}
    void unhandled_exception()           { std::terminate(); }
  };

  std::coroutine_handle<promise_type> h;
  explicit Generator(std::coroutine_handle<promise_type> handle) : h(handle) {}
  ~Generator() { if (h) h.destroy(); }
  Generator(const Generator&) = delete;
  Generator(Generator&& o) noexcept : h(std::exchange(o.h, {})) {}

  std::optional<T> next() {
    h.resume();
    if (h.done()) return std::nullopt;
    return h.promise().current;
  }
};

Generator<long long> fib() {
  long long a = 0, b = 1;
  while (true) {
    co_yield a;
    auto next = a + b;
    a = b;
    b = next;
  }
}

int main() {
  auto g = fib();
  for (int i = 0; i < 10; ++i)
    std::cout << *g.next() << ' ';      // 0 1 1 2 3 5 8 13 21 34
}
```

Notice what `fib()` *isn't*: it isn't a class, it doesn't store its iteration state in members, and it doesn't have a `next()` method. The state — `a`, `b`, the `while` loop position — lives implicitly in the suspended coroutine frame. The function reads top-to-bottom like ordinary code.

> **C++23 update:** The standard library now ships [`std::generator<T>`](https://en.cppreference.com/w/cpp/coroutine/generator), which does exactly the above (plus iterators, `ranges` integration, and recursive `co_yield ranges::elements_of(...)`). Use it instead of rolling your own when you have a C++23 compiler.

---

## A minimal task / awaitable

Now let's see the *async* side: a coroutine that suspends until some external event fires. We'll model "wait for a value to be set by another thread" with an awaiter object:

```cpp
#include <coroutine>
#include <iostream>
#include <thread>
#include <chrono>

struct AsyncInt {
  int value{};
  std::coroutine_handle<> waiter{};

  // The awaiter the coroutine sees when it does `co_await async_int`.
  bool await_ready() const noexcept { return false; }   // always suspend
  void await_suspend(std::coroutine_handle<> h) noexcept { waiter = h; }
  int  await_resume() const noexcept { return value; }

  // Producer side: called from another thread when the value is ready.
  void set(int v) {
    value = v;
    if (waiter) waiter.resume();
  }
};
```

A trivial `Task` return type to host the coroutine (eager start, fire-and-forget):

```cpp
struct Task {
  struct promise_type {
    Task get_return_object() { return {}; }
    std::suspend_never initial_suspend() noexcept { return {}; }
    std::suspend_never final_suspend()   noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() { std::terminate(); }
  };
};

Task consumer(AsyncInt& a) {
  std::cout << "before await\n";
  int v = co_await a;                   // suspends here
  std::cout << "got " << v << '\n';
}

int main() {
  AsyncInt a;
  consumer(a);                          // runs up to co_await, then suspends
  std::cout << "main does other work\n";
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  a.set(42);                            // resumes the coroutine on this thread
}
```

Output:

```
before await
main does other work
got 42
```

The body of `consumer` reads as if it were synchronous; the suspension is invisible at the call site. That's the whole point.

In real systems the producer wouldn't be `main` — it would be an I/O completion port, a timer wheel, or a thread pool that picks the resumption back up. The shape of the awaiter stays the same.

---

## Library wrappers (use these in real code)

Hand-rolling `promise_type` is verbose, easy to get wrong, and most of the boilerplate is the same every time. In production you'll reach for one of:

| Library                | What it gives you                                                  | Status                |
|------------------------|--------------------------------------------------------------------|-----------------------|
| **`std::generator`**   | Standard generator with iterator + ranges integration              | C++23                 |
| **`std::execution`**   | Senders/receivers — structured async with composable algorithms    | C++26                 |
| **cppcoro**            | `task<T>`, `generator<T>`, `async_mutex`, `when_all`, schedulers   | Library, header-only  |
| **libunifex**          | The original sender/receiver design (precursor to `std::execution`)| Library, Meta/MSFT    |
| **Boost.Cobalt**       | `task`, `promise`, `generator`, `channel` — integrates with Asio   | Boost ≥ 1.84          |
| **Boost.Asio**         | Existing async API now accepts `co_await` via `use_awaitable`      | Boost ≥ 1.75          |

Sketch with cppcoro:

```cpp
cppcoro::task<int> read_then_double(socket& s) {
  auto bytes = co_await s.async_read();    // suspends until the read completes
  co_return parse(bytes) * 2;
}
```

Sketch with Asio:

```cpp
boost::asio::awaitable<void> session(tcp::socket s) {
  std::array<char, 1024> buf;
  std::size_t n = co_await s.async_read_some(boost::asio::buffer(buf),
                                              boost::asio::use_awaitable);
  co_await boost::asio::async_write(s, boost::asio::buffer(buf, n),
                                     boost::asio::use_awaitable);
}
```

These read like blocking code while running on a single thread, multiplexed over thousands of connections.

---

## Caveats and rules of thumb

- **Don't author `promise_type` by hand unless you must.** The error messages are obscure, the lifetime rules around `final_suspend` are subtle, and exceptions in coroutines have their own (sharp) edges.
- **Coroutine frames are heap-allocated by default.** The compiler can elide the allocation (HALO — Heap Allocation eLision Optimization) when the lifetime is fully visible, but don't count on it in hot loops.
- **Returning references to locals from a coroutine is a classic UB trap** — the locals live in the frame and disappear when it's destroyed, but a dangling `co_yield`'d reference can outlive its referent if you're not careful.
- **There's no `std::task<T>` in C++20.** That's the most-asked-for missing piece; users either roll their own, use cppcoro, or wait for `std::execution` in C++26.
- **Coroutines don't *create* concurrency.** They suspend and resume on whatever thread the executor chooses. You still need a runtime (Asio, a thread pool, an event loop) to actually make multiple things progress in parallel.

---

## See also

- [callable_callbacks.md §9](callable_callbacks.md) — coroutines as one of C++'s "callable" things.
- [asynchronous_programming.md](asynchronous_programming.md) — `std::async`, `std::future`, threads — the pre-coroutine async toolbox.
- [cppreference: Coroutines](https://en.cppreference.com/w/cpp/language/coroutines)
- [Lewis Baker, "C++ Coroutines: Understanding the promise type"](https://lewissbaker.github.io/2018/09/05/understanding-the-promise-type) — still the best deep-dive on the mechanism.
- [`std::generator` (C++23)](https://en.cppreference.com/w/cpp/coroutine/generator)
- [`std::execution` (C++26 senders/receivers)](https://en.cppreference.com/w/cpp/execution)
