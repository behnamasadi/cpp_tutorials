#include <algorithm>
#include <cmath>
#include <condition_variable>
#include <exception>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <stack>
#include <thread>

std::mutex mu;
std::condition_variable cv;
bool done = false;

void worker() {
  for (int i = 0; i < 5; ++i)
    std::this_thread::sleep_for(
        std::chrono::milliseconds(200)); // pretend to work
  {
    std::scoped_lock lock(mu);
    done = true;
  }
  cv.notify_one(); // wake main
}

int main() {
  std::jthread t(worker);
  std::unique_lock lock(mu);
  cv.wait(lock, [] { return done; });
  std::cout << "worker finished\n";
}