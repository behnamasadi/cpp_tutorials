// A minimal fixed-size thread pool built from the primitives in
// docs/multithreading.md: jthread workers, a mutex-protected job queue,
// a condition_variable for sleeping when idle, and packaged_task so each
// submitted job hands back a future.

#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
  explicit ThreadPool(unsigned n) {
    for (unsigned i = 0; i < n; ++i)
      workers_.emplace_back([this](std::stop_token stoken) { run(stoken); });
  }

  ~ThreadPool() {
    // Ask workers to stop, then wake them so they observe the stop request.
    for (auto &w : workers_)
      w.request_stop();
    cv_.notify_all();
  }

  // Submit a callable; receive its eventual return value as a future.
  template <typename F> auto submit(F f) -> std::future<decltype(f())> {
    using R = decltype(f());
    auto task = std::make_shared<std::packaged_task<R()>>(std::move(f));
    std::future<R> fut = task->get_future();
    {
      std::scoped_lock lock(mu_);
      jobs_.emplace([task] { (*task)(); });
    }
    cv_.notify_one();
    return fut;
  }

private:
  void run(std::stop_token stoken) {
    while (true) {
      std::function<void()> job;
      {
        std::unique_lock lock(mu_);
        cv_.wait(lock, [&] { return stoken.stop_requested() || !jobs_.empty(); });
        if (jobs_.empty()) // stop requested with nothing left to do
          return;
        job = std::move(jobs_.front());
        jobs_.pop();
      }
      job();
    }
  }

  std::vector<std::jthread> workers_;
  std::queue<std::function<void()>> jobs_;
  std::mutex mu_;
  std::condition_variable cv_;
};

int main() {
  ThreadPool pool(4);

  std::vector<std::future<int>> results;
  for (int i = 0; i < 8; ++i)
    results.push_back(pool.submit([i] { return i * i; }));

  for (auto &f : results)
    std::cout << f.get() << ' ';
  std::cout << '\n';
}
