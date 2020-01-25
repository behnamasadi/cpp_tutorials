/*
 * Extern.cpp
 *
 *  Created on: Feb 22, 2012
 *      Author: behnam
 */
#include <iostream>
/*http://www.learncpp.com/cpp-tutorial/42-global-variables/
 * Variables declared outside of a block are called global variables. Global variables have program scope,
 * which means they can be accessed everywhere in the program, and they are only destroyed when the program ends.

int g_nX; // global variable

int main()
{
    int nY; // local variable nY

    // global vars can be seen everywhere in program
    // so we can change their values here
    g_nX = 5;
} // nY is destroyed here

 */

/*
In order to use a global variable that has been declared in another file, you have to use a forward declaration or
a header file, along with the extern keyword.


global.cpp:
	// declaration of g_nValue
	int g_nValue = 5;

main.cpp:
	// extern tells the compiler this variable is declared elsewhere
	extern int g_nValue;

	int main()
	{
		g_nValue = 7;
		return 0;
	}


_________________________________________________________________________________________________________________

Here is an example of using a header file extern:

global.cpp:

	// declaration of g_nValue
	int g_nValue = 5;

global.h:
	#ifndef GLOBAL_H // header guards
	#define GLOBAL_H

	// extern tells the compiler this variable is declared elsewhere
	extern int g_nValue;

	#endif

main.cpp:
	#include "global.h"
	int main()
	{
		g_nValue = 7;
		return 0;
	}


 */


using namespace std;

int Externmain()
//in main()
{
	return 0;
}



