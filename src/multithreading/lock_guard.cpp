// std::lock_guard (C++11) is the legacy single-mutex RAII wrapper.
// Modern code should prefer std::scoped_lock — see scoped_lock.cpp and
// docs/multithreading.md §4.2.5. Kept here because you'll still see
// lock_guard in pre-C++17 codebases.

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

int counter = 0;
std::mutex mu;

void increment_n(int n) {
  for (int i = 0; i < n; ++i) {
    std::lock_guard<std::mutex> lock(mu); // single mutex; cannot take more
    ++counter;
  }
}

int main() {
  {
    std::vector<std::jthread> threads;
    for (int i = 0; i < 4; ++i)
      threads.emplace_back(increment_n, 1000);
  }
  std::cout << "counter = " << counter << " (expected 4000)\n";
}
