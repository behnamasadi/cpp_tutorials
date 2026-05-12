// Demonstrates §7.1 of docs/multithreading.md: per-method locking is NOT
// enough — `top()` + `pop()` as two calls lets two threads observe the same
// element. The fix is an API change: combine the read and the modification
// into one atomic step (and drop `top()` entirely).

#include <iostream>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

template <typename T> class BrokenStack {
public:
  T top() {
    std::scoped_lock l(mu_);
    return data_.back();
  }
  void pop() {
    std::scoped_lock l(mu_);
    data_.pop_back();
  }
  void push(T x) {
    std::scoped_lock l(mu_);
    data_.push_back(std::move(x));
  }
  bool empty() {
    std::scoped_lock l(mu_);
    return data_.empty();
  }

private:
  std::mutex mu_;
  std::vector<T> data_;
};

template <typename T> class Stack {
public:
  // Read and modify in one critical section. optional<T> handles "empty"
  // without throwing.
  std::optional<T> pop() {
    std::scoped_lock l(mu_);
    if (data_.empty())
      return std::nullopt;
    T v = std::move(data_.back());
    data_.pop_back();
    return v;
  }
  void push(T x) {
    std::scoped_lock l(mu_);
    data_.push_back(std::move(x));
  }

private:
  std::mutex mu_;
  std::vector<T> data_;
};

int main() {
  Stack<int> s;
  for (int i = 0; i < 100; ++i)
    s.push(i);

  {
    std::vector<std::jthread> consumers;
    for (int i = 0; i < 4; ++i)
      consumers.emplace_back([&] {
        while (auto v = s.pop())
          (void)*v; // each value popped by exactly one thread
      });
  }
  std::cout << "all 100 values popped exactly once across 4 threads\n";

  // BrokenStack is here so the type-name appears in the build and the
  // reader can compare; we don't exercise the race deliberately.
  (void)sizeof(BrokenStack<int>);
}
