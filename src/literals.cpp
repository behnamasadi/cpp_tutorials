#include <iostream>
#include <string>

/*
Literal are constants. C++ has 4 kind of literals:

1)integer:
1)floating pint
3)char
4)string

unsigned int ui=20u;
int i=20;
long int li=20l;
long double height=3.4;//meter? cm or inch?

*/

//User Defined Literals 
long double operator "" _cm(long double x){return x*10;}
long double operator "" _m(long double x){return x*1000;}
long double operator "" _mm(long double x){return x;}



void userDefinedLiteralsExample()
{

    long double height=3.4_cm;
    std::cout<<height <<std::endl;
    std::cout<<height+1.2_m <<std::endl;
}

int main()
{
    userDefinedLiteralsExample();
}
