// Demonstrates §4.2.3-§4.2.4 of docs/multithreading.md: how lock ordering
// causes deadlock and how std::scoped_lock fixes it.
//
// Note: dead_lock_demo() is intentionally commented out in main() — it
// hangs forever, which is exactly the point. Uncomment to confirm, then
// kill the process with Ctrl+C.

#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex mu1, mu2;

// Thread A locks mu1 first, Thread B locks mu2 first → classic deadlock.
void func_a_unsafe() {
  std::scoped_lock l1(mu1);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  std::scoped_lock l2(mu2); // blocks if B already holds mu2
  std::cout << "func_a_unsafe done\n";
}
void func_b_unsafe() {
  std::scoped_lock l1(mu2);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  std::scoped_lock l2(mu1); // blocks if A already holds mu1
  std::cout << "func_b_unsafe done\n";
}

[[maybe_unused]] void dead_lock_demo() {
  std::jthread a(func_a_unsafe);
  std::jthread b(func_b_unsafe); // ⚠ destructor never returns
}

// Fix: one scoped_lock that takes both — order at the call site no longer
// matters. The std::lock algorithm inside scoped_lock avoids deadlock.
void func_a_safe() {
  std::scoped_lock lock(mu1, mu2);
  std::cout << "func_a_safe done\n";
}
void func_b_safe() {
  std::scoped_lock lock(mu2, mu1); // opposite order is fine
  std::cout << "func_b_safe done\n";
}

void dead_lock_solved_demo() {
  std::jthread a(func_a_safe);
  std::jthread b(func_b_safe);
}

int main() {
  // dead_lock_demo();      // uncomment to observe the hang
  dead_lock_solved_demo();
}
