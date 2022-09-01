#  Enum classes 
 Enum classes should be preferred. The enum classes ("new enums", "strong enums") address three problems with traditional C++ enumerations:
1. conventional enums implicitly convert to int, causing errors when someone does not want an enumeration to act as an integer.
2.conventional enums export their enumerators to the surrounding scope, causing name clashes.
3. the underlying type of an enum cannot be specified, causing confusion, compatibility problems, and makes forward declaration impossible.
 
 ```cpp
enum  color
{
    None = 0,
    red = 1,
    blue = 2,
    green = 4,
    All =  red| blue | green,
};
```

```cpp
enum fruit
{
    apple=1, 
    melon=2, 
    orange=3,
};
```

```cpp
enum  class animals: unsigned char
{chicken, aligator, turtle};
```

```cpp
enum class mamals
{cow, fox, dog};
```

in the main:

this is dangerous but it works:
```cpp
std::cout<<  (color::red==fruit::apple ? "equal":"not equal")   <<std::endl;
```
this won't complie
```cpp
std::cout<<  (animals::chicken==mamals::cow ? "equal":"not equal")   <<std::endl;
```

```cpp
std::cout<<  (animals::chicken==static_cast<animals>(0)? "equal":"not equal")   <<std::endl;
```

# Iterating over an enum
your enum:
```cpp
enum  color
{
	None = 0,
	red = 1,
	blue = 2,
	green = 4,
	All = red | blue | green,
};
```

iterator:
```cpp
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
```

consumer of enum

```cpp
void foo(color c) 
{
	std::cout <<c  << std::endl;
}

```
now in you  main

You'll need to specialize it
```cpp
typedef Iterator<color, color::None, color::All> colorIterator;
```

```cpp
for (color c : colorIterator())
{ 
  foo(c);
}
```

Refs: [1](https://stackoverflow.com/questions/261963/how-can-i-iterate-over-an-enum)



Automatically convert strongly typed enum into int

Refs: [1](https://stackoverflow.com/questions/8357240/how-to-automatically-convert-strongly-typed-enum-into-int)



