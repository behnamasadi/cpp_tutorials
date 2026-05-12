// Demos for docs/macros.md
// Build: cmake --build build --target macro -j 4
// Run:   ./build/Debug/macro
//
// CMakeLists.txt sets: target_compile_definitions(macro PRIVATE LOGING=0)

#include <iostream>

// 1) Defining constants with #define
#define PI 3.14159

// 2) Function-like macro. Parenthesize every argument and the whole body
//    so operator precedence does not bite you.
#define SQUARE(x) ((x) * (x))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// 3) Conditional logging macro driven by -DLOGING=0/1 from CMake.
#if LOGING == 1
#define LOG(msg) std::cout << "[LOG] " << msg << std::endl
#else
#define LOG(msg)
#endif

void constants_demo() {
  std::cout << "--- constants ---\n";
  std::cout << "PI = " << PI << "\n";
}

void function_like_demo() {
  std::cout << "--- function-like macros ---\n";
  int x = 5, y = 10;
  std::cout << "SQUARE(4) = " << SQUARE(4) << "\n";
  std::cout << "MAX(" << x << ", " << y << ") = " << MAX(x, y) << "\n";
}

void parenthesize_demo() {
  std::cout << "--- parenthesize arguments ---\n";
  // SQUARE(1 + 2) expands to ((1 + 2) * (1 + 2)) = 9.
  // Without the inner parens it would be 1 + 2 * 1 + 2 = 5.
  std::cout << "SQUARE(1 + 2) = " << SQUARE(1 + 2) << "\n";
}

void conditional_compile_demo() {
  std::cout << "--- conditional compilation ---\n";
#ifdef __linux__
  std::cout << "compiled on linux\n";
#elif defined(_WIN32)
  std::cout << "compiled on windows\n";
#elif defined(__APPLE__)
  std::cout << "compiled on apple\n";
#else
  std::cout << "compiled on unknown platform\n";
#endif

  std::cout << "LOGING = " << LOGING << "\n";
  LOG("this message only prints when LOGING == 1");
}

void type_safety_pitfall_demo() {
  std::cout << "--- type-safety pitfall ---\n";
  // Macros do no type checking. SQUARE works for any expression, even one
  // that mixes types in a way a templated/inline function would catch.
  double d = 1.5;
  std::cout << "SQUARE(1.5) = " << SQUARE(d) << "\n";
}

int main() {
  constants_demo();
  function_like_demo();
  parenthesize_demo();
  conditional_compile_demo();
  type_safety_pitfall_demo();
  return 0;
}
