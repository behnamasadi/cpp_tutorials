// Demonstrates §3.4 of docs/multithreading.md: sleep_for / sleep_until
// with <chrono> — typed durations beat C-style usleep().

#include <chrono>
#include <iostream>
#include <thread>

int main() {
  using namespace std::chrono_literals;

  std::cout << "sleep_for 200ms...\n";
  std::this_thread::sleep_for(200ms);

  std::cout << "sleep_for 1s via std::chrono::seconds...\n";
  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::cout << "sleep_until now + 300ms (steady clock)...\n";
  auto wake_at = std::chrono::steady_clock::now() + 300ms;
  std::this_thread::sleep_until(wake_at);

  std::cout << "done\n";
}
