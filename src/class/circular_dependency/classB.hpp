#ifndef CLASSB_H
#define CLASSB_H

#include "classA.hpp"

class ClassB {
public:
  ClassB();
  void setClassA(ClassA *a);
  void exampleMethod();
  void doSomethingWithA();

private:
  ClassA *a;
};

#endif // CLASSB_H
