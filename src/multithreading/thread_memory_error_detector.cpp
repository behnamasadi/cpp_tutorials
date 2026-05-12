// A deliberate data race for sanitizers to catch. Build with TSan and the
// race on `frames_received` is reported:
//
//   g++ -std=c++20 -O1 -g -fsanitize=thread \
//       src/multithreading/thread_memory_error_detector.cpp -o tsan_demo
//   ./tsan_demo
//
// See docs/sanitizers.md §2 (ThreadSanitizer) and docs/atomic.md for the
// fix — make the counter std::atomic<int> or protect it with a mutex.

#include <iostream>
#include <thread>

int frames_received = 0; // shared plain int — UNSAFE on purpose

int main() {
  auto work = [] {
    for (int i = 0; i < 1'000'000; ++i)
      ++frames_received; // load -> add -> store, not atomic
  };

  {
    std::jthread a(work);
    std::jthread b(work);
  } // joined here

  // Expected: 2'000'000. Actual (without sanitizer): some smaller, non-deterministic number.
  std::cout << "frames_received = " << frames_received
            << " (expected 2000000)\n";
}
