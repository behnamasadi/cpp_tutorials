#include <iostream>

class foo {
public:
  int add(int a, int b);
};
inline int foo::add(int a, int b) { return (a + b); }

int main(int argc, char *argv[]) {
  foo fooObj;
  std::cout << fooObj.add(2, 3) << std::endl;
  return 0;
}
