// C++20 Concepts -- companion to docs/concepts.md
//
// Demonstrates, in order:
//   1. Defining a concept with a `requires` expression  (Addable)
//   2. The four kinds of `requires` clauses             (Container)
//   3. The four ways to use a concept                   (sum)
//   4. Standard library concepts                        (std::integral, std::floating_point,
//                                                        std::ranges::range)
//   5. Overload resolution -- most-constrained wins      (describe)
//   6. Concepts vs `if constexpr`                       (process)
//   7. Robotics framing: a Sensor concept               (IMU, Encoder)
//
// Build:  cmake --build build --target concepts -j 4
// Run:    ./build/Debug/concepts

#include <concepts>
#include <cstdint>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// 1. Defining a concept
// ---------------------------------------------------------------------------
template <typename T>
concept Addable = requires(T a, T b) {
  a + b; // simple-requirement: expression must compile
};

static_assert(Addable<int>);
static_assert(Addable<std::string>);
static_assert(!Addable<void *>); // pointers don't have operator+(void*, void*)

// ---------------------------------------------------------------------------
// 2. The four kinds of `requires` clauses
// ---------------------------------------------------------------------------
template <typename T>
concept Container = requires(T c) {
  c.size();                                                 // simple
  typename T::value_type;                                   // type
  { c.size() } -> std::convertible_to<std::size_t>;         // compound
  { c.begin() } -> std::same_as<typename T::iterator>;      // compound
  requires std::default_initializable<T>;                   // nested
};

// ---------------------------------------------------------------------------
// 3. The four ways to use a concept -- all four mean "T must be Addable"
// ---------------------------------------------------------------------------
// Form 1: replace `typename` with the concept name
template <Addable T> T sum1(T a, T b) { return a + b; }

// Form 2: `requires` clause after the template header
template <typename T>
  requires Addable<T>
T sum2(T a, T b) {
  return a + b;
}

// Form 3: trailing `requires` clause
template <typename T> T sum3(T a, T b) requires Addable<T> {
  return a + b;
}

// Form 4: abbreviated function template (no template<> header at all)
auto sum4(Addable auto a, Addable auto b) { return a + b; }

// ---------------------------------------------------------------------------
// 4. Standard library concepts
// ---------------------------------------------------------------------------
template <std::integral T> T factorial(T n) {
  T r = 1;
  for (T i = 2; i <= n; ++i)
    r *= i;
  return r;
}

template <std::floating_point T> T to_radians(T degrees) {
  return degrees * T(3.14159265358979) / T(180);
}

void print_all(std::ranges::range auto &&r) {
  for (auto &&x : r)
    std::cout << x << ' ';
  std::cout << '\n';
}

// ---------------------------------------------------------------------------
// 5. Overload resolution -- most-constrained wins
//
// Subsumption is decided by *atomic-constraint inclusion*, not by logical
// implication. `std::same_as<T,int>` and `std::integral<T>` do not subsume
// each other in libstdc++ (they decompose into unrelated atomic constraints),
// so the doc's literal example is ambiguous on GCC. To keep the demo honest,
// we build the layers from one shared atomic concept (`Integral`) so the
// subsumption lattice is well-defined.
// ---------------------------------------------------------------------------
template <typename T>
concept Integral = std::is_integral_v<T>;

template <typename T>
concept ExactlyInt = Integral<T> && std::is_same_v<T, int>;

template <typename T> std::string describe(T) { return "anything"; }

template <Integral T> std::string describe(T) { return "integer"; }

template <ExactlyInt T> std::string describe(T) { return "exactly int"; }

// ---------------------------------------------------------------------------
// 6. Concepts vs `if constexpr`
// ---------------------------------------------------------------------------
// Concepts: two overloads, signature-level dispatch
template <std::integral T> T process_concept(T x) { return x * 2; }
template <std::floating_point T> T process_concept(T x) {
  return static_cast<T>(x * 0.5);
}

// if constexpr: one overload, body-level dispatch
template <typename T> T process_ifc(T x) {
  if constexpr (std::integral<T>)
    return x * 2;
  else if constexpr (std::floating_point<T>)
    return static_cast<T>(x * 0.5);
  else
    static_assert(sizeof(T) == 0, "unsupported");
}

// ---------------------------------------------------------------------------
// 7. Robotics framing: concepts constraining sensor types
//
//    - Numeric:    payload types we can do math on (filtering, scaling)
//    - Streamable: types we can log via operator<<
//    - Sensor:     types that expose a read() method returning a Numeric value
// ---------------------------------------------------------------------------
template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template <typename T>
concept Streamable = requires(std::ostream &os, T v) {
  { os << v } -> std::same_as<std::ostream &>;
};

template <typename S>
concept Sensor = requires(S s) {
  typename S::value_type;
  { s.read() } -> std::same_as<typename S::value_type>;
  requires Numeric<typename S::value_type>;
};

// A simulated IMU axis -- reads a floating-point angular rate (rad/s).
struct IMU {
  using value_type = double;
  double rate = 0.0;
  value_type read() {
    rate += 0.01;
    return rate;
  }
};

// A simulated wheel encoder -- reads an integer tick count.
struct Encoder {
  using value_type = std::int32_t;
  std::int32_t ticks = 0;
  value_type read() {
    ticks += 17;
    return ticks;
  }
};

static_assert(Sensor<IMU>);
static_assert(Sensor<Encoder>);
static_assert(!Sensor<int>); // int has no read() member

// Generic sample-and-log routine usable for any Sensor whose value_type
// is Streamable -- the kind of constraint a robotics middleware would
// enforce on a logger plugin.
template <Sensor S>
  requires Streamable<typename S::value_type>
void sample_and_log(S &s, int n, const char *name) {
  std::cout << "  " << name << ":";
  for (int i = 0; i < n; ++i)
    std::cout << ' ' << s.read();
  std::cout << '\n';
}

// ---------------------------------------------------------------------------
// main -- labeled demos
// ---------------------------------------------------------------------------
int main() {
  std::cout << "=== 1. Defining a concept: Addable ===\n";
  std::cout << "  Addable<int>          = " << Addable<int> << '\n';
  std::cout << "  Addable<std::string>  = " << Addable<std::string> << '\n';
  std::cout << "  Addable<void*>        = " << Addable<void *> << '\n';

  std::cout << "\n=== 2. Four kinds of requires clauses: Container ===\n";
  std::cout << "  Container<std::vector<int>> = " << Container<std::vector<int>>
            << '\n';
  std::cout << "  Container<int>              = " << Container<int> << '\n';

  std::cout << "\n=== 3. Four ways to use a concept (Addable sum) ===\n";
  std::cout << "  sum1(2, 3)              = " << sum1(2, 3) << '\n';
  std::cout << "  sum2(2.5, 0.25)         = " << sum2(2.5, 0.25) << '\n';
  std::cout << "  sum3(std::string,...)   = "
            << sum3<std::string>("hello, ", "world") << '\n';
  std::cout << "  sum4(1, 2.5) (mixed T)  = " << sum4(1, 2.5) << '\n';

  std::cout << "\n=== 4. Standard library concepts ===\n";
  std::cout << "  factorial(5)            = " << factorial(5) << '\n';
  std::cout << "  to_radians(180.0)       = " << to_radians(180.0) << '\n';
  // factorial(5.0);  // would fail: 5.0 is not std::integral
  std::vector v{1, 2, 3, 4};
  std::cout << "  print_all(vector)       = ";
  print_all(v);

  std::cout << "\n=== 5. Overload resolution: most-constrained wins ===\n";
  std::cout << "  describe(3.14)          -> " << describe(3.14) << '\n';
  std::cout << "  describe(42L)           -> " << describe(42L) << '\n';
  std::cout << "  describe(42)            -> " << describe(42) << '\n';

  std::cout << "\n=== 6. Concepts vs if constexpr (same result) ===\n";
  std::cout << "  process_concept(10)     = " << process_concept(10) << '\n';
  std::cout << "  process_concept(10.0)   = " << process_concept(10.0) << '\n';
  std::cout << "  process_ifc(10)         = " << process_ifc(10) << '\n';
  std::cout << "  process_ifc(10.0)       = " << process_ifc(10.0) << '\n';

  std::cout << "\n=== 7. Robotics: Sensor concept ===\n";
  IMU imu;
  Encoder enc;
  sample_and_log(imu, 3, "IMU rate (rad/s)");
  sample_and_log(enc, 3, "Encoder ticks   ");
  // sample_and_log(42, 3, "bad");  // would fail: int does not satisfy Sensor

  std::cout << "\nAll demos ran.\n";
  return 0;
}
