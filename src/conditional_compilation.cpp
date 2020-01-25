/*
 * ConditionalCompilation.cpp
 *
 *  Created on: Jan 4, 2012
 *      Author: behnam
 */

//Please have look at preprocessor.txt

#include <iostream>
using namespace std;

int ConditionalCompilation()
{
	#define PRINT_JOE

	#ifdef PRINT_JOE
	cout << "Joe" << endl;
	#endif

	#ifdef PRINT_BOB
	cout << "Bob" << endl;
	#endif
	return 0;
}



