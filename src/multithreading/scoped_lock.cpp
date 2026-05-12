// Demonstrates §4.2.2 of docs/multithreading.md: std::scoped_lock acquiring
// multiple mutexes atomically using a deadlock-avoidance algorithm.
// Naive nested locks here would deadlock if two threads transfer in opposite
// directions; scoped_lock(from.mu, to.mu) is safe regardless of caller order.

#include <iostream>
#include <mutex>
#include <thread>

struct Account {
  std::mutex mu;
  int balance;
};

void transfer(Account &from, Account &to, int amount) {
  std::scoped_lock lock(from.mu, to.mu); // both, atomically, no deadlock
  from.balance -= amount;
  to.balance += amount;
}

int main() {
  Account a{.balance = 1000};
  Account b{.balance = 1000};

  {
    // Two threads transfer in opposite directions repeatedly — the exact
    // shape that would deadlock with naive nested locks.
    std::jthread t1([&] {
      for (int i = 0; i < 10'000; ++i)
        transfer(a, b, 1);
    });
    std::jthread t2([&] {
      for (int i = 0; i < 10'000; ++i)
        transfer(b, a, 1);
    });
  }

  std::cout << "a = " << a.balance << ", b = " << b.balance
            << " (both should be 1000)\n";
}
