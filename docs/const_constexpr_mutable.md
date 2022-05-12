# const variables

When declaring a `const` variable, it is possible to put `const` either before or after the type: 
that is, both `int const MAX_FILE_SIZE = 5;` and `const int MAX_FILE_SIZE = 4;` result in `MAX_FILE_SIZE` being a constant integer.

# const parameter
Here, a `str` object is passed by reference into `func`. For safety's sake, const is used to ensure that `func` cannot change the object-
```cpp
void func(const std::string& str);
```

# const methods in classes
It only work for the methods in a class, when `const` is declared on a non-static class method, tells the compiler that the method doesn't modify the internal state of the object. It is recommended practice to make as many functions const as possible so that accidental changes to objects are avoided.
```cpp
<return-value> <class>::<member-function>(<args>) const
{
   
}
```
Example:

```cpp
class Foo
{
public:
    void non-const-method();
    void const-method() const;
private:
    int a;
};
```
Now in your application:
```cpp
const Foo const_foo;
Foo non-const_foo;
```

allowed:
`const_foo.const-method();`  

not allowed, A const object can't call non-const method:
`const_foo.non-const-method();`

allowed, can call const method on a non-const object
non-const_.const-method();`  

allowed
`non-const_.non-const-method();`

As it also menthod a const method doesn't modify the internal state of the object, so the following is not allowed
```cpp
void Foo::const_method() const
{
    a = 0;   
}
```
Refs: [1](https://www.youtube.com/watch?v=4fJBrditnJU), [2](https://stackoverflow.com/questions/2157458/using-const-in-classs-functions)

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
let say you have the following function:
```cpp
void foo(char* message){}
```
Now if you call it like this:
```cpp
std::string msg = "Hello";
foo(msg.c_str());
```
It won't compile because `msg.c_str()` returns a `const char*` and you can't  call non-const method on a constant pointer The solution is to change the call
```cpp
foo(msg.begin());
```  
or 

```cpp
foo(const_cast<char*>(msg.c_str()));
```



# mutable
The mutable specifier allows modification of the class member declared mutable even if the containing object is declared const. 
If we don`t put mutable before debugCounter, we can't have debugCounter++ in `getName()` as it is a const function and can't change state of the object.

```cpp
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
```
Now in your main:
```cpp
student stdObject("jumbo");
```  
Also Since c++11 `mutable` can be used on a lambda to denote that things captured by value are modifiable (they aren't by default):
 
 
compile error: a by-value capture cannot be modified in a non-mutable lambda 
```cpp
int x = 0;
auto f2 = [=]() {x = 42;};  
```

problem solved:
```cpp
auto f1 = [=]() mutable {x = 42;};  
```

 
Refs: [1](https://www.youtube.com/watch?v=bP9z3H3cVMY), [2](https://stackoverflow.com/questions/105014/does-the-mutable-keyword-have-any-purpose-other-than-allowing-the-variable-to)

# constexpr 
`constexpr` specifies that the value of an **object** or a **function** can be evaluated at **compile** time However that does NOT necessarily guarantee it will be evaluated at compile time and  it **may** be used for such. The purpose of using `constexpr` is performance improvement by doing computations at compile. For example, in below code `sum()` we suggest to compiler to evaluate it at compile time because we are calling it like `const int x = sum(2, 3);`.

```cpp
constexpr int sum(int x, int y)
{
    return (x + y);
}
```

`constexpr` can be used in places that require compile-time evaluation, for instance, template parameters and array-size specifiers:
```cpp
template<int N>
struct fixed_size_array{ };
```

`N` must be an integer constant expression:
```cpp
fixed_size_array<N> my_array;   
int numbers[N];  
```
An object may be fit for use in constant expressions without being declared constexpr. For instance, in the following `N` is constant expression,
because `N` is constant and initialized at declaration time with a literal which satisfies the criteria for a constant expression, even if it isn't declared `constexpr`.

```cpp
const int N = 5;
int numbers[N] = {1, 2, 3, 4, 5};  
```

For a function it must be explicitly declared constexpr to be fit for use in constant expressions. consider the following function:

```cpp
constexpr long int fib(int n)
{
    return (n <= 1)? n : fib(n-1) + fib(n-2);
}
```  
if  call it like:
```cpp
const long int res = fib(30);
```
and messure the running time of the application:
`$time ./a.out`

and remove the `constexpr` and  messure the running time again, you can see the differnce/

Also the following only works because of `constexpr`, because the size of the array should be specified at compile time:
```cpp
int a[fib(3)];
```


## constexpr vs inline functions
Both are used performance improvements.  The [inline functions](inline_functions.md) is keyword that hints to the compiler to expand a function code (to save the overhead time of function call), however expressions are always evaluated at run time. `constexpr`  are evaluated at compile time. Inline functions suggest the compiler to expand at compile time and

# constexpr vs const 
`constexpr` is mainly used for optimization. `const` are used to make sure that there are no accidental changes by the method. 


Refs: [1](https://www.geeksforgeeks.org/understanding-constexper-specifier-in-c/),[2](https://www.youtube.com/watch?v=4Vnd2I91s2c&)

