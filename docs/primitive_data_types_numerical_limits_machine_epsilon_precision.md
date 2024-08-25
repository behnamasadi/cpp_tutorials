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




# Machine Precision

Machine precision in the context of C++ programming (or any other programming language) refers to the degree of accuracy and reliability with which a computer can represent and manipulate real (floating-point) numbers. Here's a breakdown of the key aspects:

1. **Floating-Point Representation**: In C++, floating-point numbers are typically represented using the `float`, `double`, and `long double` data types. These types are based on the IEEE 754 standard for floating-point arithmetic.

2. **Precision**: The term "precision" refers to the number of digits that can be represented. In C++, `float` typically offers single precision (around 7 decimal digits), `double` offers double precision (about 15 decimal digits), and `long double` can offer extended precision (usually more than `double`, but this is platform-dependent).

3. **Limits**: The precision of these floating-point types is not just about the number of decimal places; it's also about the range of values they can represent. C++ provides header files like `<limits>` and `<cfloat>` which can be used to determine the properties of these types, such as their maximum and minimum representable values.

4. **Machine Epsilon**: This is a critical concept in understanding machine precision. It is the smallest positive number such that `1.0 + epsilon` is not equal to `1.0` when using a particular floating-point representation. In simple terms, it gives an idea of how small a difference the floating-point type can detect.

5. **Issues with Precision**: Floating-point numbers cannot represent all real numbers precisely due to their finite precision. This can lead to rounding errors and issues like loss of significance, especially when performing arithmetic operations on numbers of vastly different magnitudes.

6. **Determining Precision**: To find out the precision details of floating-point types in a particular C++ environment, one can use functions like `std::numeric_limits<float>::digits10` or `std::numeric_limits<double>::epsilon()`.

7. **Handling Precision Issues**: Programmers often need to be mindful of precision issues. This involves strategies like avoiding equality comparisons between floating-point numbers, being cautious with operations that can amplify rounding errors, and sometimes using higher-precision types or specialized libraries for numerical computations.

# Machine Epsilon
Machine epsilon is a fundamental concept in numerical computing, representing the smallest difference between two representable floating-point numbers. In other words, it's the smallest positive number that, when added to 1.0, yields a different number in a given floating-point format. Understanding machine epsilon helps in appreciating the limitations and characteristics of floating-point arithmetic.

### More on Machine Epsilon

1. **Definition**: Machine epsilon, often denoted as ε, is the upper bound on the relative error due to rounding in floating-point arithmetic.

2. **Significance**: Machine epsilon gives an idea of how much precision is lost during floating-point calculations. It is crucial for tasks that require high precision, such as scientific computations, where small errors can accumulate and lead to significant inaccuracies.

3. **Different for Each Data Type**: In C++, the machine epsilon varies for `float`, `double`, and `long double`. Generally, `double` has a smaller epsilon than `float`, indicating higher precision.

4. **Usage**: Knowing the machine epsilon is important when comparing floating-point numbers. Since direct equality checks (`==`) can be unreliable due to precision issues, epsilon is used to check if numbers are "close enough" to be considered equal.

### C++ Examples

#### Example 1: Finding Machine Epsilon for `float` and `double`

```cpp
#include <iostream>
#include <cfloat>

int main() {
    // Machine epsilon for float
    float epsilon_float = 1.0f;
    while ((1.0f + (epsilon_float / 2.0f)) != 1.0f) {
        epsilon_float /= 2.0f;
    }

    // Machine epsilon for double
    double epsilon_double = 1.0;
    while ((1.0 + (epsilon_double / 2.0)) != 1.0) {
        epsilon_double /= 2.0;
    }

    std::cout << "Machine epsilon for float: " << epsilon_float << std::endl;
    std::cout << "Machine epsilon for double: " << epsilon_double << std::endl;

    return 0;
}
```

This code calculates the machine epsilon for `float` and `double` by continuously halving a number until adding it to 1.0 no longer changes the result.

#### Example 2: Using Epsilon for Floating-Point Comparisons

```cpp
#include <iostream>
#include <cmath>

bool areAlmostEqual(double a, double b, double epsilon) {
    return std::fabs(a - b) < epsilon;
}

int main() {
    double num1 = 0.3 * 3 + 0.1;
    double num2 = 1.0;

    // Comparing using a defined epsilon
    double epsilon = 1e-9; // You can choose an appropriate epsilon value
    if (areAlmostEqual(num1, num2, epsilon)) {
        std::cout << "Numbers are almost equal." << std::endl;
    } else {
        std::cout << "Numbers are not equal." << std::endl;
    }

    return 0;
}
```

C++ provides a standard way to find the machine epsilon without manually calculating it as shown in the earlier examples. This can be done using the `std::numeric_limits` template class found in the `<limits>` header. This class contains various properties of arithmetic types, including their epsilon values.

Here's how you can use `std::numeric_limits` to find the machine epsilon for `float`, `double`, and `long double`:

```cpp
#include <iostream>
#include <limits>

int main() {
    // Machine epsilon for float
    float epsilon_float = std::numeric_limits<float>::epsilon();
    std::cout << "Machine epsilon for float: " << epsilon_float << std::endl;

    // Machine epsilon for double
    double epsilon_double = std::numeric_limits<double>::epsilon();
    std::cout << "Machine epsilon for double: " << epsilon_double << std::endl;

    // Machine epsilon for long double
    long double epsilon_long_double = std::numeric_limits<long double>::epsilon();
    std::cout << "Machine epsilon for long double: " << epsilon_long_double << std::endl;

    return 0;
}
```
## std::nan

`std::nan` is a C++ standard library constant that represents a Not-a-Number (NaN) value. A NaN is a special and **only** floating-point value that indicates an undefined or invalid result from a numerical operation.

**Key characteristics of `std::nan`:**

- **Not-a-Number:** It's not a valid number, but a special value used to signify undefined or exceptional results.
- **Comparison:** `std::nan` always compares unequal to itself and any other floating-point value, including other NaNs. This is because NaNs are inherently indeterminate.
- **Arithmetic operations:** Operations involving `std::nan` typically result in `std::nan`. For example, adding, subtracting, multiplying, or dividing any number by `std::nan` yields `std::nan`.
- **Comparison with NaN:** To check if a floating-point value is a NaN, you can use the `std::isnan()` function.

**When is `std::nan` used?**

`std::nan` is commonly used in various scenarios where undefined or exceptional results can occur:

- **Division by zero:** Dividing a non-zero number by zero results in `std::nan`.
- **Square root of a negative number:** Taking the square root of a negative number yields `std::nan`.
- **Mathematical operations with invalid inputs:** Certain mathematical functions (e.g., `log`, `asin`, `acos`) may return `std::nan` for invalid inputs.
- **Error handling:** `std::nan` can be used to indicate errors or invalid results in calculations.

**Example:**

```cpp
#include <iostream>
#include <cmath>

int main() {
    double result = 0.0 / 0.0; // Division by zero results in NaN

    if (std::isnan(result)) {
        std::cout << "Result is NaN." << std::endl;
    } else {
        std::cout << "Result is not NaN." << std::endl;
    }

    return 0;
}
```

This code will output:

```
Result is NaN.
```



The `const char* arg` parameter in the `std::nan` function is used to provide a **payload** or **diagnostic message** associated with the NaN value. This payload can be helpful for debugging or understanding the cause of the NaN.

**Purpose of the payload:**

- **Debugging:** The payload can provide additional context or information about the circumstances that led to the NaN, making it easier to identify and fix the underlying issue.
- **Tracing:** The payload can be used to trace the origin of the NaN through a program's execution, helping to isolate the specific part of the code where it occurred.
- **Customization:** The payload can be customized to include specific details relevant to the application or domain, providing more meaningful information.

**Example:**

```cpp
#include <iostream>
#include <cmath>

int main() {
    double result = 0.0 / 0.0; // Division by zero results in NaN
    std::cout << "Result: " << std::nan("Division by zero") << std::endl;

    return 0;
}
```

In this example, the `std::nan("Division by zero")` expression creates a NaN value with the payload "Division by zero". When printed, the output might be something like:

```
Result: nan(Division by zero)
```

The payload "Division by zero" is included in the output, providing a clear indication of the cause of the NaN.

**Key points:**

- The `const char* arg` parameter is optional and can be omitted if a payload is not needed.
- The interpretation of the payload is implementation-dependent and may vary across different C++ compilers and platforms.
- The payload can be any string, but it is generally recommended to use informative messages that help to understand the context of the NaN.

By using the `const char* arg` parameter in `std::nan`, you can provide more meaningful and helpful information about the NaN values in your C++ programs.

