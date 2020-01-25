#include "b.hpp"

void B::getName()
{
    std::cout<<"B" <<std::endl;
}

void B::memberName()
{
    m_a->getName();
}


