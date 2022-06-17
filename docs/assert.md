# assert
An assert statement is a preprocessor macro that evaluates a conditional expression. If the conditional expression is true, the assert statement does nothing. assert will terminate the program (usually with a message quoting the assert statement) if its argument turns out to be false. 
It's commonly used during debugging to make the program fail more obviously if an unexpected condition occurs. 

```cpp
assert ( bool-constexpr )
```

## Add custom messages in assert

As a hack you can use the `&&` operator in your assert. Since assert shows the condition that has failed, it will also show your message too.
```cpp
assert(bool-constexpr && "message");
```

or you can reverse the operands and use the comma operator. You need extra parentheses`()` so the comma isn't treated as a delimiter between the arguments:

```cpp
assert(("message", bool-constexpr));

```

Example:

Here we're asserting that `idx` is between 0 and 9:
```cpp
int idx=12;
int array[10]={0,1,2,3,4,5,6,7,8,9};
// we're asserting that idx is between 0 and 9
assert((idx >= 0 && idx <= 9) &&  "idx should be between 0 and 9");

assert(("idx should be between 0 and 9" , (idx >= 0 && idx <= 9) ));
```

# static_assert 
Static assert is used to make assertions at compile time. When the static assertion fails, the program simply doesn't compile. This is useful in different situations, like, for example,
if you implement some functionality by code that critically depends on unsigned int object having exactly 32 bits. You can put a static assert like this:
```cpp
static_assert(sizeof(unsigned int) * CHAR_BIT == 32);
```
in your code. On another platform, with differently sized unsigned int type the compilation will fail, thus drawing attention of the developer
As an other example, you might want to asset that char type is signed:

```cpp
static_assert(CHAR_MIN < 0);
```
or that integral division with negative values rounds towards zero
```cpp
static_assert(-5 / 2 == -2);
```

or checking the version of c++:


- pre-C++98: `__cplusplus` is 1.
- C++98:     `__cplusplus` is 199711L.
- C++11:     `__cplusplus` is 201103L.
- C++14:     `__cplusplus` is 201402L.
- C++17:     `__cplusplus` is 201703L.
- C++20:     `__cplusplus` is 202002L.


```cpp
static_assert(__cplusplus > 201103L);
```



# NDEBUG

When you're doing a release (non-debug) build, you can also remove the overhead of evaluating assert statements by defining the `NDEBUG` macro, usually with a compiler switch. 
If `NDEBUG` is defined as a macro name at the point in the source file where is included, the assert macro is defined simply as

```cpp
 #define assert(ignore) ((void)0)
 ```
 
The output of followings with comented NDEBUG: `Assertion x >= 0.0 failed.`
 
```cpp
double x = -1.0;
assert(x >= 0.0);
printf("sqrt(x) = %f\n", sqrt(x));  
``` 

The output with uncomented NDEBUG: `sqrt(x) = -nan`



Refs: [1](https://en.cppreference.com/w/c/error/assert)

[Code](../src/assert.cpp)  
  
