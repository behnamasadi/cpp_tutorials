#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>

/*
Allocators encapsulate memory management. std::allocator separates allocation
from object construction, and destruction from deallocation. This matters when:

  - you want to reserve raw storage and only construct elements lazily
    (e.g. a ring buffer of IMU samples sized at startup);
  - you want a custom strategy (pool, arena, slab) plugged into an STL
    container without rewriting the container itself.

The default allocator just forwards to operators new/delete.

Member functions associated with std::allocator (per the doc):

  1) address      -- removed in C++20.
  2) construct    -- removed in C++20.
  3) destroy      -- removed in C++20.
  4) max_size     -- deprecated in C++17, removed in C++20.
  5) allocate     -- allocates raw, uninitialised storage.
  6) deallocate   -- releases storage previously returned by allocate.

For the removed ones, the modern replacements live in <memory>:

  std::construct_at(p, args...)            replaces allocator::construct
  std::destroy_at(p)                       replaces allocator::destroy
  std::allocator_traits<A>::max_size(a)    replaces allocator::max_size

This file demonstrates both: (1) the raw two-step allocate/construct pattern
the doc describes, and (2) a minimal custom allocator (the doc's "GameAllocator"
shape, re-cast as a logging allocator for a robotics sensor pipeline) plugged
into std::vector.
*/

// -----------------------------------------------------------------------------
// (1) Raw two-step allocate + construct + destroy + deallocate
// -----------------------------------------------------------------------------
static void demo_raw_allocator() {
  std::cout << "--- raw std::allocator two-step ---\n";

  std::allocator<int> myAllocator;
  using Traits = std::allocator_traits<decltype(myAllocator)>;

  // allocate raw storage for 4 ints (no constructors run yet).
  int *arr = Traits::allocate(myAllocator, 4);

  // construct arr[0] and arr[3] in-place; arr[1] and arr[2] are still raw.
  Traits::construct(myAllocator, arr + 0, 10);
  Traits::construct(myAllocator, arr + 3, 100);

  std::cout << "arr[0]   = " << arr[0] << '\n';
  std::cout << "arr[3]   = " << arr[3] << '\n';
  std::cout << "max_size = " << Traits::max_size(myAllocator) << '\n';

  // destroy only the elements we actually constructed, then release storage.
  Traits::destroy(myAllocator, arr + 0);
  Traits::destroy(myAllocator, arr + 3);
  Traits::deallocate(myAllocator, arr, 4);
}

// -----------------------------------------------------------------------------
// (2) Custom allocator plugged into std::vector
//     The doc's example:
//
//         template <typename T>
//         class GameAllocator : public std::allocator<T> { ... };
//         std::vector<Particle, GameAllocator<Particle>> particleList;
//
//     Robotics flavour: a SensorSample stream that we want to trace
//     allocations for, e.g. to verify that a realtime loop is not silently
//     hitting the heap after warm-up.
// -----------------------------------------------------------------------------
struct SensorSample {
  double timestamp;
  double value;
};

template <typename T> struct LoggingAllocator {
  using value_type = T;

  LoggingAllocator() noexcept = default;

  template <typename U>
  LoggingAllocator(const LoggingAllocator<U> &) noexcept {}

  T *allocate(std::size_t n) {
    std::cout << "  [LoggingAllocator] allocate(" << n
              << " * " << sizeof(T) << " B)\n";
    return static_cast<T *>(::operator new(n * sizeof(T)));
  }

  void deallocate(T *p, std::size_t n) noexcept {
    std::cout << "  [LoggingAllocator] deallocate(" << n
              << " * " << sizeof(T) << " B)\n";
    ::operator delete(p);
  }
};

template <typename T, typename U>
bool operator==(const LoggingAllocator<T> &, const LoggingAllocator<U> &) {
  return true;
}
template <typename T, typename U>
bool operator!=(const LoggingAllocator<T> &, const LoggingAllocator<U> &) {
  return false;
}

static void demo_custom_allocator() {
  std::cout << "--- custom allocator in std::vector ---\n";

  std::vector<SensorSample, LoggingAllocator<SensorSample>> samples;

  // reserve() should only trigger storage allocation, not element construction.
  std::cout << "reserve(4):\n";
  samples.reserve(4);

  std::cout << "push_back 3 samples:\n";
  samples.push_back({0.001, 9.81});
  samples.push_back({0.002, 9.80});
  samples.push_back({0.003, 9.82});

  for (const auto &s : samples)
    std::cout << "  t=" << s.timestamp << " a=" << s.value << '\n';

  std::cout << "vector going out of scope:\n";
}

int main() {
  demo_raw_allocator();
  std::cout << '\n';
  demo_custom_allocator();
  return 0;
}
