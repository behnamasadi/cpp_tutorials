// C++20 coroutines: a minimal generator and a minimal task/awaiter.
//
// Companion to docs/coroutines.md. We hand-roll the two return types
// (Generator<T> and Task) to keep the boilerplate explicit. In real code
// you would use std::generator (C++23) or a library like cppcoro.

#include <coroutine>
#include <iostream>
#include <optional>
#include <utility>

// ----------------------------------------------------------------------------
// Demo 1: a barebones generator
// ----------------------------------------------------------------------------
template <class T>
struct Generator {
    struct promise_type {
        T current{};

        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(T v) noexcept {
            current = v;
            return {};
        }
        void return_void() noexcept {}
        void unhandled_exception() { std::terminate(); }
    };

    std::coroutine_handle<promise_type> h;

    explicit Generator(std::coroutine_handle<promise_type> handle) : h(handle) {}
    ~Generator() {
        if (h)
            h.destroy();
    }
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;
    Generator(Generator&& o) noexcept : h(std::exchange(o.h, {})) {}

    // Resume the coroutine; returns std::nullopt once it runs past final_suspend.
    std::optional<T> next() {
        h.resume();
        if (h.done())
            return std::nullopt;
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

// ----------------------------------------------------------------------------
// Demo 2: a trivial Task + awaiter (no threading, no scheduler)
//
// The coroutine `consumer` suspends at `co_await a`. main() later calls
// a.set(42), which resumes the coroutine right there on main's thread.
// This is the simplest possible illustration of suspend / resume.
// ----------------------------------------------------------------------------
struct Task {
    struct promise_type {
        Task get_return_object() { return {}; }
        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };
};

struct AsyncInt {
    int value{};
    std::coroutine_handle<> waiter{};

    // Awaiter interface: what the coroutine sees on `co_await async_int`.
    bool await_ready() const noexcept { return false; } // always suspend
    void await_suspend(std::coroutine_handle<> h) noexcept { waiter = h; }
    int await_resume() const noexcept { return value; }

    // Producer side: deliver a value and resume the waiting coroutine.
    void set(int v) {
        value = v;
        if (waiter)
            waiter.resume();
    }
};

Task consumer(AsyncInt& a) {
    std::cout << "  consumer: before await\n";
    int v = co_await a; // suspends here until a.set(...) is called
    std::cout << "  consumer: got " << v << '\n';
}

// ----------------------------------------------------------------------------
int main() {
    std::cout << "===== Demo 1: Generator<long long> (Fibonacci) =====\n";
    auto g = fib();
    for (int i = 0; i < 10; ++i)
        std::cout << *g.next() << ' ';
    std::cout << '\n';

    std::cout << "\n===== Demo 2: Task + awaiter (co_await suspends, set() resumes) =====\n";
    AsyncInt a;
    consumer(a); // runs up to co_await, then suspends
    std::cout << "  main: doing other work...\n";
    a.set(42); // resumes the coroutine on this thread

    return 0;
}
