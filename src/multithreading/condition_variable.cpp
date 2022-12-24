#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

namespace polling_deque {
std::deque<int> q;
std::mutex mu;

void function_1() {
  int count = 10;
  while (count > 0) {
    std::unique_lock<std::mutex> locker(
        mu); // this will create and lock the mutex
    q.push_front(count);
    locker.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    count--;
  }
}

void function_2() {
  int data = 0;
  while (data != 1) {
    std::unique_lock<std::mutex> locker(mu);
    if (!q.empty()) {
      data = q.back();
      q.pop_back();
      locker.unlock();
      std::cout << "t2 got a value from t1: " << data << std::endl;
    } else {
      locker.unlock();
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
}
} // namespace polling_deque

namespace condition_variable_deque {
std::deque<int> q;
std::mutex mu;
std::condition_variable cond;
void function_1() {
  int count = 10;
  while (count > 0) {
    std::unique_lock<std::mutex> locker(mu);
    q.push_front(count);
    locker.unlock();
    cond.notify_one(); // Notify one waiting thread, if there is one.
    std::this_thread::sleep_for(std::chrono::seconds(1));
    count--;
  }
}

void function_2() {
  int data = 0;
  while (data != 1) {
    std::unique_lock<std::mutex> locker(mu);
    cond.wait(locker, []() { return !q.empty(); });
    data = q.back();
    q.pop_back();
    locker.unlock();
    std::cout << "t2 got a value from t1: " << data << std::endl;
  }
}
} // namespace condition_variable_deque

std::condition_variable cond;
std::mutex mu;

bool main_thread_is_ready = false;
bool data_has_been_processed = false;
int worker_func() {
  std::unique_lock<std::mutex> worker_lock(mu);
  cond.wait(worker_lock, [] { return main_thread_is_ready; });
  std::cout << "Worker thread is processing data" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  data_has_been_processed = true;
  std::cout << "Worker thread has processed the data" << std::endl;
  /* Manual unlocking is done before notifying, to avoid waking up the waiting
   * thread only to block again. */
  worker_lock.unlock();
  cond.notify_one();
}

int main() {
  { // queue example
    std::cout << "Queueing and Dequeueing with polling" << std::endl;

    {
      std::thread t1(polling_deque::function_1);
      std::thread t2(polling_deque::function_2);

      t1.join();
      t2.join();
    }

    std::cout << "Queueing and Dequeueing with condition_variable" << std::endl;
    {
      std::thread t1(condition_variable_deque::function_1);
      std::thread t2(condition_variable_deque::function_2);

      t1.join();
      t2.join();
    }
  }

  { // worker example
    std::thread worker_thread(worker_func);
    {
      std::cout << "pre-preparing data in main thread" << std::endl;
      std::lock_guard<std::mutex> main_thread_lock(mu);
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::cout << "main thread is ready" << std::endl;
      main_thread_is_ready = true;
    }
    cond.notify_one();
    std::unique_lock<std::mutex> main_lock(mu);
    cond.wait(main_lock, [] { return data_has_been_processed; });
    std::cout << "Back in main()" << std::endl;
    worker_thread.join();
  }
}
