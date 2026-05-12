// Demonstrates §4.2.6 of docs/multithreading.md: when unique_lock buys you
// something scoped_lock can't — deferred locking, unlock/relock partway,
// and (the most common case) condition_variable::wait, which requires it.

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex mu;
std::condition_variable cv;
bool ready = false;

// 1. Deferred locking + unlock-relock within one scope.
void deferred_demo() {
  std::unique_lock<std::mutex> lock(mu, std::defer_lock); // not yet locked

  // ... preparation that doesn't need the lock ...

  lock.lock();
  std::cout << "  deferred: now under the lock\n";
  lock.unlock();

  // ... more lock-free work ...

  lock.lock();
  std::cout << "  deferred: re-acquired the lock\n";
} // unlocks on destruction if still held

// 2. condition_variable::wait *requires* unique_lock<mutex> because it has
// to release and re-acquire the mutex around the sleep.
void cv_demo() {
  std::jthread worker([] {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    {
      std::scoped_lock lock(mu);
      ready = true;
    }
    cv.notify_one();
  });

  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [] { return ready; }); // predicate form handles spurious wakeups
  std::cout << "  cv: woke up, ready = " << std::boolalpha << ready << '\n';
}

int main() {
  deferred_demo();
  cv_demo();
}
