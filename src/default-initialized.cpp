#include <iostream>

class Foo {
public:
  int i;
};

int main() {

  Foo inst1;
  Foo *inst2 = new Foo;
  std::cout << inst1.i << std::endl;
  std::cout << inst2->i << std::endl;
}
