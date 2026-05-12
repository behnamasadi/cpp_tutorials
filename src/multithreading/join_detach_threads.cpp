// Demonstrates §2.7–2.8 of docs/multithreading.md: the older std::thread API
// where the caller is responsible for join() or detach(). New code should
// prefer std::jthread (see creating_and_terminating_threads.cpp).

#include <chrono>
#include <iostream>
#include <syncstream>
#include <thread>
#include <vector>

void process_frame(int id) {
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  std::osyncstream(std::cout) << "  frame " << id << " done\n";
}

// Fleet pattern: spawn N workers, wait for all of them.
void joining_demo() {
  std::cout << "joining_demo:\n";
  std::vector<std::thread> workers;
  for (int i = 0; i < 4; ++i)
    workers.emplace_back(process_frame, i);

  for (auto &t : workers)
    t.join(); // forgetting either join() or detach() -> std::terminate()
}

// Detached threads run on their own. The runtime cleans them up when they
// finish — but if main exits first, they're killed mid-operation.
void detaching_demo() {
  std::cout << "detaching_demo:\n";
  std::thread t([] {
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    std::cout << "  detached thread finished\n";
  });
  t.detach(); // gives up ownership; cannot join() afterwards.

  // Give the detached thread a moment to finish so we actually see its output.
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// joinable() guards against double-join / join-after-detach (which would
// both call std::terminate()).
void joinable_guard_demo() {
  std::cout << "joinable_guard_demo:\n";
  std::thread t(process_frame, 99);
  if (t.joinable())
    t.join();
  if (t.joinable()) // false now — already joined.
    t.join();
  std::cout << "  joinable() correctly returned false the second time\n";
}

int main() {
  joining_demo();
  detaching_demo();
  joinable_guard_demo();
}
