// Demonstrates §3.3 of docs/multithreading.md: std::thread::id for
// per-thread bookkeeping. Both std::thread and std::jthread use the same id type.

#include <iostream>
#include <syncstream>
#include <thread>
#include <unordered_map>
#include <vector>

void sensor_loop(int sensor_id) {
  // osyncstream prevents interleaved characters within a single line.
  std::osyncstream(std::cout)
      << "sensor " << sensor_id
      << " running on thread " << std::this_thread::get_id() << '\n';
}

int main() {
  std::cout << "main thread id: " << std::this_thread::get_id() << '\n';

  // Spawn three sensor threads; remember which jthread served which sensor.
  std::unordered_map<std::thread::id, int> sensor_of;
  std::vector<std::jthread> threads;
  for (int sensor = 0; sensor < 3; ++sensor) {
    std::jthread t(sensor_loop, sensor);
    sensor_of[t.get_id()] = sensor;
    threads.push_back(std::move(t));
  }

  for (const auto &[tid, sensor] : sensor_of)
    std::cout << "  sensor " << sensor << " -> thread " << tid << '\n';
}
