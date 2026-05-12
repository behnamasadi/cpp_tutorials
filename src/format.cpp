// Companion code for docs/format.md
//
// std::format is C++20. std::print / std::println are C++23.
// We gate the C++23 parts behind __cpp_lib_print so the file
// still builds on older libstdc++.

#include <chrono>
#include <format>
#include <iostream>
#include <iterator>
#include <string>

#ifdef __cpp_lib_print
#include <print>
#endif

// ---- Custom type for std::formatter<T> specialization ----
struct Point {
  double x, y;
};

template <> struct std::formatter<Point> {
  // Accept only an empty spec, i.e. "{}".
  constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

  auto format(const Point &p, std::format_context &ctx) const {
    return std::format_to(ctx.out(), "({}, {})", p.x, p.y);
  }
};

int main() {
  // ---- Basic usage ----
  std::cout << "-- basic --\n";
  int a = 7, b = 35;
  std::string s = std::format("{} + {} = {}", a, b, a + b);
  std::cout << s << '\n';

  // ---- Width, alignment, fill ----
  std::cout << "-- width / align / fill --\n";
  std::cout << std::format("[{:>10}]", "hi") << '\n';  // right
  std::cout << std::format("[{:<10}]", "hi") << '\n';  // left
  std::cout << std::format("[{:^10}]", "hi") << '\n';  // center
  std::cout << std::format("[{:*^10}]", "hi") << '\n'; // fill '*'

  // ---- Precision ----
  std::cout << "-- precision --\n";
  std::cout << std::format("{:.3f}", 3.14159) << '\n';   // 3.142
  std::cout << std::format("{:.3e}", 12345.6) << '\n';   // 1.235e+04
  std::cout << std::format("{:.5}", "abcdefg") << '\n';  // abcde

  // ---- Numeric base, sign, zero-pad ----
  std::cout << "-- base / sign / pad --\n";
  std::cout << std::format("{:#x}", 255) << '\n';  // 0xff
  std::cout << std::format("{:#o}", 8) << '\n';    // 010
  std::cout << std::format("{:08b}", 13) << '\n';  // 00001101
  std::cout << std::format("{:+d}", 42) << '\n';   // +42

  // ---- Combining ----
  std::cout << "-- combined --\n";
  double pi = 3.14159265;
  std::cout << std::format("|{:>15.4f}|", pi) << '\n';
  std::cout << std::format("|{:0>15.4f}|", pi) << '\n';

  // ---- Positional arguments ----
  std::cout << "-- positional --\n";
  std::cout << std::format("{0} {1} {0}", "hi", "world") << '\n';
  std::cout << std::format("{1} loves {0}", "Alice", "Bob") << '\n';

  // ---- Chrono ----
  std::cout << "-- chrono --\n";
  auto now = std::chrono::system_clock::now();
  std::cout << std::format("{:%Y-%m-%d %H:%M:%S}", now) << '\n';
  std::cout << std::format("{:%A}", now) << '\n';
  using namespace std::chrono_literals;
  std::cout << std::format("{}", 250ms) << '\n';
  std::cout << std::format("{:%Q %q}", 1500ms) << '\n';

  // ---- Custom formatter ----
  std::cout << "-- custom formatter --\n";
  Point p{1.5, 2.5};
  std::cout << std::format("p = {}", p) << '\n';

  // ---- std::format_to: write into an existing buffer ----
  std::cout << "-- format_to --\n";
  std::string out;
  std::format_to(std::back_inserter(out), "x={}, y={}", p.x, p.y);
  std::cout << out << '\n';

  // ---- std::vformat: format string known only at runtime ----
  std::cout << "-- vformat --\n";
  std::string fmt = "{} and {}";
  std::cout << std::vformat(fmt, std::make_format_args(a, b)) << '\n';

  // ---- C++23: std::print / std::println ----
#ifdef __cpp_lib_print
  std::cout << "-- print / println (C++23) --\n";
  std::print("x = {}\n", 42);
  std::println("x = {}", 42);
#else
  std::cout << "-- print / println: not available (need C++23 <print>) --\n";
#endif

  return 0;
}
