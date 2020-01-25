#include <iostream>

/*
 If a method in the derived class has the same name as base, we have shdowed that method from base class.
 Here we have shadowed info()
 If we need to overload a function from base class we need to use scope resolution operator ::
 Here we have overloader print()
*/

class base
{
public:
    void info()
    {
        std::cout<<"info from base" <<std::endl;
    }
    void print()
    {
        std::cout<<"print from base" <<std::endl;
    }
};


class derived:public base
{
public:

    void info()
    {
        std::cout<<"info from derived" <<std::endl;

    }

    void print(int id)
    {
        std::cout<<"print from derived, the given id is: "<<id <<std::endl;
    }

    void print()
    {
        std::cout<<"the print func from derived"<<std::endl;
    }

};


int main()
{
    base baseObject;
    derived derivedObject;

    std::cout<<"===========info() from base===========" <<std::endl;
    baseObject.info();
    std::cout<<"===========print() from base===========" <<std::endl;
    baseObject.print();
    std::cout<<"===========info() in the drived class has shadowed the base info()===========" <<std::endl;
    derivedObject.info();
    std::cout<<"===========calling the shadowed method (print() from base) in derived class===========" <<std::endl;
    derivedObject.base::print();
    std::cout<<"===========calling the print() which shadows print() from base class===========" <<std::endl;
    derivedObject.print();
    std::cout<<"===========calling the print() which overloads the print() from base class===========" <<std::endl;
    derivedObject.print(10);

}
