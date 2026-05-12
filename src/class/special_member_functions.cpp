// Demonstrates the six special member functions (SMFs) on a class that
// actually owns a resource — a heap-allocated float[] buffer modelling a
// fixed-rate sensor sample (e.g. one frame of LIDAR ranges or IMU samples).
//
// The point of using a real resource: the difference between copy and move
// only matters when there is something expensive or unique to own. A
// std::string-sized POD class would let the compiler-generated SMFs do the
// right thing and you would learn nothing.
//
// Build:
//   cmake --build build --target special_member_functions -j 4
//
// What you should observe in the output:
//   - default ctor allocates an empty-ish buffer
//   - copy ctor / copy assign do a *deep* copy (data pointers differ)
//   - move ctor / move assign steal the pointer (source is left empty)
//   - dtor logs which instance is being destroyed and whether it owned data

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string>
#include <utility>

class SensorBuffer {
public:
  // 1. Default constructor.
  // Allocates a zero-initialised frame. Useful for "no reading yet" state.
  SensorBuffer() : SensorBuffer("default", 0) {}

  // Tag constructor so we can label instances in the log output. Not a SMF
  // but it lets the demo print which object each line is talking about.
  SensorBuffer(std::string tag, std::size_t n)
      : tag_(std::move(tag)), size_(n), data_(n ? new float[n]{} : nullptr) {
    std::cout << "  [ctor]      " << tag_ << " allocates " << size_
              << " samples at " << static_cast<void *>(data_) << '\n';
  }

  // 2. Copy constructor — deep copy. If we used the compiler-generated one
  // we would share the float* between two objects and double-free on dtor.
  SensorBuffer(const SensorBuffer &other)
      : tag_(other.tag_ + "_copy"), size_(other.size_),
        data_(other.size_ ? new float[other.size_] : nullptr) {
    std::copy(other.data_, other.data_ + other.size_, data_);
    std::cout << "  [copy-ctor] " << tag_ << " <- " << other.tag_
              << " deep-copies " << size_ << " samples (new buffer at "
              << static_cast<void *>(data_) << ")\n";
  }

  // 3. Copy assignment — copy-and-swap idiom. Strong exception guarantee:
  // if the allocation in the copy ctor throws, *this is unchanged.
  SensorBuffer &operator=(const SensorBuffer &other) {
    std::cout << "  [copy=]     " << tag_ << " = " << other.tag_ << '\n';
    SensorBuffer tmp(other);
    swap(tmp);
    return *this;
  }

  // 4. Move constructor — noexcept so std::vector<SensorBuffer> will move
  // (not copy) on reallocation. Steals the pointer, leaves source empty.
  SensorBuffer(SensorBuffer &&other) noexcept
      : tag_(std::move(other.tag_)), size_(other.size_), data_(other.data_) {
    other.size_ = 0;
    other.data_ = nullptr;
    std::cout << "  [move-ctor] " << tag_ << " steals buffer at "
              << static_cast<void *>(data_) << " (source emptied)\n";
  }

  // 5. Move assignment — also noexcept. Release current, then steal.
  SensorBuffer &operator=(SensorBuffer &&other) noexcept {
    std::cout << "  [move=]     " << tag_ << " = std::move(" << other.tag_
              << ")\n";
    if (this != &other) {
      delete[] data_;
      tag_ = std::move(other.tag_);
      size_ = other.size_;
      data_ = other.data_;
      other.size_ = 0;
      other.data_ = nullptr;
    }
    return *this;
  }

  // 6. Destructor — releases the heap buffer. Logs so we can see Rule of
  // Five in action: every constructed object gets exactly one dtor call,
  // and only the *real* owner frees the buffer (moved-from objects log
  // "(empty)").
  ~SensorBuffer() {
    std::cout << "  [dtor]      " << tag_ << " releases "
              << (data_ ? "buffer" : "(empty)") << " at "
              << static_cast<void *>(data_) << '\n';
    delete[] data_;
  }

  void fill(float v) { std::fill(data_, data_ + size_, v); }
  float front() const { return size_ ? data_[0] : 0.0f; }
  std::size_t size() const { return size_; }
  const std::string &tag() const { return tag_; }

private:
  void swap(SensorBuffer &other) noexcept {
    using std::swap;
    swap(tag_, other.tag_);
    swap(size_, other.size_);
    swap(data_, other.data_);
  }

  std::string tag_;
  std::size_t size_ = 0;
  float *data_ = nullptr;
};

static void banner(const char *title) {
  std::cout << "\n=== " << title << " ===\n";
}

int main() {
  banner("1. Default construction");
  SensorBuffer empty; // calls default ctor

  banner("2. Parameterised construction (a real lidar frame)");
  SensorBuffer lidar("lidar", 8);
  lidar.fill(1.25f);
  std::cout << "  lidar.front() = " << lidar.front() << " size=" << lidar.size()
            << '\n';

  banner("3. Copy construction: SensorBuffer dup(lidar)");
  // Deep copy — dup must own its own buffer so modifying lidar later does
  // not affect dup and the destructors do not double-free.
  SensorBuffer dup(lidar);
  lidar.fill(9.0f);
  std::cout << "  after lidar.fill(9): lidar.front()=" << lidar.front()
            << "  dup.front()=" << dup.front() << "  (should differ)\n";

  banner("4. Copy assignment: imu = lidar");
  SensorBuffer imu("imu", 4);
  imu.fill(0.1f);
  imu = lidar; // copy-assign

  banner("5. Move construction: SensorBuffer stolen(std::move(dup))");
  SensorBuffer stolen(std::move(dup));
  std::cout << "  dup.size() after move = " << dup.size()
            << " (should be 0)\n";

  banner("6. Move assignment: empty = std::move(stolen)");
  empty = std::move(stolen);
  std::cout << "  stolen.size() after move = " << stolen.size()
            << " (should be 0)\n";
  std::cout << "  empty now owns " << empty.size() << " samples, front="
            << empty.front() << '\n';

  banner("7. Self-assignment guard (move=)");
  // Pathological but legal: a = std::move(a). Our guard prevents UB.
  empty = std::move(empty);
  std::cout << "  empty still has size " << empty.size() << '\n';

  banner("End of main — destructors fire in reverse construction order");
  return 0;
}
