/*
 * ScopeResolutionOperator.cpp
 *
 *  Created on: Mar 19, 2012
 *      Author: behnam
 */
#include <iostream>

int n = 12; // A global variable

int main() {
  int n = 13; // A local variable

  std::cout << ::n << '\n'; // Print the global variable: 12
  std::cout << n << '\n';   // Print the local variable: 13
}
