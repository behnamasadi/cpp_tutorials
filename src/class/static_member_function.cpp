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

class Something {
private:
  static int s_nValue;

public:
  static int GetValue() { return s_nValue; }
};

int Something::s_nValue = 1; // initializer

int StaticMemberFunctionmain() { return Something::GetValue(); }

/*
Static Initialization Order Fiasco
The static initialization order fiasco refers to the ambiguity in the order that
objects with static storage duration in different translation units are
initialized in. If an object in one translation unit relies on an object in
another translation unit already being initialized, a crash can occur if the
compiler decides to initialize them in the wrong order.

Suppose you have two static objects x and y which exist in separate source
files, say x.cpp and y.cpp. Suppose further that the initialization for the y
object (typically the y object’s constructor) calls some method on the x object.

The tough part is that you have a 50%-50% chance of corrupting the program.
If the compilation unit for x.cpp happens to get initialized first, all is well.
But if the compilation unit for y.cpp get initialized first, then y’s
initialization will get run before x’s initialization, and you’re toast. E.g.,
y’s constructor could call a method on the x object, yet the x object hasn’t yet
been constructed.

Within a single translation unit, the fiasco does not apply because the objects
\ are initialized from top to bottom.

solutions:

https://isocpp.org/wiki/faq/ctors#static-init-order
*/

int main() {}
