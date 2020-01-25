#include <iostream>
/*

http://www.cplusplus.com/articles/2LywvCM9/

Calling a function has lot of overhead, calling, copying arguments, push/pop on the stack, and return.
Inline function will be added to your code so there is no call and return.
Use inline function over macros.

*/

class foo
{
 public:
    int add(int a, int b);
};
inline int foo::add(int a, int b)
{
   return (a + b);
}

int main(int argc, char *argv[])
{
    foo fooObj;
    std::cout<<fooObj.add(2,3)<<std::endl;
    return 0;
}
