

/*
preprocessor is a separate program that runs before the compiler when you compile your program.
It’s purpose is to process directives.
Directives are specific instructions that start with a # symbol and end with a newline (NOT a semicolon).

There are several different types of directives:
1)Includes
2)Macro defines
3)Conditional compilation
4)Header guards

The preprocessor manipulates text before the compiler gets to it.


For example, consider the following program:

add.h:
	#include "mymath.h"
	int add(int x, int y);

subtract.h:
	#include "mymath.h"
	int subtract(int x, int y);

main.cpp:
	#include "add.h"
	#include "subtract.h"
	
Consequently, all the contents of mymath.h will have been included twice in main.cpp, which will cause the compiler 
to complain.

solution:

#ifndef SOME_UNIQUE_NAME_HERE
#define SOME_UNIQUE_NAME_HERE
 
// your declarations here
 
#endif

for example in our case:
add.h

	#ifndef ADD_H
	#define ADD_H
	 
	// your declarations here
	 
	#endif

subtract.h

	#ifndef SUBTRACT_H
	#define SUBTRACT_H
	 
	// your declarations here
	 
	#endif

and because in math.h we have 

	#ifndef MATH_H
	#define MATH_H
	 
	// your declarations here
	 
	#endif
	
so after we include add.h, via add.h automatically we define MATH_H, so when we include subtract.h, 
MATH_H already defined so #ifndef MATH_H would return false and the content of math.h
won't included again 
*/

#include "add.h"
#include "subtract.h"

int main()
{
}
