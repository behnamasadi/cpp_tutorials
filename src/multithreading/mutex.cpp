// Demonstrates §4.2 of docs/multithreading.md: protecting shared state
// with a mutex. The Wallet from race_condition.cpp, fixed.

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class Wallet {
public:
  void deposit(int amount) {
    std::scoped_lock lock(mu_);
    balance_ += amount;
  }
  int balance() {
    std::scoped_lock lock(mu_);
    return balance_;
  }

private:
  std::mutex mu_;
  int balance_ = 0;
};

int main() {
  Wallet w;
  {
    std::vector<std::jthread> threads;
    for (int i = 0; i < 5; ++i)
      threads.emplace_back([&] {
        for (int j = 0; j < 1000; ++j)
          w.deposit(1);
      });
  } // jthreads join here
  std::cout << "balance = " << w.balance() << " (expected 5000)\n";
}
