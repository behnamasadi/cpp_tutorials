#include "a.hpp"

void A::getName()
{
    std::cout<<"A" <<std::endl;
}

void A::memberName()
{
    m_b->getName();
}
