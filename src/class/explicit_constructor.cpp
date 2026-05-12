// Explicit Constructor
//
// The compiler is allowed to make ONE implicit conversion to resolve the
// arguments of a function/constructor. A single-argument constructor that is
// not marked `explicit` therefore doubles as an implicit conversion from its
// parameter type. This is often surprising. The `explicit` keyword disables
// that conversion: the constructor can still be called, but only directly.

#include <iostream>

namespace Implicit {
class String {
public:
  // allocate n bytes to the String object
  String(int n) {
    std::cout << "  allocate " << n << " bytes to the String object"
              << std::endl;
  }

  // initialize object with char *p
  String(const char *p) {
    std::cout << "  initialize object with char *p" << std::endl;
  }
};
} // namespace Implicit

namespace Explicit {
class String {
public:
  // allocate n bytes
  explicit String(int n) {
    std::cout << "  allocate " << n << " bytes to the String object"
              << std::endl;
  }
  // initialize object with char *p
  String(const char *p) {
    std::cout << "  initialize object with char *p" << std::endl;
  }
};
} // namespace Explicit

int main() {
  std::cout << "--- Implicit conversion trap ---" << std::endl;
  // 'a' is a char, implicitly convertible to int (ASCII 97).
  // Copy-initialization picks String(int) and allocates 97 bytes,
  // which is almost certainly not what the caller meant.
  Implicit::String s1 = 'a';

  std::cout << "--- explicit disables the trap ---" << std::endl;
  // With `explicit String(int)`, the following line no longer compiles:
  //
  //     Explicit::String s2 = 'a';   // error: copy-init cannot use
  //                                  //        explicit constructor
  //
  // We must convert the char ourselves. Casting to const char* selects
  // the String(const char *) constructor instead.
  Explicit::String s2 = (const char *)'a';

  return 0;
}
