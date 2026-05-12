// Demonstrates §5 of docs/multithreading.md: futures with the three
// producer-side options — std::async, std::promise, std::shared_future.

#include <future>
#include <iostream>
#include <stdexcept>

long factorial(int n) {
  long v = 1;
  for (int i = 1; i <= n; ++i)
    v *= i;
  return v;
}

// Wait for an input handed in via promise → future, then compute.
long factorial_from_future(std::future<int> in) {
  int n = in.get(); // blocks until promise is fulfilled
  return factorial(n);
}

int main() {
  // 5.1 — std::async with an explicit launch policy.
  // The default policy can fall back to "deferred" on the calling thread;
  // always pass std::launch::async unless you have a reason not to.
  {
    auto fut = std::async(std::launch::async, factorial, 10);
    std::cout << "10! = " << fut.get() << '\n';
  }

  // 5.3 — std::promise: hand a value to the worker after it has started.
  {
    std::promise<int> input;
    auto fut =
        std::async(std::launch::async, factorial_from_future, input.get_future());
    input.set_value(8); // can also: set_exception(std::make_exception_ptr(...))
    std::cout << "8! = " << fut.get() << '\n';
  }

  // 5.3 — promise carrying an exception.
  {
    std::promise<int> p;
    auto fut = p.get_future();
    p.set_exception(std::make_exception_ptr(std::runtime_error("bad input")));
    try {
      (void)fut.get();
    } catch (const std::exception &e) {
      std::cout << "rethrown via future.get(): " << e.what() << '\n';
    }
  }

  // 5.4 — shared_future: many consumers read the same one-time value.
  {
    std::promise<int> input;
    std::shared_future<int> shared = input.get_future().share();

    auto a = std::async(std::launch::async, [shared] { return shared.get() + 1; });
    auto b = std::async(std::launch::async, [shared] { return shared.get() * 2; });

    input.set_value(10);
    std::cout << "shared_future: a = " << a.get() << ", b = " << b.get() << '\n';
  }
}
