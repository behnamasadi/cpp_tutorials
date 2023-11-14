#include "classA.hpp"
#include "classB.hpp"
#include <iostream>

ClassA::ClassA() : b(nullptr) {}

void ClassA::setClassB(ClassB *b) { this->b = b; }

void ClassA::doSomethingWithB() {
  if (b) {
    // Assuming ClassB has a method exampleMethod() that we want to call
    std::cout << "ClassA is calling method from classB." << std::endl;
    b->exampleMethod();
  } else {
    std::cout << "ClassB instance is not set in ClassA." << std::endl;
  }
}

void ClassA::exampleMethod() {
  std::cout << "Method in ClassA called." << std::endl;
}
