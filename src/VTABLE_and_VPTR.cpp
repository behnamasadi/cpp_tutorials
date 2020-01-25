#include <iostream>
/*
https://www.geeksforgeeks.org/virtual-functions-and-runtime-polymorphism-in-c-set-1-introduction/
https://www.geeksforgeeks.org/virtual-function-cpp/
https://www.learncpp.com/cpp-tutorial/125-the-virtual-table/
*/

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

/*
Explanation:
Late binding(Runtime) is done in accordance with the content of pointer (i.e. location pointed to by pointer)
and Early binding(Compile time) is done according to the type of pointer, since print() function is declared
with virtual keyword so it will be bound at run-time.

vtable
A table of only "virtual function" pointers, maintained per class.

vptr
A pointer to vtable, maintained per object instance. When an object is created, a pointer to vtable,
called the virtual table pointer, vpointer or VPTR, is added as a hidden member of this object.
As such, the compiler must also generate "hidden" code in the constructors of each class to initialize
a new object's virtual table pointer to the address of its class's virtual method table.




   ____________________                                              _____________________________________
   |      Base         |                                             |   vtable for class base            |
   |  _vptr------------|-------------------------------------------->|                                    |
   | virtual foo()<--- |---------------------------------------------|--address of base version of foo()  |
---|->virtual func1()<-|---------------------------------------------|--address of base version of func1()|
|  |___________________|                                             |____________________________________|
|
|
|
|
|
|   ____________________                                             ______________________________________
|   |      Derived     |                                             |   vtable for class derived          |
|   |  _vptr-----------|-------------------------------------------->|                                     |
|   | virtual foo()<---|---------------------------------------------|--address of derived version of foo()|
|   |                  |                                      |------|--address of base version of func1() |
|   |__________________|                                      |      |_____________________________________|
|                                                             |
|-------------------------------------------------------------|
*/
}
