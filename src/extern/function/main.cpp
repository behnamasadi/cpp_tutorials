/*
c++ mangle the name of functions so it will be able to overload function, while in "c",
there is no overloading and function will have the same name in object file as what they had in their
respective "c" file, so if we want to compile a "c" function in c++, we have to use it extern

*/
extern  "C"
{
#include "foo.h"
}

int main()
{
    foo();
}
