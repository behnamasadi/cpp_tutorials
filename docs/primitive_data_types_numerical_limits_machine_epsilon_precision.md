- [Fundamental types categories in C++](#fundamental-types-categories-in-c--)
  * [integral](#integral)
  * [floating point](#floating-point)
  * [void](#void)
- [Numerical Limits and Precision](#numerical-limits-and-precision)
- [size of objects and types](#size-of-objects-and-types)
- [Setting Precision and Format in cout](#setting-precision-and-format-in-cout)
- [Variable Suffix](#variable-suffix)
- [decltype vs typeof](#decltype-vs-typeof)



# Fundamental types categories in C++ 

In C++, the fundamental types can be categorized into several groups based on their nature and usage:

1. **Character Types**:
   - `char`: Represents a single character. Can be used to store ASCII characters.
   - `wchar_t`: Wide character type, used for characters that cannot be represented by `char` (e.g., international characters).
   - `char16_t` (since C++11): For Unicode characters.
   - `char32_t` (since C++11): For wider Unicode characters.

2. **Integer Types**:
   - `short`: Short integer type.
   - `int`: Integer type, typically used for integer arithmetic.
   - `long`: Long integer type, larger than `int`.
   - `long long` (since C++11): Even larger integer type than `long`.

   Each of these integer types comes in two variants: signed (can hold both negative and positive values) and unsigned (can only hold positive values or zero).

3. **Floating Point Types**:
   - `float`: Single precision floating point type.
   - `double`: Double precision floating point type.
   - `long double`: Extended precision floating point type.

4. **Boolean Type**:
   - `bool`: Represents a boolean value (`true` or `false`).

5. **Void Type**:
   - `void`: Represents the absence of a type. Used in functions that do not return a value.

Each of these types can have several modifiers like `signed`, `unsigned`, `short`, `long` to alter their representation and range. The exact size and range of these types can vary based on the compiler and architecture, but the C++ standard provides minimum size requirements for each type.

## integral

integral Integral types are capable of handling whole numbers.  Floating point types are capable of specifying values that may have fractional parts..The category integral includes the following datatypes: `bool, char, char8_t (since C++20), char16_t, char32_t, wchar_t, short, int, long, long longt`.


```cpp
struct foo {};
std::cout << std::boolalpha <<
std::is_integral<foo>::value << ", " <<
std::is_integral_v<float> << ", " <<
std::is_integral_v<int> << ", " <<
std::is_integral_v<const int> << ", " <<
std::is_integral_v<bool> << ", " << std::endl;
```

the output is: 

```
false, false, true, true, true
```


## floating point

floating point. The category floating point includes the following datatypes: `float, double, long double`.

```cpp
std::cout <<std::boolalpha   <<
std::is_floating_point<float>::value <<", " <<
std::is_floating_point<float&>::value << ", " <<
std::is_floating_point<int>::value << std::endl;
```
the output is: 
```
true, false, false
````

we can use `std::is_arithmetic` for both  integral and floating point type:
```cpp
std::cout << std::boolalpha <<
std::is_arithmetic<int>::value << ", " <<
std::is_arithmetic<float>::value << ", " <<
std::is_arithmetic<char>::value << std::endl;
```
output is:

```
true, true, true
```
## void

The void type describes an empty set of values.  It is used in the followings situations:
-  Function returns as void :  functions  which do not return any value (they return void) has the return type as void. 
-  Function arguments as void :  functions which do not accept any parameter.
-  Pointers to void :   A pointer of type `void *` represents the address of an object, but not its type. For instance, a memory allocation function `void *malloc( size_t size);` returns a pointer to void which can be casted to any data type.


# Numerical Limits and Precision 

The maximum value for type float int is:  
```cpp
std::numeric_limits<float>::max( );
```
or

```cpp
std::cout<<"FLT_MAX: " <<FLT_MAX <<std::endl;
```


The minimum value for type float int is:
```cpp
std::numeric_limits<float>::min( )
```
or
```cpp
std::cout<<"FLT_MIN: " <<FLT_MIN <<std::endl;
```

The difference between 1 and the smallest value greater than 1 for float objects is:
```cpp
std::numeric_limits<float>::epsilon( )
```
or
```cpp
std::cout<<"FLT_EPSILON: " <<FLT_EPSILON <<std::endl;
```


|DATA TYPE    |SIZE (IN BYTES)     | RANGE  | 
|---|---|---| 
|signed char|                     1   |            -128 to 127   |
|unsigned char|                   1    |               0 to 255  |
|short int|                       2    |          -32,768 to 32,767|
|unsigned short int|              2    |               0 to 65,535|
|unsigned int|                    4    |           0 to 4,294,967,295|
|int  |                           4    |       -2,147,483,648 to 2,147,483,647|
|long int|                        4    |       -2,147,483,648 to 2,147,483,647|
|unsigned long int|               4    |       0 to 4,294,967,295|
|long long int|                   8    |           -(2^63) to (2^63)-1|
|unsigned long long int   |          8     |          0 to 18,446,744,073,709,551,615|
|float |                          4     |  |
|double|                          8     |  |
|long double|                     12    |   |
|wchar_t |                        2 or 4 |             1 wide character|



# size of objects and types
`sizeof` operator queries size of the object or type.

```cpp
struct Empty {};
struct Base { int a; };
struct Derived : Base { int b; };
```

size of primitive types in bytes
```cpp
std::cout<<sizeof(char) <<std::endl;

std::cout<<sizeof(std::byte) <<std::endl;
```

Most modern std::string implementations save very small strings directly on the stack in a statically sized char array instead of using dynamic heap storage. This is known as Small (or Short) String Optimisation (SSO) 

```cpp
std::cout<<sizeof(std::string("a")) <<std::endl;//32

```



size of empty class:
```cpp
std::cout<< sizeof(Empty)   <<std::endl;
```

size of pointer:
```cpp
std::cout << sizeof(Empty *) << std::endl;
```

size of Base:
```cpp
std::cout << sizeof(Base) << std::endl;
```

size of Derived:
```cpp
std::cout << sizeof(Derived) << std::endl;
```

size of array of 10 int:
```cpp
std::cout << sizeof(int[10]) << std::endl;
```

size of a array of 10 int:

```cpp
int a[10];
std::cout << sizeof(a) << std::endl;
```

length of array of 10 int:
```cpp
std::cout << ((sizeof a) / (sizeof *a)) << std::endl;
std::cout << ((sizeof a) / (sizeof a[0])) << std::endl;
```


for string `sizeof(string)` tells you the size of the pointer, so it should **NOT** be used.
for c strings `strlen` could be used:


```cpp
char my_str[100]="my string";
std::cout<<"size of string is: "<<strlen( my_str )<<" bytes and string is: "<< my_str <<std::endl;
```

`str.size()` also return the size of c++ strings.


# Setting Precision and Format in cout

It can be set with `std::cout.setf(arg1, arg2);` and `std::cout.precision(num);`


where `arg1` could be:

`std::ios::left, std::ios::right, std::ios::scientific, std::ios::fixed, std::ios::dec, std::ios::oct, std::ios::hex`

and `arg2` could be:

`std::ios::adjustfield, std::ios::floatfield, std::ios::basefield, std::ios::basefield`.


```cpp
double a;
a = 3.1415926534;
std::cout<<"a is: 3.1415926534" <<std::endl;

std::cout.setf(std::ios::scientific,std::ios::floatfield);
std::cout<<a<<std::endl;//3.140000e+00

std::cout.setf(std::ios::fixed,std::ios::floatfield);
std::cout<<a<<std::endl;//3.140000

std::cout.precision(1);
std::cout<<a<<std::endl;//3.1
```



# Variable Suffix
Your templated function:

```cpp
template<class T> T min(T a, T b)
{
  return (a < b) ? a : b;
}
```
your variables:
```cpp
25;     //int
25l;    //long
25u;   //unsigned
25.0;  //double
25.0f; //float
```

will compile
```cpp
float x = min(3.0f, 2.0f); // 
```
compiler cannot deduce T type
```cpp
x = min(3.0f, 2);
x = min(3.0f, 2.0);
```

# decltype vs typeof
There is no `typeof` operator in c++. although the functionality has been offered by most of compilers it is a compiler specific language extension and it is extremely platform dependent. Also `decltype` always preserves references as part of the information, whereas `typeof` may not:
```cpp
int a = 1;
int& ra = a;
typeof(a) b = 1;     // int
typeof(ra) b2 = 1;   // int
decltype(a) b3;      // int
decltype(ra) b4 = a; // reference to int
```



Refs: [1](https://stackoverflow.com/questions/14130774/difference-between-decltype-and-typeof)   
