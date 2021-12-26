#include<iostream>
#include <limits>

#ifndef _MSC_VER
#   include <cxxabi.h>
#endif

#include <memory>
#include <cstdlib>
#include <vector>

template <class T> std::string type_name()
{
    typedef typename std::remove_reference<T>::type TR;
    std::unique_ptr<char, void(*)(void*)> own
           (
#ifndef _MSC_VER
                abi::__cxa_demangle(typeid(TR).name(), nullptr,
                                           nullptr, nullptr),
#else
                nullptr,
#endif
                std::free
           );
    std::string r = own != nullptr ? own.get() : typeid(TR).name();
    if (std::is_const<TR>::value)
        r += " const";
    if (std::is_volatile<TR>::value)
        r += " volatile";
    if (std::is_lvalue_reference<T>::value)
        r += "&";
    else if (std::is_rvalue_reference<T>::value)
        r += "&&";
    return r;
}



#if defined(__GNUC__) || defined(__GNUG__)
template <typename T>
void variable_type_info(T var_type)
{
    std::cout << "size of " <<type_name<decltype(var_type)>()
                << " is "<< sizeof(__typeof__(var_type) )<<" byte(s) "
                <<"and the value range is "<< std::numeric_limits<__typeof__(var_type) >::min()
               <<","<<   std::numeric_limits<__typeof__(var_type) >::max() << std::endl;
}
#endif


void typeidExample()
{
    short int short_int;
    std::cout <<typeid(short_int).name()<<std::endl;

    int integer;
    std::cout <<typeid(integer).name()<<std::endl;
}

/*

How to create a variable nased on the type of an other variable:
1) __typeof__
2) decltype
There is no typeof operator in c++, it has always been a compiler specific language extension
and it's pretty much obsolete. One should use decltype instead.
*/
#if defined(__GNUC__) || defined(__GNUG__)

int typeofExample()
{
    int a=10;
    __typeof__(a) b=a;//equal to int _a=a
    b=12;
    std::cout<<b<<std::endl;
    return 0;
}
#endif

void decltypeExample()
{
    int i = 5;
    decltype(i) j = i * 2;
    std::cout<<j<<std::endl;
}


//determine the type of an array element
void decltypeArrayExample()
{
    int x[]={1,2,3};

    //First method
    using arrElemType = std::remove_reference<decltype( *x )>::type;

    //Secodn Method
    //typedef std::remove_reference<decltype( *x )>::type arrElemType;
    std::vector<arrElemType> z(std::begin(x),std::end(x));
}

#if defined(__GNUC__) || defined(__GNUG__)
void cTypeVariableSize()
{
//Integers

    char c=0;
    variable_type_info(c);

    unsigned char uc=0;
    variable_type_info(uc);

    short int si=0;
    variable_type_info(si);

    unsigned short int usi=0;
    variable_type_info(usi);

    int i=0;
    variable_type_info(i);

    unsigned int ui=0;
    variable_type_info(ui);

    long int li=0;
    variable_type_info(li);

    unsigned long int uli=0;
    variable_type_info(uli);

//Float
    float f=0;
    variable_type_info(f);

    double d=0;
    variable_type_info(d);

    long double ld=0;
    variable_type_info(ld);
}
#endif

int main()
{
    cTypeVariableSize();
//    typeidExample();
//    typeofExample();
}
