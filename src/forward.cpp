#include <iostream>

/*

std::forward has a single use case: to cast a templated function parameter (inside the function)
to the value category (lvalue or rvalue) the caller used to pass it.

std::move takes an object and allows you to treat it as a temporary (an rvalue). Although it isn’t a
semantic requirement, typically a function accepting a reference to an rvalue will invalidate it.
When you see std::move, it indicates that the value of the object should not be used afterwards,
but you can still assign a new value and continue using it.


*/


//https://www.youtube.com/watch?v=0xcCNnWEMgs&t=40s
struct S
{

};

void f(S& s)
{
    std::cout<<"S&" <<std::endl;
}

void f(S&& s)
{
    std::cout<<"S&&" <<std::endl;
}

template< typename T>
void wrap(T t)
{
    f(t);
}


template< typename T>
void wrap_forward(T t)
{
    f(std::forward<T>(t));
}

int main()
{
    S s;
    //this will call lvalue function of f()
    f(s);
    //this will call rvalue function of f()
    f(S());

    //now if we have wrapper class for f() and we call he following we get lvalue which is wrong
    wrap(S());

    //so if we use forward we will get the rvalue f() whih is the correct one
    wrap_forward(S());
}



