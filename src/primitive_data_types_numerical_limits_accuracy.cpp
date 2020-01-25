/*
 * Variables.cpp
 *
 *  Created on: Jan 4, 2012
 *      Author: behnam
 */
#include <iostream>
#include <math.h>
#include <limits>
#include <cfloat>

#include <typeinfo>
#include <iomanip> //for using setprecision(n)

//#include "type_name.hpp"
///////////////////////////////// type_name class /////////////////////////////////
#include <type_traits>
#include <typeinfo>
#ifndef _MSC_VER
#   include <cxxabi.h>
#endif
#include <memory>
#include <string>
#include <cstdlib>

/*
DATA TYPE               SIZE (IN BYTES)             RANGE
signed char                     1               -128 to 127
unsigned char                   1                   0 to 255
short int                       2              -32,768 to 32,767
unsigned short int              2                   0 to 65,535
unsigned int                    4               0 to 4,294,967,295
int                             4           -2,147,483,648 to 2,147,483,647
long int                        4           -2,147,483,648 to 2,147,483,647
unsigned long int               4           0 to 4,294,967,295
long long int                   8               -(2^63) to (2^63)-1
unsigned long long int          8               0 to 18,446,744,073,709,551,615
float                           4
double                          8
long double                     12
wchar_t                         2 or 4              1 wide character

*/



/*
==========================================Properties of integer types=========================================
CHAR_BIT - size of the char type in bits (it is not necessarily 8 bits)
SCHAR_MIN, SHRT_MIN, INT_MIN, LONG_MIN, LLONG_MIN(C99) - minimum possible value of signed integer types: signed char, signed short, signed int, signed long, signed long long
SCHAR_MAX, SHRT_MAX, INT_MAX, LONG_MAX, LLONG_MAX(C99) - maximum possible value of signed integer types: signed char, signed short, signed int, signed long, signed long long
UCHAR_MAX, USHRT_MAX, UINT_MAX, ULONG_MAX, ULLONG_MAX(C99) - maximum possible value of unsigned integer types: unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long
CHAR_MIN - minimum possible value of char
CHAR_MAX - maximum possible value of char
MB_LEN_MAX - maximum number of bytes in a multibyte character


============================================Properties of floating-point types==================================
#include <cfloat>
FLT_MIN, DBL_MIN, LDBL_MIN - minimum value of float, double, long double respectively
FLT_MAX, DBL_MAX, LDBL_MAX - maximum value of float, double, long double respectively
FLT_ROUNDS - rounding mode for floating-point operations
FLT_EVAL_METHOD - evaluation method of expressions involving different floating-point types (only available in C99)
FLT_RADIX - radix of the exponent in the floating-point types
FLT_DIG, DBL_DIG, LDBL_DIG - number of decimal digits that can be represented without losing precision by float, double, long double respectively
FLT_EPSILON, DBL_EPSILON, LDBL_EPSILON - difference between 1.0 and the next representable value of float, double, long double respectively
FLT_MANT_DIG, DBL_MANT_DIG, LDBL_MANT_DIG - number of FLT_RADIX-base digits in the floating-point mantissa for types float, double, long double respectively
FLT_MIN_EXP, DBL_MIN_EXP, LDBL_MIN_EXP - minimum negative integer such that FLT_RADIX raised to a power one less than that number is a normalized float, double, long double respectively
FLT_MIN_10_EXP, DBL_MIN_10_EXP, LDBL_MIN_10_EXP - minimum negative integer such that 10 raised to a power one less than that number is a normalized float, double, long double respectively
FLT_MAX_EXP, DBL_MAX_EXP, LDBL_MAX_EXP - maximum positive integer such that FLT_RADIX raised to a power one more than that number is a normalized float, double, long double respectively
FLT_MAX_10_EXP, DBL_MAX_10_EXP, LDBL_MAX_10_EXP - maximum positive integer such that 10 raised to a power one more than that number is a normalized float, double, long double respectively
DECIMAL_DIG - minimum number of decimal digits needed to represent all the significant digits for long double.[4] The value is at least 10. (only available in C99)
*/



void numericalLimitsPrecisionExample()
{
    std::cout << "The maximum value for type float int is:  " << std::numeric_limits<float>::max( ) << std::endl;
    std::cout<<"FLT_MAX: " <<FLT_MAX <<std::endl;

    std::cout << "The minimum value for type float int is:  " << std::numeric_limits<float>::min( ) << std::endl;
    std::cout<<"FLT_MIN: " <<FLT_MIN <<std::endl;

    std::cout << "The difference between 1 and the smallest value greater than 1 for float objects is: "   << std::numeric_limits<float>::epsilon( )     << std::endl;
    std::cout<<"FLT_EPSILON: " <<FLT_EPSILON <<std::endl;
}


void setPrescion()
{
    double a;
    a = 3.1415926534;
    std::cout<<"a is: 3.1415926534" <<std::endl;

    std::cout.setf(std::ios::scientific,std::ios::floatfield);
    std::cout<<a<<std::endl;//3.140000e+00

    std::cout.setf(std::ios::fixed,std::ios::floatfield);
    std::cout<<a<<std::endl;//3.140000

    std::cout.precision(1);
    std::cout<<a<<std::endl;//3.1
}

template<class T> T min(T a, T b)
{
  return (a < b) ? a : b;
}

void variableSuffix()
{
    //Suffix of “f” on float value
    25;     //int
    25l;    //long
    25u;   //unsigned
    25.0;  //double
    25.0f; //float
    float x = min(3.0f, 2.0f); // will compile
/*
    x = min(3.0f, 2);   // compiler cannot deduce T type
    x = min(3.0f, 2.0); // compiler cannot deduce T type
*/
}


void scientific_notation()
{

}


int main(int argc, char ** argv)
{

    //stackSize();
    //numericalLimitsPrecisionExample();
    setPrescion();


}

