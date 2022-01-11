# const Variables

When declaring a `const` variable, it is possible to put `const` either before or after the type: 
that is, both `int const MAX_FILE_SIZE = 5;` and `const int MAX_FILE_SIZE = 4;` result in `MAX_FILE_SIZE` being a constant integer.

# const parameter
Here, a `str` object is passed by reference into `func`. For safety's sake, const is used to ensure that `func` cannot change the object-
```cpp
void func(const std::string& str);
```

# const Functions
It only work for the methods in a class, 
It is recommended practice to make as many functions const as possible so that accidental changes to objects are avoided.
```cpp
<return-value> <class>::<member-function>(<args>) const
{
   
}
```

# const iterators
They make sure that you can not change the variable in the loop,
```cpp
    std::vector<int> vec;
    vec.push_back( 3 );
    vec.push_back( 4 );
    vec.push_back( 8 );

    for ( std::vector<int>::const_iterator itr = vec.cbegin(), end = vec.cend(); itr != vec.cend(); ++itr ) { }
```
# const Pointers
Declares a pointer whose data cannot be changed through the pointer:
```cpp
const int *p = &someInt;
```
or 
```cpp
int const *p;
```

Declares a pointer who cannot be changed to point to something else:
```cpp
int * const p = &someInt;
```

to make it easy to read remove the variable type,  then read it like:

`const int *p;`  ==> `const  *p ;` ==> `*p` is which is data is fixed.
`int const *p;`  ==> `const  *p ;` ==> `*p` is which is data is fixed.

`int * const p` ==>  `* const p` ==> `p` is fixed which is an address.



# const cast


# mutable
 

If we don`t put mutable before debugCounter, we can't have debugCounter++ in getName() as it is a const function and can't change anything




class student 
{
    std::string name;
    int mutable debugCounter;
public: 
    student(std::string name): name(name), debugCounter(0){}
    void setName(std::string name)
    {
        name=name;
    }
    const std::string getName() const
    {
        debugCounter++;
        return name;
    }
};

int mutableExample()
{
    //first example
    student stdObject("jumbo");
    
    
    //second example
    int x=0;
    auto f=[=]() mutable
    {
        x++;
        std::cout<<x <<std::endl;
    };
    return 0;
}

# constexpr 


Refs: [1](https://www.geeksforgeeks.org/understanding-constexper-specifier-in-c/),[2](https://www.youtube.com/watch?v=4Vnd2I91s2c&)

constexpr specifies that the value of an object or a function can be evaluated at compile time
 and the expression can be used in other constant expressions.
For example, in below code product() is evaluated at compile time.
*/

//First example
// constexpr function for product of two numbers.
// By specifying constexpr, we suggest compiler to
// to evaluate value at compiler time
// you can call this function like:  const int x = product(10, 20);

constexpr int product(int x, int y)
{
    return (x * y);
}


//Second example
/*

you can call it like:
const long int res = fib(30);

if you remove the constexpr and check the run time you will see the difference
$time ./a.out

*/

//Third example
/*
This only works because of constexpr
 int a[fib(3)];

*/
constexpr long int fib(int n)
{
    return (n <= 1)? n : fib(n-1) + fib(n-2);
}


