// Demonstrates §4.3 of docs/multithreading.md: counting_semaphore for
// capacity limiting, binary_semaphore for one-shot signaling.

#include <chrono>
#include <iostream>
#include <semaphore>
#include <syncstream>
#include <thread>
#include <vector>

// §4.3.1 — cap N concurrent users of a resource (e.g. DB connections).
std::counting_semaphore<3> db_slots(3);

void query(int id) {
  db_slots.acquire(); // blocks if 3 are already in flight
  std::osyncstream(std::cout) << "  query " << id << " running\n";
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  db_slots.release();
}

// §4.3.2 — one-shot signal from producer to consumer.
std::binary_semaphore data_ready(0);
int shared_pose = 0;

int main() {
  std::cout << "counting_semaphore — 6 queries, only 3 run concurrently:\n";
  {
    std::vector<std::jthread> pool;
    for (int i = 0; i < 6; ++i)
      pool.emplace_back(query, i);
  } // joined

  std::cout << "binary_semaphore — consumer waits for producer:\n";
  {
    std::jthread producer([] {
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
      shared_pose = 42;
      data_ready.release(); // signal
    });
    std::jthread consumer([] {
      data_ready.acquire(); // blocks until producer signals
      std::cout << "  consumer sees pose = " << shared_pose << '\n';
    });
  }
}
