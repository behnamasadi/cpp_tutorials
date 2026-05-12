// Demonstrates §5.2 of docs/multithreading.md: packaged_task wraps a
// callable so its return value lands in a future. Useful when you want
// to control *where* the task runs — a worker thread, a thread pool,
// a task queue — rather than letting std::async pick.

#include <future>
#include <iostream>
#include <thread>

long factorial(int n) {
  long v = 1;
  for (int i = 1; i <= n; ++i)
    v *= i;
  return v;
}

int main() {
  // Wrap factorial in a task; get the future side before moving the task.
  std::packaged_task<long(int)> task(factorial);
  std::future<long> result = task.get_future();

  // Run the task on a worker thread we own.
  std::thread t(std::move(task), 10);
  t.join();

  std::cout << "10! = " << result.get() << '\n';
}
