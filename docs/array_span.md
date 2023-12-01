# C-Style Array


Comparing C arrays with `std::array` from C++. Both are fundamental types used for storing sequences of objects, but they have some key differences:

### C Array
1. **Basic Definition**: A C array is a collection of items of the same type, stored in contiguous memory locations. It is a low-level construct with no built-in bounds checking or safety features.
   
2. **Size Flexibility**: The size of a C array must be known at compile time if it's a static array, or it can be dynamically allocated. However, the size is not a part of the type.

3. **Memory Management**: Requires manual management if dynamically allocated (using `malloc`/`free`). No automatic memory management is provided.

4. **Safety and Convenience**: Provides minimal safety. It does not keep track of its own size, leading to potential buffer overflows and related issues. There's also no direct support for high-level operations like iteration, copying, comparison, etc.

5. **Performance**: Very efficient in terms of performance as it is a simple data structure with direct memory access.

### std::array (C++)
1. **Basic Definition**: `std::array` is a container that encapsulates fixed size arrays. It’s part of the C++ Standard Library.

2. **Size Flexibility**: The size of `std::array` must be known at compile time and is part of its type. This makes it safer but less flexible in terms of size variability.

3. **Memory Management**: As part of the C++ Standard Library, it handles memory automatically and is typically stack-allocated.

4. **Safety and Convenience**: Provides more safety features like bounds checking (when using `.at()` method), size awareness, and compatibility with C++ Standard Library algorithms. It supports iterators, range-based for loops, and can be easily copied or compared.

5. **Performance**: Offers performance comparable to raw arrays but with added benefits of modern C++ features. 

### Summary
- **Use C Array when**: You need minimal overhead, are working in a C or low-level context, or when dynamic size arrays are necessary.
- **Use std::array when**: You are working in C++ and need a fixed-size array with modern language features, safety, and ease of use.

While `std::array` brings many advantages of modern C++, especially in terms of safety and ease of use, C arrays are still relevant for low-level or performance-critical tasks where the overhead of higher-level abstractions is undesirable.




## Size of Array

```cpp
int array[] = { 1, 2, 3, 4, 5 };
std::cout << "sizeof(array)="<<sizeof(array)<<std::endl; // prints 20 (5 elements * 4 bytes each)
const int number_of_elemenets = sizeof(array)/sizeof(int);
std::cout << "number of elemenets = "<<number_of_elemenets<<std::endl;
```
## Accessing Array Elements

These are equal:  
`array<=>&array[0]`  
`array+index<=>&array[index]`  


getting elements:
```cpp
std::cout<<"*array="<< *array<<std::endl;
std::cout<<"*(array+2)="<< *(array+2)<<std::endl;


for (int* p = array; p != array+number_of_elemenets; p++)
{
	std::cout << *p << std::endl;
}
```


## Multidimensional Arrays

`C` does not have true multidimensional arrays. you can have arrays of arrays, so like
```cpp
int first2DArray[3][5] ={
{ 1, 2, 3, 4, 5, }, // row 0
{ 6, 7, 8, 9, 10, }, // row 1
{ 11, 12, 13, 14, 15 } // row 2
};
```
Two-dimensional arrays with initializer lists can omit (only) the first size dimension(the most left one):
```cpp
int second2DArray[][5] ={{ 1, 2, 3, 4, 5, },{ 6, 7, 8, 9, 10, },{ 11, 12, 13, 14, 15 }};
```
or 

```cpp
    int **array;
    array=new int *[3];
    array[0]=new int[2];
    array[1]=new int[4];
    array[2]=new int[3];

    array[0][0]=1;
    array[0][1]=0;

    array[1][0]=5;
    array[1][1]=4;

    std::cout<<array[1][1] <<std::endl;

    delete[] array[0];
    delete[] array[1];
    delete[] array[2];
    delete[] array;
```    


```cpp
int my3DArray[2][3][2] ={
			{{1, 2},{3, 4},{5, 6}},
			{{7, 8}, {9, 10}, {11, 12}}
	};
```






## Sending Multidimensional Array to Functions

1. Fixed Size: it works for automatic (usually stack-living) arrays only i.e. the dimensions should be known at compile time. 

The parameter is a 2D array, you have to specify the size of all dimension except the first one.

this one expect exactly 3x4:
```cpp
void foo(int a[4][3])
{
}
```

we can omit (only) the first size dimension(the most left one)
```cpp
void foo(int a[][3], int size)
{
}
```

2. Variable Size


a pointer to the decayed type:

```cpp
void foo(int *a[10], int size)
{
}
```

```cpp
int *array[10];
for(int i = 0; i < 10; i++)
    array[i] = new int[10];
void foo(int *a[10]) 
{
}

foo(array);
```


pointer to a pointer:

```cpp
void foo(int **a, int m, int n)
{
}
```


```cpp
 int **pp_array;
    int m,  n;

    m=4;
    n=5;

    pp_array = new int *[n];
    for(int i = 0; i <m; i++)
        pp_array[i] = new int[n];
```
or 

```cpp

int first2DArray[3][5] ={
{ 1, 2, 3, 4, 5, }, // row 0
{ 6, 7, 8, 9, 10, }, // row 1
{ 11, 12, 13, 14, 15 } // row 2
};

foo((int**)&first2DArray[0][0], 3, 5);
```


Refs: [1](https://stackoverflow.com/questions/8767166/passing-a-2d-array-to-a-c-function)




# std::array
`std::array` is a container that encapsulates fixed size arrays.

```cpp
std::array<int, 3> a2 = {1, 2, 3};
std::array<std::string, 2> a3 = { std::string("a"), "b" };
```
container operations are supported:

```cpp
std::sort(a2.begin(), a2.end());
std::reverse_copy(a2.begin(), a2.end(), std::ostream_iterator<int>(std::cout, " "));
```

to_array:
```cpp
std::to_array("foo");        // creates std::array<char, 4>{ 'f', 'o', 'o', '\0' }
std::array{"foo"};           // creates std::array<const char*, 1>{ +"foo" }
```

checking is array:

```cpp
char *s  = "Behnam";
std::cout << std::boolalpha;
std::cout << std::is_array<Foo>::value << '\n';
std::cout << std::is_array<Foo[]>::value << '\n';
std::cout << std::is_array<Foo[3]>::value << '\n';
std::cout << std::is_array<float>::value << '\n';
std::cout << std::is_array<int>::value << '\n';
std::cout << std::is_array<int[]>::value << '\n';
std::cout << std::is_array<int[3]>::value << '\n';
std::cout << std::is_array<std::array<int, 3>>::value << '\n';
```


# Span


[code](../src/arrays.cpp)
