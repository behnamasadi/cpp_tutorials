/*
 * Enum.cpp
 *
 *  Created on: Jan 4, 2012
 *      Author: behnam
 */
#include <iostream>
using namespace std;


/*
 Enum classes should be preferred. The enum classes ("new enums", "strong enums") address three problems with traditional C++ enumerations:

1)conventional enums implicitly convert to int, causing errors when someone does not want an enumeration to act as an integer.
2)conventional enums export their enumerators to the surrounding scope, causing name clashes.
3)the underlying type of an enum cannot be specified, causing confusion, compatibility problems, and makes forward declaration impossible.
 
 */
enum  color
{
    None = 0,
    red = 1,
    blue = 2,
    green = 4,
    All =  red| blue | green,
};

enum fruit
{
    apple=1, 
    melon=2, 
    orange=3,
    
};


enum  class animals: unsigned char
{chicken, aligator, turtle};

enum class mamals
{cow, fox, dog};





int main()
{
    //this is dangerous but it works:
     std::cout<<  (color::red==fruit::apple ? "equal":"not equal")   <<std::endl;

    //this won't complie
    // std::cout<<  (animals::chicken==mamals::cow ? "equal":"not equal")   <<std::endl;
     
     
     
    std::cout<<  (animals::chicken==static_cast<animals>(0)? "equal":"not equal")   <<std::endl;


}


