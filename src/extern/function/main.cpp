// Demo: calling a C function from C++.
//
// C++ mangles function names to support overloading; C does not. If we
// declared foo() with normal C++ linkage, the linker would look for a
// mangled symbol that the C compiler never produced. 'extern "C"' tells
// the C++ compiler to use C linkage for these declarations so the names
// match.
//
// foo.h already wraps its declarations in extern "C" when __cplusplus is
// defined, so a plain #include is enough here.

#include <iostream>

#include "foo.h"

int main() {
  std::cout << "calling C function from C++:\n";
  foo();
}
