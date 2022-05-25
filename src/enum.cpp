/*
 * Enum.cpp
 *
 *  Created on: Jan 4, 2012
 *      Author: behnam
 */
#include <iostream>
using namespace std;



enum  color
{
    None = 0,
    red = 1,
    blue = 2,
    green = 4,
    All =  red| blue | green,
};

enum fruit
{
    apple=1, 
    melon=2, 
    orange=3,
    
};


enum  class animals: unsigned char
{chicken, aligator, turtle};

enum class mamals
{cow, fox, dog};


#include <type_traits>
template < typename C, C beginVal, C endVal>
class Iterator
{
	typedef typename std::underlying_type<C>::type val_t;
	int val;
public:
	Iterator(const C & f) : val(static_cast<val_t>(f)) {}
	Iterator() : val(static_cast<val_t>(beginVal)) {}
	Iterator operator++()
	{
		++val;
		return *this;
	}
	C operator*() { return static_cast<C>(val); }
	Iterator begin() { return *this; } //default ctor is good
	Iterator end()
	{
		static const Iterator endIter = ++Iterator(endVal); // cache it
		return endIter;
	}
	bool operator!=(const Iterator& i) { return val != i.val; }
};

void foo(color c) 
{
	std::cout <<c  << std::endl;
}

int main()
{
    //this is dangerous but it works:
     std::cout<<  (color::red==fruit::apple ? "equal":"not equal")   <<std::endl;

    //this won't complie
    // std::cout<<  (animals::chicken==mamals::cow ? "equal":"not equal")   <<std::endl;
    std::cout<<  (animals::chicken==static_cast<animals>(0)? "equal":"not equal")   <<std::endl;

   typedef Iterator<color, color::None, color::All> colorIterator;
   for (color c : colorIterator())
   { 
    foo(c);
   }

}


