#include <iostream>

// Demo 1: #ifdef / #else / #endif
// A macro is either defined or not. The body compiles only when defined.
void demo_ifdef() {
  std::cout << "--- demo_ifdef (#ifdef / #else / #endif) ---\n";
#define PRINT_JOE

#ifdef PRINT_JOE
  std::cout << "PRINT_JOE is defined\n";
#else
  std::cout << "PRINT_JOE is not defined\n";
#endif

#ifdef PRINT_BOB
  std::cout << "PRINT_BOB is defined\n";
#else
  std::cout << "PRINT_BOB is not defined\n";
#endif
}

// Demo 2: #if VALUE == N / #elif / #else
// When a macro has a value, compare it with #if and branch with #elif.
void demo_if_value() {
  std::cout << "--- demo_if_value (#if / #elif / #else) ---\n";
#define FOO 30

#if FOO == 1
  std::cout << "FOO == 1\n";
#elif FOO == 30
  std::cout << "FOO == 30\n";
#else
  std::cout << "FOO is something else\n";
#endif
}

// Demo 3: macro from CMake (target_compile_definitions)
// CMake passes -DMY_FEATURE=1, so we both test "defined" and its value.
void demo_cmake_macro() {
  std::cout << "--- demo_cmake_macro (macro from CMake) ---\n";
#ifdef MY_FEATURE
#if MY_FEATURE == 1
  std::cout << "Feature is enabled.\n";
#else
  std::cout << "Feature is disabled.\n";
#endif
#else
  std::cout << "MY_FEATURE is not defined.\n";
#endif
}

// Demo 4: macro tied to CMAKE_BUILD_TYPE (DEBUG_FEATURE in Debug builds)
void demo_debug_feature() {
  std::cout << "--- demo_debug_feature (build-type-driven macro) ---\n";
#ifdef DEBUG_FEATURE
  std::cout << "Debug feature is active.\n";
#else
  std::cout << "Debug feature is not active.\n";
#endif
}

int main() {
  demo_ifdef();
  demo_if_value();
  demo_cmake_macro();
  demo_debug_feature();
  return 0;
}
