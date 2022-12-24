#ifndef A_H
#define A_H

#include "b.hpp"
#include <iostream>

class B;
class A {
public:
  B *m_b;
  void getName();
  void memberName();
};
#endif
