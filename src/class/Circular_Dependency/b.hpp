#ifndef B_H
#define B_H

#include "a.hpp"
#include <iostream>

class A;

class B
{
public:
    A* m_a;
    void getName();
    void memberName();

};
#endif
