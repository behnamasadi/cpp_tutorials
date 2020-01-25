#include <iostream>
/*
RVOvs std::move
https://www.ibm.com/developerworks/community/blogs/5894415f-be62-4bc0-81c5-3956e82276f3/entry/RVO_V_S_std_move?lang=en

=================Named/ Return Value Optimization==============

We create temporary object in return and then we copy it so the compiler will
optimize it for us and we only get "constructor" in the output

If we disable this optimization by
    g++ -fno-elide-constructors  RVO_NRVO_copy_elision.cpp
Then you will get
"constructor"
"copy constructor"
"copy constructor"

=================Copy elision (Copy omission) ====================
it is a compiler optimization technique that avoids unnecessary copying of objects.

This line:
    string s="hello";

should be broken down by the compiler as

    string s=S("hello");

However, most of the C++ compilers avoid such overheads of creating a temporary
object and then copying it.
*/
struct S
{
    S() {std::cout<<"constructor" <<std::endl;}

    S(S const &rhs) {std::cout<<"copy constructor" <<std::endl;}
};

//Return Value Optimization
S RVO()
{
    return S();
}

//Named Return Value Optimization
S NRVO()
{   S s;
    return s;
}

struct string
{
    string(const char* str="\0") {std::cout<<"constructor" <<std::endl;}
    string (const string &rhs ){std::cout<<"copy constructor" <<std::endl;}
};

int main()
{
    S s1=RVO();
    S s2=NRVO();

    string s="hello";
}
