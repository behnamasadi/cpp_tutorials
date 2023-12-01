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

`constexpr` is a keyword introduced in C++11 and expanded in later versions of the C++ standard. It's used to declare that the value of a variable or the return value of a function can be evaluated at compile time. This can lead to performance optimizations, as computations can be done during compilation rather than at runtime.

Here are the key points about `constexpr`:

1. **Compile-Time Evaluation**: A `constexpr` variable or function must be able to be evaluated at compile time. This means the compiler must be able to determine the value during the compilation process.

2. **Literal Types**: For a `constexpr` variable, the type needs to be a literal type. In simple terms, it's a type whose initialization and destruction are trivial. Basic data types (like int, float, char) and user-defined types with `constexpr` constructors and destructors qualify.

3. **`constexpr` Functions**: A `constexpr` function can be used to compute values at compile time. The function must have a body that is suitable for constant expression evaluation, meaning it can't have side effects, can't throw exceptions, and must return a value that is a constant expression.

4. **Constant Expressions**: When using `constexpr`, you are essentially declaring that something is a constant expression. This is a key concept in C++ and is used for more than just `constexpr`.

5. **Use Cases**: Common use cases for `constexpr` include defining constants, creating compile-time utility functions (e.g., for computing array sizes or hash values), and for template metaprogramming.

6. **`constexpr` vs `const`**: `const` expresses the intent that a value shouldn't be modified after initialization, but doesn't say anything about compile-time evaluability. `constexpr` implies `const` and additionally indicates that the value can be computed at compile time.

7. **Limitations**: In C++11, there were many limitations to `constexpr` functions (like not being able to use loops), but these have been relaxed in C++14 and later.

Here's an example to illustrate `constexpr` in action:

```cpp
constexpr int factorial(int n) {
    return (n <= 1) ? 1 : (n * factorial(n - 1));
}

int main() {
    constexpr int val = factorial(5); // Computed at compile time
    return 0;
}
```

In this example, the factorial function is defined as `constexpr`, allowing it to be evaluated at compile time when used to initialize the `val` variable. This means that `val` will have the computed value (in this case, 120) embedded in the compiled code, eliminating the need for runtime computation.

Absolutely! Let's explore real-world scenarios where `constexpr` can be beneficial in C++ programming:

1. **Defining Compile-Time Constants**:
    - Scenario: In a graphics application, you want to define the number of pixels per inch (PPI) for rendering images. This value is used throughout your code and should remain constant.
    - Example:
      ```cpp
      constexpr int PixelsPerInch = 300;
      ```

2. **Compile-Time Array Sizes**:
    - Scenario: You're writing a utility function for a network protocol that requires a fixed-size buffer based on the header size and payload size. You want this size to be computed at compile time for efficiency.
    - Example:
      ```cpp
      constexpr size_t HeaderSize = 20;
      constexpr size_t PayloadSize = 512;
      constexpr size_t BufferSize = HeaderSize + PayloadSize;

      char buffer[BufferSize];
      ```

3. **Compile-Time Utility Functions**:
    - Scenario: In a scientific computation library, you need a function to compute the nth Fibonacci number. For certain known values, you want this computation to be done at compile time to improve performance.
    - Example:
      ```cpp
      constexpr int Fibonacci(int n) {
          return (n <= 1) ? n : Fibonacci(n - 1) + Fibonacci(n - 2);
      }

      constexpr int fib10 = Fibonacci(10); // Computed at compile time
      ```

4. **Template Metaprogramming**:
    - Scenario: You're implementing a template class for a math library and need to compute the power of a number at compile time for template instantiations.
    - Example:
      ```cpp
      template<int N, int P>
      struct Power {
          static constexpr int value = N * Power<N, P - 1>::value;
      };

      template<int N>
      struct Power<N, 0> {
          static constexpr int value = 1;
      };

      constexpr int squareOfFive = Power<5, 2>::value; // 5^2 computed at compile time
      ```

5. **Constants for Configuration**:
    - Scenario: You're developing a game engine where certain engine parameters like the maximum number of players should be configurable and known at compile time for optimization.
    - Example:
      ```cpp
      constexpr int MaxPlayers = 16;
      ```

6. **Optimization in Embedded Systems**:
    - Scenario: In an embedded system with limited resources, you need to perform calculations for sensor data processing. These calculations are based on known constants and should be as efficient as possible.
    - Example:
      ```cpp
      constexpr float SensorScaleFactor = 0.0343;
      constexpr float ProcessSensorData(float rawValue) {
          return rawValue * SensorScaleFactor;
      }

      float processedValue = ProcessSensorData(rawValue); // If rawValue is a constexpr, this can be computed at compile time
      ```

In each of these cases, `constexpr` allows for computations to be moved to compile time, reducing runtime overhead and making the code more efficient, especially in scenarios where performance and resource utilization are critical.


The choice between `constexpr` and `const` in C++ depends on your specific needs and the context of your code. Here's why `constexpr` might be preferred over `const` in certain scenarios:

1. **Compile-Time Evaluation**:
   - `constexpr` ensures that a variable or function can be evaluated at compile time. This is particularly useful for optimizing performance, as computations can be done during compilation rather than at runtime.
   - A `const` variable, on the other hand, merely indicates that the variable's value will not change after initialization. It doesn't guarantee that the value can be computed at compile time.

2. **Context of Use**:
   - In template metaprogramming, `constexpr` allows you to perform calculations that are integral to the structure of the program itself, such as in template arguments or array sizes. `const` cannot be used for these purposes as it does not guarantee compile-time evaluation.
   - `constexpr` functions can return different values when called with different arguments, all computed at compile time, which is not possible with `const`.

3. **Optimization**:
   - `constexpr` can lead to better optimization by the compiler. Since `constexpr` expressions are evaluated at compile time, they can reduce the program's runtime overhead. For instance, using `constexpr` for constants used in loops or repetitive calculations can significantly enhance performance.
   - With `const`, the compiler may still need to compute the value at runtime, especially if the initial value isn't known at compile time.

4. **Intent and Clarity**:
   - Using `constexpr` clearly communicates to other developers that the value or function is not only constant but is also intended to be used in a compile-time context.
   - `const` is more general-purpose and doesn't convey the same intent.

Let's revisit the examples to understand why `constexpr` is more suitable than `const`:

- **Defining Compile-Time Constants**: For the PixelsPerInch example, using `constexpr` instead of `const` ensures that the value is embedded directly in the code during compilation, potentially saving a memory read at runtime.

- **Compile-Time Array Sizes**: In the fixed-size buffer example, using `constexpr` guarantees that the size of the buffer is determined at compile time, which is necessary for array declarations.

- **Compile-Time Utility Functions**: The Fibonacci example benefits from `constexpr` because the values can be computed at compile time for known inputs, making it more efficient than calculating at runtime.

- **Template Metaprogramming**: The Power example requires compile-time evaluation for template instantiations, something that `const` cannot provide.

- **Constants for Configuration**: In scenarios like the game engine's MaxPlayers, `constexpr` can embed the value directly in the code, whereas `const` might not, depending on the context and compiler optimizations.

- **Optimization in Embedded Systems**: For the SensorScaleFactor, using `constexpr` ensures that any computation using this factor can potentially be done at compile time, which is crucial in resource-constrained environments like embedded systems.

In summary, `constexpr` is a powerful tool for situations where compile-time computation and optimization are important, while `const` is suitable for general-purpose use where the value is constant but not necessarily evaluated at compile time.

The distinction between `const` and `constexpr` in C++ is not precisely about when the checking happens, but rather about what each keyword guarantees and allows.

1. **`const`:**
   - A `const` variable in C++ means that once the variable is initialized, its value cannot be altered. This is a guarantee about the variable's mutability.
   - Initialization of a `const` variable can happen at either compile time or runtime, depending on the context. For example, if you initialize a `const` variable with a literal value or a constant expression, the compiler might optimize this and treat it as a compile-time constant. However, if the `const` variable is initialized with a value that can only be determined at runtime (e.g., based on user input or a function whose result isn't known until the program runs), then the initialization and the checking of the `const`-ness (i.e., ensuring it's not modified) happen at runtime.

2. **`constexpr`:**
   - A `constexpr` variable or function is intended to be evaluated at compile time. This means that the value of a `constexpr` variable or the return value of a `constexpr` function must be computable during the compilation process.
   - The compiler checks whether a `constexpr` variable or function meets the requirements for compile-time evaluation. If it does, the computation is done at compile time. If a `constexpr` variable or function does not meet these requirements, it results in a compilation error.

In summary, `const` is about constancy (immutability) and doesn't by itself guarantee compile-time evaluation. `constexpr`, on the other hand, is about ensuring that something can and should be evaluated at compile time. The enforcement of `const` and `constexpr` constraints is indeed checked by the compiler, but the timing of their application (compile-time vs. runtime) is different.

Let's consider an example where a `const` variable is initialized at compile time. In this scenario, we'll initialize a `const` variable with a literal value, which allows the compiler to treat it as a compile-time constant.

Here's a simple example:

```cpp
#include <iostream>

int main() {
    const int MaxScore = 100; // Initialized with a literal value

    std::cout << "The maximum score is: " << MaxScore << std::endl;
    return 0;
}
```

In this code:

- `MaxScore` is a `const` integer initialized with the literal value `100`.
- Since `100` is a literal value (a constant expression), the compiler can and typically will optimize this by embedding the value directly in the compiled code. This means that `MaxScore` is effectively a compile-time constant.
- The `const` keyword guarantees that `MaxScore` cannot be modified after its initialization.

Note that while this `const` variable is initialized at compile time, `const` itself does not enforce compile-time initialization. It's the context (i.e., the fact that it's initialized with a constant expression) that allows for compile-time initialization in this case.







Sure! A `const` variable initialized at runtime occurs when its value is determined by something that isn't known until the program is actually running. A common scenario is initializing a `const` variable with the result of a function call or with user input.

Here's an example demonstrating runtime initialization of a `const` variable:

```cpp
#include <iostream>

int getUserInput() {
    std::cout << "Enter a number: ";
    int input;
    std::cin >> input;
    return input;
}

int main() {
    const int userValue = getUserInput(); // Runtime initialization

    std::cout << "The value you entered is: " << userValue << std::endl;
    return 0;
}
```

In this code:

- The function `getUserInput()` prompts the user to enter a number. The number entered by the user can only be known at runtime.
- `userValue` is a `const` integer initialized with the value returned by `getUserInput()`. This initialization happens at runtime because it depends on user input, which is not known at compile time.
- The `const` keyword ensures that `userValue` cannot be modified after its initialization, but it does not enforce or imply compile-time initialization in this case.





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
