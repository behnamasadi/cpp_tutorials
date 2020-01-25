#include "a.hpp"
#include "b.hpp"

int main()
{
    A a;
    B b;

    a.m_b=&b;
    b.m_a=&a;

    a.getName();
    a.memberName();

    b.getName();
    b.memberName();

}
