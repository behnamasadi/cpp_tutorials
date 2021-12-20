#include <iostream>
#include <vector>
#include <algorithm>

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
    virtual ~Base()
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
int main ()
{
//    std::cout<<"=================== base =================== " <<std::endl;
//    Base * objBase=new Base;
//    delete objBase;
//    std::cout<<"=================== derived =================== " <<std::endl;
//    Derived * objDerived=new Derived;
//    delete objDerived;

    Base * objPolymorphic=new Derived;
    delete objPolymorphic;
}
