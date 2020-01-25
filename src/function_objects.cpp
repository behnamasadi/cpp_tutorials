/*

http://www.cprogramming.com/tutorial/functors-function-objects-in-c++.html


Function objects (aka "functors"), Functors are objects that can be treated as though they are a function or function pointer
You could write code that looks like this:


myFunctorClass functor;
functor( 1, 2, 3 );


This code works because C++ allows you to overload operator(), the "function call" operator. The function call operator can take any number of arguments of any types and return anything it wishes to.
For ease of discussion, when an object's operator() is invoked, I'll refer to it as "calling" the object as though it were a function being called.

*/


#include <iostream>
 
class myFunctorClass
{
    public:
        myFunctorClass (int x) : _x( x ) {}
        int operator() (int y) { return _x + y; }
    private:
        int _x;
};
 
int main()
{
    myFunctorClass addFive( 5 );
    std::cout << addFive( 6 );
 
    return 0;
} 
