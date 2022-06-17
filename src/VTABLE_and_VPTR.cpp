#include <iostream>


class base
{
public:

    void virtual foo()
    {
        std::cout<<"foo from base" <<std::endl;
    }

    void virtual bar()
    {
        std::cout<<"bar from base" <<std::endl;
    }

    void print()
    {
        std::cout<<"print from base" <<std::endl;
    }

};


class derived : public base
{
public:

    void foo() override
    {
        std::cout<<"foo from derived" <<std::endl;
    }
    void print()
    {
        std::cout<<"print from derived" <<std::endl;
    }
};


int main()
{
    base *bptr;
    derived derived_object;
    bptr = &derived_object;

    //virtual function, binded at runtime, we got the foo() from derived
    bptr->foo();

    // Non-virtual function, binded at compile time, we get print() from base
    bptr->print();

    // shadow method, 
    derived_object.print();

}
