- [Size and range of primitive data types](#size-and-range-of-primitive-data-types)
- [Numerical Limits and Precision](#numerical-limits-and-precision)
- [size of objects and types](#size-of-objects-and-types)
- [Setting Precision](#setting-precision)
- [Variable Suffix](#variable-suffix)
- [Scientific Notation](#scientific-notation)
- [decltype vs typeof](#decltype-vs-typeof)


# Size and range of primitive data types


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
# size of objects and types
sizeof operator queries size of the object or type.

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



# Setting Precision
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



# Scientific Notation
Code: [1](src/data_type_info.cpp), [2](src/primitive_data_types_numerical_limits_accuracy.cpp)
   
   
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
