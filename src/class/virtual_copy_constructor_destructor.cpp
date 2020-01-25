#include <iostream>
//Virtual Copy Constructor
/*
We can't make the constructor of a class virtuall,
compiler must be aware of the class type to create the object.
If we make constructor virtual, compiler flags an error. In fact
except inline, no other keyword is allowed in the declaration of constructor.
*/

/*==================================Virtual Destructor==================================
if we don't make the destructor virtual in the folloing example, we only call the destructor of Base and not
Dervied class so we will have memory leaking.

*/

namespace virtual_destructor
{

class Base
{

    int * data;
public:
    Base(int size): data(new int[size]){}
    virtual ~Base()
    {
        std::cout<<"base destrcutor" <<std::endl;
        delete []data;
    }
};

class Derived1:public Base
{

    double * points;
    public:
        Derived1(int size):Base(size)
        {
            points=new double[size];
        }
        virtual ~Derived1()
        {
            delete [] points;
            std::cout<<"derived1 destrcutor" <<std::endl;
        }
};

}

namespace virtual_constructor {
//https://www.geeksforgeeks.org/advanced-c-virtual-constructor/
}

namespace virtual_copy_constructor
{
//https://www.geeksforgeeks.org/advanced-c-virtual-copy-constructor/

}

int main()
{
    {
        using namespace   virtual_destructor;
        Base *b2=new Derived1(10);
        delete b2;
    }
}

