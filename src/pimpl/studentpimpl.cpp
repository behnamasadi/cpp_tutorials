#include "studentpimpl.hpp"


class Studentpimpl::Impl
{
    public:
        Impl (int id, std::string name):ID(id),name(name){}
        int ID;
        std::string name;
};

Studentpimpl::Studentpimpl(int id, std::string name):pimpl(new Studentpimpl::Impl(id,name) ){}
Studentpimpl::Studentpimpl(){}

std::string Studentpimpl::getName()
{
    return pimpl->name;
}
int Studentpimpl::getID()
{
    return pimpl->ID;
}
void Studentpimpl::setName(std::string name)
{
    pimpl->name=name;
}
void Studentpimpl::setID(int ID)
{
    pimpl->ID=ID;
}


Studentpimpl::~Studentpimpl()=default;
 
//copy constructor
Studentpimpl::Studentpimpl(const Studentpimpl &other)
{
    pimpl.reset(new Impl(*other.pimpl));
}

//assignment operator
Studentpimpl& Studentpimpl::operator=(Studentpimpl  &rhs)
{
    std::swap(pimpl,rhs.pimpl);
    return *this;
}


