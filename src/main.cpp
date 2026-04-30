#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

// Bounded SPSC queue: producer blocks when full, consumer blocks when empty.
// No data is lost; the producer is throttled by the queue's capacity.
class BoundedQueue {
public:
  explicit BoundedQueue(std::size_t capacity) : capacity_(capacity) {}

  void push(int value) {
    std::unique_lock lock(mutex_);
    not_full_.wait(lock, [this] { return queue_.size() < capacity_; });
    queue_.push(value);
    not_empty_.notify_one();
  }

  int pop() {
    std::unique_lock lock(mutex_);
    not_empty_.wait(lock, [this] { return !queue_.empty(); });
    int value = queue_.front();
    queue_.pop();
    not_full_.notify_one();
    return value;
  }

private:
  std::size_t capacity_;
  std::queue<int> queue_;
  std::mutex mutex_;
  std::condition_variable not_full_, not_empty_;
};

int main() {
  std::cout << "--- bounded queue (capacity=4, no loss) ---\n";
  {
    BoundedQueue queue(4);
    std::jthread producer([&] {
      for (int i = 0; i < 10; ++i) {
        queue.push(i);
        std::cout << "produced " << i << '\n';
      }
    });
    std::jthread consumer([&] {
      for (int i = 0; i < 10; ++i)
        std::cout << "  consumed " << queue.pop() << '\n';
    });
  }

  // Latest-value: producer overwrites, consumer reads the most recent.
  // For a plain int, std::atomic is all we need.
  std::cout << "\n--- latest value (60Hz producer, 10Hz consumer) ---\n";
  {
    std::atomic<int> latest = -1;
    std::jthread producer([&] {
      for (int frame = 0; frame < 30; ++frame) {
        latest.store(frame);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
      }
    });
    std::jthread consumer([&] {
      for (int i = 0; i < 5; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "  consumed frame " << latest.load() << '\n';
      }
    });
  }
}
