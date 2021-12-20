#include <iostream>


namespace virtual_destructor
{

class Base
{
    int * data;
public:
    Base()
    {
        std::cout<<"base constrcutor" <<std::endl;
        int size=10;
        data=new int[size];
    }
    ~Base()
    {
        std::cout<<"base destrcutor" <<std::endl;
        delete []data;
    }
};

class Derived: public Base
{
    double * points;
    public:
        Derived()
        {
            std::cout<<"derived constrcutor" <<std::endl;
            int size=5;
            points=new double[size];
        }
        ~Derived()
        {
            delete [] points;
            std::cout<<"derived destrcutor" <<std::endl;
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
        std::cout<<"=================== base =================== " <<std::endl;
        Base * objBase=new Base;
        delete objBase;
        std::cout<<"=================== derived =================== " <<std::endl;
        Derived * objDerived=new Derived;
        delete objDerived;
    }
}

