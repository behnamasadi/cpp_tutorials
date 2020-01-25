/*
 * PassingArgToParentConstructor.cpp
 *
 *  Created on: Feb 8, 2012
 *      Author: behnam
 * In C++, whenever an object of a class is created, its constructor is called.
 * But that's not all--its parent class constructor is called
 * so if we need to send some parameter to initial parent we have to use:
 *  child_contructor: parent_constructor
 *  Bar() : Foo( 10 )
 *
 * http://codewrangler.home.comcast.net/~codewrangler/tech_info/ctor_init.html
 * http://www.cprogramming.com/tutorial/initialization-lists-c++.html
 */


#include <iostream>
using namespace std;
class Foo
{
        public:
        Foo( int x )
        {
                std::cout << "Foo's constructor "
                          << "called with "
                          << x
                          << std::endl;
        }
};

class Bar : public Foo
{
        public:
        Bar() : Foo( 10 )  // construct the Foo part of Bar
        {
                std::cout << "Bar's constructor" << std::endl;
        }
};
// int PassingArgToParentConstructormain()
int main()
{
        Bar stool;
}



