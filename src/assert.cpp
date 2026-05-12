#include <climits>
#include <cmath>
#include <cstdio>
#include <iostream>
// Uncomment to disable runtime assert() — same effect as -DNDEBUG in a release
// build. With NDEBUG defined, `assert(expr)` expands to `((void)0)`.
// #define NDEBUG
#include <cassert>

// ---------------------------------------------------------------------------
// 1) Runtime assert() — preconditions/invariants in a robotics control loop.
//    The asserts here are written to PASS so the demo binary doesn't abort.
//    A failing assert would print e.g.
//        Assertion `(idx >= 0 && idx <= 9) && "idx out of range"` failed.
//    and call std::abort().
// ---------------------------------------------------------------------------
int assertExample() {
  // Imagine `idx` is a joint index coming from a CAN frame; the bus protocol
  // guarantees 0..9 but we assert it anyway to catch firmware bugs early.
  int idx = 2;
  int joint_positions[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  // Custom-message trick #1: bool-expr && "message".
  assert((idx >= 0 && idx <= 9) && "idx should be between 0 and 9");

  // Custom-message trick #2: comma operator, with extra parens so the comma
  // isn't parsed as a macro-argument separator.
  assert(("idx should be between 0 and 9", (idx >= 0 && idx <= 9)));

  // Another typical invariant: battery percentage must be in [0, 100].
  double battery_pct = 87.3;
  assert(battery_pct >= 0.0 && battery_pct <= 100.0 &&
         "battery_pct out of range");

  return joint_positions[idx];
}

// ---------------------------------------------------------------------------
// 2) static_assert — compile-time invariants. Useful for wire-format structs,
//    fixed-width register layouts, and platform/C++-version checks.
// ---------------------------------------------------------------------------
void static_assertExample() {
  // Most desktop/embedded toolchains we target have 32-bit unsigned int. If
  // someone tries to build on a 16-bit MCU toolchain, fail loudly.
  static_assert(sizeof(unsigned int) * CHAR_BIT == 32,
                "unsigned int must be exactly 32 bits");

  // `char` should be signed on our platforms (matters for CAN signal decoding).
  static_assert(CHAR_MIN < 0, "char must be signed");

  // Integer division with negative numerator rounds toward zero in C++11+.
  static_assert(-5 / 2 == -2, "integer division should round toward zero");

  // We require at least C++11; the CMake target actually builds as C++20.
  static_assert(__cplusplus > 201103L, "C++11 or newer required");
}

// ---------------------------------------------------------------------------
// 3) NDEBUG behaviour — with NDEBUG defined, the failing assert below would
//    be stripped and sqrt(-1.0) would silently produce NaN. We use a valid
//    value here so the demo runs to completion regardless of build mode.
// ---------------------------------------------------------------------------
void NDEBUGExample() {
  double x = 1.0;  // change to -1.0 and rebuild without NDEBUG to see it fire
  assert(x >= 0.0 && "sqrt input must be non-negative");
  std::printf("sqrt(x) = %f\n", std::sqrt(x));
}

int main() {
  std::cout << "joint_positions[idx] = " << assertExample() << '\n';
  static_assertExample();
  NDEBUGExample();
  return 0;
}
