#include <future>
#include <iostream>
#include <thread>

int adder(int a, int b) {
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  return a + b;
}

void task_thread() {
  std::packaged_task<int(int, int)> adder_task(adder);
  std::future<int> result = adder_task.get_future();
  std::thread t1(std::move(adder_task), 2, 3);
  std::cout << "packaged_task called" << std::endl;
  t1.join();
  std::cout << result.get() << std::endl;
}
void task_simple() {
  std::packaged_task<int(int, int)> adder_task(adder);
  std::future<int> result = adder_task.get_future();
  adder_task(2, 3);
  std::cout << "packaged_task called" << std::endl;
  std::cout << result.get() << std::endl;
}
int main() {
  //   task_simple();
  task_thread();
}
