#include "classA.hpp"
#include "classB.hpp"

int main() {
  ClassA a;
  ClassB b;
  a.setClassB(&b);
  a.doSomethingWithB();

  b.setClassA(&a);
  b.doSomethingWithA();
}
