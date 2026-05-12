#include <stdio.h>

#include "foo.h"

// Defined in a .c file, compiled by the C compiler. Its symbol is plain
// "foo", with no C++ name mangling.
void foo(void) { printf("foo here (from C)\n"); }
