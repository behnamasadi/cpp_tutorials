# Size and Range of Primitive Data types


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


# Setting Precision

double a;
a = 3.1415926534;
std::cout<<"a is: 3.1415926534" <<std::endl;

std::cout.setf(std::ios::scientific,std::ios::floatfield);
std::cout<<a<<std::endl;//3.140000e+00

std::cout.setf(std::ios::fixed,std::ios::floatfield);
std::cout<<a<<std::endl;//3.140000

std::cout.precision(1);
std::cout<<a<<std::endl;//3.1




# Variable Suffix

```cpp
template<class T> T min(T a, T b)
{
  return (a < b) ? a : b;
}
```


    //Suffix of “f” on float value
    25;     //int
    25l;    //long
    25u;   //unsigned
    25.0;  //double
    25.0f; //float
    float x = min(3.0f, 2.0f); // will compile
/*
    x = min(3.0f, 2);   // compiler cannot deduce T type
    x = min(3.0f, 2.0); // compiler cannot deduce T type
*/



# Scientific Notation
Code: [1](src/data_type_info.cpp), [2](src/primitive_data_types_numerical_limits_accuracy.cpp)
   
   
# decltype    
   
