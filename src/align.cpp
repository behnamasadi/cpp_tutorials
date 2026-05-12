// Memory alignment demos -- see docs/align.md for the canonical narrative.
//
// We mirror every named technique from the doc:
//   * alignof() of built-in types
//   * struct padding and the cost of bad member ordering
//   * alignas() on arrays, structs, and per-member (cache-line padding)
//   * std::hardware_destructive_interference_size (C++17)
//   * sysconf(_SC_LEVEL1_DCACHE_LINESIZE) at runtime (Linux)
//
// The Eigen section of the doc is illustrated with comments only -- Eigen is
// not linked into this tutorial's build.

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <new>      // std::hardware_destructive_interference_size
#include <unistd.h> // sysconf

// -------- structs from the doc --------

struct Foo {
  char c; // 1 byte
  int i;  // 4 bytes, must start at a 4-aligned offset -> 3 bytes of padding
};        // sizeof == 8, alignof == 4

// Reordering members reduces padding.
struct Bad {
  char a;
  int b;
  char c;
  int d;
}; // sizeof == 16
struct Good {
  int b;
  int d;
  char a;
  char c;
}; // sizeof == 12

// `alignas(N)` on the struct forces it to start at an N-aligned address and
// rounds its size up to a multiple of N.
struct alignas(64) CacheLineSized {
  int data[16]; // 16 * 4 == 64 bytes already; no extra trailing padding
};

// False-sharing fix: each atomic lands on its own cache line.
struct alignas(64) PerThreadCounter {
  std::atomic<long> value;
  // ~56 bytes of trailing padding inserted automatically by alignas
};

// The "struct straddles two cache lines" example from the doc.
struct StraddlingFoo {
  int a;
  double b;
  int c;
}; // sizeof == 24, alignof == 8

struct alignas(64) PinnedFoo {
  int a;
  double b;
  int c;
}; // sizeof == 64

// Robotics-flavoured: separate cache lines for left/right wheel encoder ticks
// so two ISR threads aren't bouncing the same line back and forth.
struct WheelCounters {
  alignas(64) std::atomic<long> left_ticks;
  alignas(64) std::atomic<long> right_ticks;
}; // sizeof == 128

// Portable version using hardware_destructive_interference_size where the
// implementation provides it.
#ifdef __cpp_lib_hardware_interference_size
struct WheelCountersPortable {
  alignas(std::hardware_destructive_interference_size) std::atomic<long> left_ticks;
  alignas(std::hardware_destructive_interference_size) std::atomic<long> right_ticks;
};
#endif

// -------- demos --------

static void demoAlignof() {
  std::cout << "--- alignof of built-in types ---\n";
  std::cout << "char:   " << alignof(char) << '\n';
  std::cout << "short:  " << alignof(short) << '\n';
  std::cout << "int:    " << alignof(int) << '\n';
  std::cout << "double: " << alignof(double) << '\n';
  std::cout << "void*:  " << alignof(void *) << '\n';
  std::cout << '\n';
}

static void demoPadding() {
  std::cout << "--- padding inside structs ---\n";
  std::cout << "sizeof(Foo)  = " << sizeof(Foo) << "  (1-byte char + 3 pad + 4-byte int)\n";
  std::cout << "alignof(Foo) = " << alignof(Foo) << '\n';

  std::cout << "sizeof(Bad)  = " << sizeof(Bad) << "  (char,int,char,int -- 6 wasted bytes)\n";
  std::cout << "sizeof(Good) = " << sizeof(Good) << "  (int,int,char,char -- 2 wasted bytes)\n";
  std::cout << '\n';
}

static void demoAlignas() {
  std::cout << "--- alignas on arrays and structs ---\n";

  alignas(32) float buffer[8];
  std::cout << "alignas(32) float buffer[8]\n";
  std::cout << "  &buffer[0] = " << static_cast<const void *>(&buffer[0])
            << "  (mod 32 = " << (reinterpret_cast<std::uintptr_t>(&buffer[0]) % 32) << ")\n";
  std::cout << "  &buffer[1] = " << static_cast<const void *>(&buffer[1])
            << "  (mod 32 = " << (reinterpret_cast<std::uintptr_t>(&buffer[1]) % 32)
            << ", only natural 4-byte alignment)\n";

  std::cout << "alignof(CacheLineSized) = " << alignof(CacheLineSized) << '\n';
  std::cout << "sizeof(CacheLineSized)  = " << sizeof(CacheLineSized) << '\n';

  std::cout << "alignof(PerThreadCounter) = " << alignof(PerThreadCounter) << '\n';
  std::cout << "sizeof(PerThreadCounter)  = " << sizeof(PerThreadCounter)
            << "  (atomic<long> + trailing padding)\n";
  std::cout << '\n';
}

static void demoCacheLines() {
  std::cout << "--- cache lines ---\n";

  // Compile-time, portable (C++17 and the implementation must provide it).
#ifdef __cpp_lib_hardware_interference_size
  std::cout << "std::hardware_destructive_interference_size = "
            << std::hardware_destructive_interference_size << '\n';
#else
  std::cout << "std::hardware_destructive_interference_size: not provided by this stdlib\n";
#endif

  // Runtime, Linux.
  long line = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
  std::cout << "sysconf(_SC_LEVEL1_DCACHE_LINESIZE)          = " << line << '\n';

  std::cout << "sizeof(StraddlingFoo) = " << sizeof(StraddlingFoo)
            << "  (24 bytes -- can straddle a 64-byte line)\n";
  std::cout << "sizeof(PinnedFoo)     = " << sizeof(PinnedFoo)
            << "  (padded to one full cache line)\n";

  WheelCounters c{};
  auto base = reinterpret_cast<std::uintptr_t>(&c);
  std::cout << "WheelCounters: left  at offset 0, right at offset "
            << (reinterpret_cast<std::uintptr_t>(&c.right_ticks) - base)
            << "  (each atomic on its own cache line)\n";
  std::cout << "sizeof(WheelCounters) = " << sizeof(WheelCounters) << '\n';
  std::cout << '\n';
}

static void demoRawBytes() {
  std::cout << "--- raw bytes of a Foo instance (showing padding bytes) ---\n";
  Foo f{};
  f.c = 'A'; // 0x41
  f.i = 0x01020304;
  auto *bytes = reinterpret_cast<const unsigned char *>(&f);
  for (std::size_t i = 0; i < sizeof(Foo); ++i) {
    std::printf("%02x ", bytes[i]);
  }
  std::printf("\n  ^ byte 0 is 'A' (0x41), bytes 1-3 are padding, bytes 4-7 are the int "
              "(little-endian).\n\n");
}

int main() {
  demoAlignof();
  demoPadding();
  demoAlignas();
  demoCacheLines();
  demoRawBytes();
  return 0;
}
