// Companion to docs/functions.md
//
// Topics already demonstrated elsewhere (not repeated here):
//   - function pointers .......... src/function_pointer.cpp
//   - std::function / bind ....... src/function.cpp, src/bind.cpp
//   - functors ................... src/function_objects.cpp
//   - lambdas .................... src/lambda.cpp
//   - callbacks .................. src/callbacks.cpp
//   - inline functions ........... src/inline_functions.cpp
//   - extern "C" ................. src/extern/function/
//
// This file covers what is UNIQUE to docs/functions.md:
//   - constexpr functions (compile-time evaluation)
//   - noexcept functions
//   - returning by value (RVO / move semantics for std::vector)
//   - returning a reference to a member (safe)
//   - returning a const reference (no copy)
//   - returning a reference to a local (DANGEROUS — shown but not called)

#include <iostream>
#include <vector>

// --- constexpr: can run at compile time -----------------------------------
// Imagine a robot joint with a fixed gear ratio computed at compile time.
constexpr long gear_steps(int reductions) {
  return (reductions <= 0) ? 1 : 2 * gear_steps(reductions - 1);
}

// --- noexcept: promises not to throw --------------------------------------
// Useful for realtime callbacks where exceptions are forbidden.
int clamp_pwm(int v) noexcept {
  if (v < 0)
    return 0;
  if (v > 255)
    return 255;
  return v;
}

// --- returning by value: RVO / move avoids copy ---------------------------
// Returning a local std::vector is fine — the compiler elides the copy.
std::vector<int> make_encoder_log() {
  std::vector<int> log;
  log.push_back(100);
  log.push_back(101);
  log.push_back(103);
  return log; // RVO / move, not a deep copy
}

// --- returning a reference to a member: SAFE ------------------------------
// The member outlives the call because the caller owns the object.
struct Sensor {
  int last_reading = 42;
  const int &get_reading() const { return last_reading; }
};

// --- returning a const reference: no copy, picks the larger one ----------
const std::vector<int> &longer(const std::vector<int> &a,
                                const std::vector<int> &b) {
  return (a.size() >= b.size()) ? a : b;
}

// --- DANGEROUS: returning a reference to a local --------------------------
// Shown for illustration only. Do NOT call this — the reference dangles
// the moment the function returns. Most compilers warn about it.
#if 0
int &bad_get() {
  int tmp = 7;
  return tmp; // tmp dies here; reference is invalid
}
#endif

int main() {
  // constexpr
  constexpr long steps = gear_steps(5); // evaluated at compile time
  std::cout << "[constexpr] gear_steps(5) = " << steps << "\n";

  // noexcept
  std::cout << "[noexcept]  clamp_pwm(300) = " << clamp_pwm(300) << "\n";

  // return by value (RVO/move)
  std::vector<int> v = make_encoder_log();
  std::cout << "[return vec] size = " << v.size()
            << ", front = " << v.front() << "\n";

  // safe reference return (to a member)
  Sensor s;
  const int &r = s.get_reading();
  std::cout << "[ref to member] reading = " << r << "\n";

  // const reference return: no copy of either vector
  std::vector<int> a = {1, 2};
  std::vector<int> b = {1, 2, 3, 4};
  const std::vector<int> &big = longer(a, b);
  std::cout << "[const ref] longer.size = " << big.size() << "\n";

  return 0;
}
