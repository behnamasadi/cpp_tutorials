- [const variables](#const-variables)
- [const parameter in functions](#const-parameter-in-functions)
- [const return from functions](#const-return-from-functions)
- [const methods in classes](#const-methods-in-classes)
   * [const and non-const getter methods](#const-and-non-const-getter-methods)
- [all possible const](#all-possible-const)
- [const iterators](#const-iterators)
- [const pointers](#const-pointers)
- [const cast](#const-cast)
  * [casting const to non const](#casting-const-to-non-const)
  * [casting non const to const](#casting-non-const-to-const)
  * [calling a specific overload](#calling-a-specific-overload)
- [mutable](#mutable)
- [constexpr](#constexpr)
  * [constexpr vs inline functions](#constexpr-vs-inline-functions)
  * [constexpr vs const](#constexpr-vs-const)
- [checking if type or value is const](#checking-if-type-or-value-is-const)

# const variables
When declaring a `const` variable, it is possible to put `const` either before or after the type: 
that is, both `int const MAX_FILE_SIZE = 5;` and `const int MAX_FILE_SIZE = 4;` result in `MAX_FILE_SIZE` being a constant integer.

# const parameter in functions
Here, a `str` object is passed by reference into `func`. For safety's sake, const is used to ensure that `func` cannot change the object-
```cpp
void func(const std::string& str);
```

# const return from functions
Here, the return value is const.

```cpp
const int func();
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
struct Foo
{
	int i;
	int non_const_func() 
	{
		return i;
	}

	int const_func() const
	{
		return i;
	}
};
```
const method and const objects can call only const methods:

```cpp
const Foo const_foo_obj;
const_foo_obj.const_func();
```
const methods can not call non-const method, so if we change the code to the following:
```cpp
struct Foo
{
	int i;
	int non_const_func() 
	{
		return i;
	}

	int const_func() const
	{
		return non_const_func();
	}
};
```
we will get the following error:
```
cannot convert 'this' pointer from 'const Foo' to 'Foo &'
```

non-const object and non-const method can call const methods:

```cpp
Foo non_const_foo_obj;
non_const_foo_obj.const_func();
non_const_foo_obj.non_const_func();
``` 
so the rule is, if you have something const, whatever it calls should be const as well.

Refs: [1](https://www.youtube.com/watch?v=4fJBrditnJU), [2](https://stackoverflow.com/questions/2157458/using-const-in-classs-functions)


## const and non-const getter methods

If you have methods that differs only by `const` qualifier they should be overloaded (duplicated code).

```cpp
struct Bar {};

class Foo
{
	Bar m_Bar;
public:
	Bar& GetBar()
	{
		return m_Bar;
	}

	const Bar& GetBar() const
	{
		return m_Bar;
	}
};
```
So we have duplicated code. To avoid it, we should call one getter from another. However, we can not call non-const method from the const one. But we can call const method from non-const one. That will require as to use 'const_cast' to remove the const qualifier.

```cpp
class Foo
{
	Bar m_Bar;
public:
	Bar& GetBar()
	{
		return const_cast<Bar&>(const_cast<const Foo*>(this)->GetBar());
	}

	const Bar& GetBar() const
	{
		return m_Bar;
	}
};
```
Here we call const getter of `Foo` by casting `this` to `const Foo*`:

```cpp
const_cast<const Foo*>(this)->GetBar()
```

Now that we called the const getter, we cast it to non-const value 

```cpp
const_cast<Bar&>(/**/)
```

Since we call const method from non-const, the object itself is non-const, and casting away the const is allowed.



Refs: [1](https://riptutorial.com/cplusplus/example/16974/avoiding-duplication-of-code-in-const-and-non-const-getter-methods-)

# all possible const

struct S
{
	const <return-value> foo(const &<args>) const
	{   
	}

};


# const iterators
They make sure that you can not change the variable in the loop,
```cpp
 std::vector<int> vec;
 vec.push_back( 3 );
 vec.push_back( 4 );
 vec.push_back( 8 );

 for ( std::vector<int>::const_iterator itr = vec.cbegin(), end = vec.cend(); itr != vec.cend(); ++itr ) { }
```
# const pointers
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
`const_cast` is one of the type casting operators. It can be used in order **remove** or **add** constness to an object



## casting const to non const
One good example would be passing const objects to non-const method.
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

## casting non const to const

```cpp
Func( const_cast<const decltype(variable)>(variable) );
```
C++ 17  provides `std::as_const` for exactly this purpose:
```cpp
Func( as_const(variable) );
```
`as_const` is a function rather than a trait, we can use it to "add const" to actual values rather than to types.

Refs: [1](https://stackoverflow.com/questions/53450366/what-do-we-need-stdas-const-for)

## calling a specific overload

```cpp
class foo 
{
    int i;
public:
    foo(int i) : i(i) { }

    int bar() const 
    {
        return i;    
    }
    
// not const
    int bar() 
    { 
        i++;
        return const_cast<const foo*>(this)->bar(); 
    }
};
```

The type of the `this` pointer inside a non-const member function is just `Foo*` (and it is an rvalue) and inside of const method is const.


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
When a variable is captured by value in a lambda expression in C++, a copy of the variable is made and used within the lambda. This copy is independent of the original variable. By default, this copy is `const`, meaning it cannot be modified within the lambda.


However, when you use the `mutable` keyword with a lambda, it allows this copied variable to be modified within the lambda. It's important to understand that this modification affects only the lambda's internal copy of the variable, not the original variable outside the lambda. The original variable remains unchanged regardless of any modifications made to the copy within the lambda.
 
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
    constexpr int max_exp = 17;      // constexpr enables max_exp to be used in Expects
    Expects(0 <= n && n < max_exp);  // prevent silliness and overflow
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


another example:

```cpp
constexpr int  multiplyBy10(int x)
{
    return 10*x;
}
```
in the main:

```cpp
const int a=5;
const int result=multiplyBy10(a);
const int expected=50;

static_assert(expected==result);
```
if you change the value `expected`
```cpp
const int expected=50;
```
it will fail during the **compile** time.

## constexpr vs inline functions
Both are used performance improvements.  The [inline functions](inline_functions.md) is keyword that hints to the compiler to expand a function code (to save the overhead time of function call), however expressions are always evaluated at run time. `constexpr`  are evaluated at compile time. Inline functions suggest the compiler to expand at compile time and

## constexpr vs const 
`constexpr` is mainly used for optimization. `const` are used to make sure that there are no accidental changes by the method. 


Refs: [1](https://www.geeksforgeeks.org/understanding-constexper-specifier-in-c/),[2](https://www.youtube.com/watch?v=4Vnd2I91s2c&)



# checking if type or value is const
You can use `std::is_const`

```cpp
const Foo foo_obj(10);
std::cout << std::boolalpha;
std::cout << std::is_const<decltype(foo_obj)>::value << '\n';
```

output is true:

```cpp
std::cout<< std::is_const_v<const int> << 
```

If `T` is a reference type then `is_const<T>::value` is always false. The proper way to check a potentially-reference type for const-ness is to remove the reference: `is_const<typename remove_reference<T>::type>`.

output is true:
```cpp
std::cout << std::is_const_v<std::remove_reference_t<const int&> > << '\n'; 
```

Output is false:
```cpp
std::cout << std::is_const_v<const int&>  << '\n'; 
```

Output is false:

```cpp
std::cout<< std::is_const_v<const int*> << '\n'; 
```	

Because the pointer itself can be changed but not the int pointed at, the output is true:
```cpp
std::cout<< std::is_const_v<int* const> << '\n'
```	
	
The output is **false**:
```cpp
std::is_const_v< std::remove_pointer<int* const> > << '\n';
```

# Where to not use const
Refs [1](https://www.youtube.com/watch?v=dGCxMmGvocE)	
