#ifndef A_H
#define A_H

#include <iostream>
#include "b.hpp"

class B;
class A
{
public:
    B *m_b;
    void getName();
    void memberName();
};
#endif
