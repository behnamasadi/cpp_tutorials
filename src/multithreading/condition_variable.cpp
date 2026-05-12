// Demonstrates §4.4 of docs/multithreading.md: worker notifies main using a
// condition variable, with the inner-scope notify pattern that avoids the
// wake-bounce.

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex mu;
std::condition_variable cv;
bool done = false;

void worker() {
  // Pretend to do work.
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  // Inner scope: release the lock *before* notify_one, so the woken
  // waiter can take the lock immediately (avoids the wake-bounce).
  {
    std::scoped_lock lock(mu);
    done = true;
  }
  cv.notify_one();
}

int main() {
  std::jthread t(worker);

  std::unique_lock<std::mutex> lock(mu);
  cv.wait(lock, [] { return done; }); // predicate form: re-checks on every wake
  std::cout << "worker finished, done = " << std::boolalpha << done << '\n';
}
