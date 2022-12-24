#include <iostream>

namespace Implicit {
class String {
public:
  // allocate n bytes to the String object
  String(int n) {
    std::cout << "allocate " << n << " bytes to the String object" << std::endl;
  }

  // initializes object with char *p
  String(const char *p) {
    std::cout << "initializes object with char *p" << std::endl;
  }
};

} // namespace Implicit

namespace Explicit {

class String {
public:
  // allocate n bytes
  explicit String(int n) {
    std::cout << "allocate " << n << " bytes to the String object" << std::endl;
  }
  // initialize sobject with string p
  String(const char *p) {
    std::cout << "initializes object with char *p" << std::endl;
  }
};

} // namespace Explicit

int main() {
  // This is actually possible because Entity has a constructor that can get
  // only an integer or a string
  Implicit::String s1 = 'a';

  // explicit keyword will disable the above conversion, so we should do it by
  // casting
  Explicit::String s2 = (const char *)'a';
}
