#include <iostream>
#include <cassert> // for assert()

/*
//////////////////////////////////// assert ////////////////////////////////////

    An assert statement is a preprocessor macro that evaluates a conditional expression.
    If the conditional expression is true, the assert statement does nothing.
    assert will terminate the program (usually with a message quoting the assert statement) if its argument turns out
    to be false. It's commonly used during debugging to make the program fail more obviously if an unexpected condition occurs.

    When you're doing a release (non-debug) build, you can also remove the overhead of evaluating assert
    statements by defining the NDEBUG macro, usually with a compiler switch. The corollary of this is that
    your program  should never rely on the assert macro running.
*/

int assertExample()
{
    int nIndex=12;
    int g_anArray[10];
    // we're asserting that nIndex is between 0 and 9
    assert(nIndex >= 0 && nIndex <= 9); // this is line 7 in Test.cpp
    return g_anArray[nIndex];
}
/*
//////////////////////////////////// static_assert ////////////////////////////////////

Static assert is used to make assertions at compile time. When the static assertion fails,
the program simply doesn't compile. This is useful in different situations, like, for example,
if you implement some functionality by code that critically depends on unsigned int object
having exactly 32 bits. You can put a static assert like this

static_assert(sizeof(unsigned int) * CHAR_BIT == 32);
in your code. On another platform, with differently sized unsigned int type the compilation
 will fail, thus drawing attention of the developer to the problematic portion of the code and
advising them to re-implement or re-inspect it.

For another example, you might want to pass some integral value as a void * pointer to a function (a hack, but useful at times) and you want to make sure that the integral value will fit into the pointer

int i;

static_assert(sizeof(void *) >= sizeof i);
foo((void *) i);
You might want to asset that char type is signed

static_assert(CHAR_MIN < 0);
or that integral division with negative values rounds towards zero

static_assert(-5 / 2 == -2);
And so on.

Run-time assertions in many cases can be used instead of static assertions,
but run-time assertions only work at run-time and only when control passes over the assertion.
For this reason a failing run-time assertion may lay dormant, undetected for extended
periods of time.

Of course, the expression in static assertion has to be a compile-time constant.
It can't be a run-time value. For run-time values you have no other choice but use
the ordinary assert.


Also for checking against static member functions

*/
class Foo
{
    public:
        static const int bar = 5;//if you change into 3, you will get compiler error
};

int static_assertExample()
{
    static_assert(Foo::bar > 4, "Foo::bar is too small :(");
    return Foo::bar;
}


constexpr int  multiplyBy10(int x)
{
    return 10*x;
}

void static_assertExample2()
{
    const int a=5;
    const int result=multiplyBy10(a);
    const int expected=50;
    static_assert(expected==result);
}


int main(int argc, char *argv[])
{
    assertExample();
    return 0;
}
