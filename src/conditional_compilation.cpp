/*
 * ConditionalCompilation.cpp
 *
 *  Created on: Jan 4, 2012
 *      Author: behnam
 */

#include <iostream>
using namespace std;

int main() {
#define PRINT_JOE

#ifdef PRINT_JOE
  cout << "Joe" << endl;
#endif

#ifdef PRINT_BOB
  cout << "Bob" << endl;
#endif

#define FOO 30

#if FOO > 5
  cout << "FOO is >5 " << endl;
#elif FOO == 30
  cout << "FOO == 30 " << endl;
#else
  cout << "FOO is something else " << endl;
#endif
  return 0;
}
