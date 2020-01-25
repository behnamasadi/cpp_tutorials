/*
 * StaticMemberFunction.cpp
 *
 *  Created on: May 30, 2012
 *      Author: behnam
 */


/*
 Because static member functions are not attached to a particular object,
 they can be called directly by using the class name and the scope operator.

static member functions can only access static member variables.
They can not access non-static member variables.

 */
#include <iostream>

class Something
{
private:
    static int s_nValue;
public:
    static int GetValue() { return s_nValue; }
};

int Something::s_nValue = 1; // initializer


int StaticMemberFunctionmain()
{
    std::cout << Something::GetValue() << std::endl;
}

