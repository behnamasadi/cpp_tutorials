#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class wallet {
public:
  int money;
  wallet() : money(0) {}
  void increaseMoney(int amount) { money = money + amount; }
};

int racingProblem() {
  wallet walletObject;
  std::vector<std::thread> threads;
  for (int i = 0; i < 5; ++i) {
    threads.push_back(std::thread(&wallet::increaseMoney, &walletObject, 10));
  }

  for (int i = 0; i < threads.size(); i++) {
    threads.at(i).join();
  }
  return walletObject.money;
}

void racingProblemExample() {
  int val = 0;
  for (int k = 0; k < 10000; k++) {
    if ((val = racingProblem()) != 50) {
      std::cout << "Error at count = " << k << " Money in Wallet = " << val
                << std::endl;
    }
  }
}

int counter = 0;

void worker() {
  counter++;
  std::cout << "job number: " << counter << " started" << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::cout << "job number: " << counter << " finished" << std::endl;
}

int main() {
  std::thread t1(worker);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::thread t2(worker);
  t1.join();
  t2.join();
}
