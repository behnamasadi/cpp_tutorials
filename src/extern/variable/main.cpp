// Demo: using a global variable defined in another file via 'extern'.
//
//   global.cpp : int g_value = 10;        // definition
//   global.h   : extern int g_value;      // declaration
//   main.cpp   : includes global.h and uses g_value
//
// The linker resolves the reference to the definition in global.cpp.

#include <iostream>

#include "global.h"

int main() {
  std::cout << "initial g_value: " << g_value << '\n';

  g_value = 7;
  std::cout << "after write:     " << g_value << '\n';
}
