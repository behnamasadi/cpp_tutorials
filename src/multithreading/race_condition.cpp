// Demonstrates §4.1.2 of docs/multithreading.md: the lost-update race.
// Compiled normally this is undefined behavior; the value will typically be
// less than the expected total. The fix is shown in mutex.cpp.

#include <iostream>
#include <thread>
#include <vector>

class Wallet {
public:
  int balance = 0;
  // Not atomic: read -> add -> write is three steps, not one.
  void deposit(int amount) { balance = balance + amount; }
};

constexpr int kThreads = 8;
constexpr int kDepositsPerThread = 100'000;
constexpr int kExpected = kThreads * kDepositsPerThread;

int run_one_trial() {
  Wallet w;
  {
    std::vector<std::jthread> threads;
    for (int i = 0; i < kThreads; ++i)
      threads.emplace_back([&] {
        for (int j = 0; j < kDepositsPerThread; ++j)
          w.deposit(1);
      });
  } // jthreads join here
  return w.balance;
}

int main() {
  int wrong = 0;
  for (int trial = 0; trial < 10; ++trial) {
    int balance = run_one_trial();
    if (balance != kExpected) {
      std::cout << "trial " << trial << ": balance = " << balance
                << " (lost " << kExpected - balance << " updates)\n";
      ++wrong;
    }
  }
  std::cout << wrong << " / 10 trials had lost updates\n";
}
