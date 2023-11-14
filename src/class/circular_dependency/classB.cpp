#include "classB.hpp"
#include <iostream>

ClassB::ClassB() : a(nullptr) {}

void ClassB::exampleMethod() {
  std::cout << "Method in ClassB called." << std::endl;
}

void ClassB::doSomethingWithA() {
  std::cout << "ClassB is calling method from classA." << std::endl;
  a->exampleMethod();
}
void ClassB::setClassA(ClassA *a) { this->a = a; }
