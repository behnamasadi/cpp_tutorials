#include <iostream>


/*
https://www.geeksforgeeks.org/virtual-function-cpp/

Very Important

1)Virtual functions are always defined in base class (declaration is not enough, you have to define them as well in the base) and (may or may not) overridden in derived class.
 
2)It is not mandatory for derived class to override (or re-define the virtual function), in that case base class version of function is used.

3)A class may have virtual destructor but it cannot have a virtual constructor.

4)A pure virtual function or pure virtual method is a virtual function that is required to be implemented by a derived class,
if that class is not abstract.
 
Here BaseAbstract is an abstract class, meaning it contains at least one pure virtual function.
You can't amke an object from it and you can only derive from it.
*/



namespace VirtualFunction
{
class base
{
public:

    //it is not possible to have virtual constructor!
    base()
    {
        std::cout<<"base constructor" <<std::endl;
    }
    //it is possible to have virtual destructor
    virtual  ~base()
    {
        std::cout<<"base destructor" <<std::endl;
    }
    // decration like void virtual foo(); is not enough, you have to fully define the function
    void virtual foo()
    {
        std::cout<<"foo base" <<std::endl;
    }

    //since we shadow the show() in the derived class we can declare it here without full definition
    void show();

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

    void show()
    {
        std::cout<<"show derived" <<std::endl;
    }

    derived()
    {
        std::cout<<"derived constructor" <<std::endl;
    }
    ~derived()
    {
        std::cout<<"derived destructor" <<std::endl;
    }
    //we shadow the print from base
    void print()
    {
        std::cout<<"print derived" <<std::endl;
    }


};

namespace  PureVirtual
{

class Base
{
public:
    void getname()
    {
        std::cout <<"Base" <<std::endl;
    }
    virtual void print() // a normal virtual
    {
        std::cout <<"Base print" <<std::endl;
    }
    //a pure virtual function
    virtual int GetValue() const  = 0; //   =0; is pure virtual in C++ which means that you have to implement those function in the child classes of
    virtual int GetValue(int i) = 0;
};

class Derived : public Base
{
public :
    void getname()
    {
        std::cout <<"Derived" <<std::endl;
    }

    void print()
    {
        std::cout <<"Derived print" <<std::endl;
    }

    int  GetValue() const
    {
        int  j=5;
        return j;
    }

    int GetValue(int i)
    {
        return 5;
    }

};

}

}
int main()
{
///////////////////////////////////////Virtual Function///////////////////////////////////

    {
        std::cout<<"Order of function calls:" <<std::endl;
        VirtualFunction::derived d1;
        //foo from derived will be called
        d1.foo();
        //since show() has been defined in the derived, we can call show
        d1.show();
        d1.print();
    }




///////////////////////////////////////Pure Virtual and Abstract Classes///////////////////////////////////

    {

    }



    return 0;
}
