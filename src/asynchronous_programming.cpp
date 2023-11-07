#include <chrono> // For std::chrono::seconds
#include <future> // For std::async and std::future
#include <iostream>
#include <thread> // For std::this_thread::sleep_for

// A function that simulates some work by sleeping for 3 seconds
void do_work() {
  std::cout << "Work started\n";
  std::this_thread::sleep_for(std::chrono::seconds(3));
  std::cout << "Work completed\n";
}

int main() {
  // Launch do_work in a separate thread
  std::future<void> work_future = std::async(std::launch::async, do_work);

  std::cout << "Main thread continues executing\n";

  // Optionally, wait for the asynchronous task to complete
  work_future.get();

  std::cout << "Main thread completed\n";

  return 0;
}
