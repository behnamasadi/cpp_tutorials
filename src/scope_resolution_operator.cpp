/*
 * ScopeResolutionOperator.cpp
 *
 *  Created on: Mar 19, 2012
 *      Author: behnam
 */
#include <iostream>

// Without this using statement cout below would need to be std::cout
using namespace std;

int n = 12; // A global variable

int ScopeResolutionOperatormain()
//int main()
{
  int n = 13; // A local variable

  cout << ::n << '\n'; // Print the global variable: 12
  cout << n   << '\n'; // Print the local variable: 13
}



