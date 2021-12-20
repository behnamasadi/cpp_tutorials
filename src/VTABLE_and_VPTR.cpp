#include <iostream>


class base
{
public:

    void virtual foo()
    {
        std::cout<<"foo base" <<std::endl;
    }

    void virtual func1()
    {
        std::cout<<"func1 base" <<std::endl;
    }

    void print()
    {
        std::cout<<"print base" <<std::endl;
    }

};


class derived : public base
{
public:

    void foo() override
    {
        std::cout<<"foo derived" <<std::endl;
    }
    void print()
    {
        std::cout<<"print derived" <<std::endl;
    }
};


int main()
{
    base *bptr;
    derived d;
    bptr = &d;

    //virtual function, binded at runtime, we got the foo() from derived
    bptr->foo();

    // Non-virtual function, binded at compile time, we get print() from base
    bptr->print();

    //we get print() from derived
    d.print();


}
