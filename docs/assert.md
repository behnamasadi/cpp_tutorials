# assert
The `assert` function in C++ is used primarily for debugging purposes. It helps you test assumptions made in your code during development. Here's a detailed look at why and when you would use `assert`:

1. **Checking Assumptions**: `assert` is used to ensure that certain conditions hold true in the code at runtime. If the condition evaluated by `assert` is false, the program will terminate. This is helpful in catching bugs early by verifying that assumptions about program state are correct.

2. **Debugging Phase**: The typical use of `assert` is during the development or debugging phase. It allows developers to identify logical errors and incorrect assumptions in their code. If an assertion fails, it provides a clear indication of where the problem might be.

3. **Self-Documentation**: Assertions can also serve as a form of documentation. They clearly state the expectations about the program's behavior at specific points, making it easier for someone reading the code to understand its logic.

4. **Non-Invasive Testing**: Since `assert` statements are usually disabled in the production version of the code (when compiled without the `DEBUG` flag), they don't impact the performance or behavior of the final product. This makes them a non-invasive tool for testing during development.

5. **Conditions to Use `assert`**:
   - Checking preconditions (conditions that must be true before the execution of a function or a block of code).
   - Checking postconditions (conditions that must be true after the execution of a function or a block of code).
   - Validating assumptions about program state at various points in the code.

6. **When Not to Use `assert`**:
   - Do not use `assert` for handling runtime errors that are expected to occur in normal operation; use proper error handling instead.
   - Avoid using `assert` for conditions that must be checked even in the production version of the code.

7. **Disabling Assertions**: In C++, assertions can be disabled by defining the `NDEBUG` macro before including the `cassert` or `assert.h` header. This is commonly done in the production build to avoid the performance overhead and to prevent the program from terminating due to failed assertions.

In summary, `assert` in C++ is a valuable tool for developers to verify the correctness of their code during the development phase, and it plays a crucial role in identifying logical errors and ensuring that code behaves as expected. However, it should not be used as a substitute for proper error handling in situations where errors are expected or in production code.


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
`static_assert` in C++ is a feature that enables compile-time assertion checks. Unlike the regular `assert` function, which checks conditions at runtime, `static_assert` evaluates expressions during the compilation of the program. Here's a detailed explanation:

1. **Compile-Time Evaluation**: `static_assert` is used to check conditions that can be determined at compile time. This means the condition you are testing must be a constant expression, whose value can be determined before the program runs.

2. **Syntax**:
   ```cpp
   static_assert(expression, message);
   ```
   - `expression`: A compile-time constant expression that evaluates to either true or false.
   - `message`: A string literal that will be included in the compilation error message if the assertion fails. This helps in understanding why the compilation failed.

3. **Use Cases**:
   - **Type Traits**: Checking properties of types. For example, ensuring a template argument is of a particular type or has certain characteristics.
   - **Template Metaprogramming**: In complex template programming, `static_assert` can be used to enforce constraints on template parameters.
   - **Checking Constants**: Verifying the values of constant expressions, such as the size of types, buffer sizes, etc.
   - **Platform-Specific Code**: Ensuring that certain conditions are met in platform-specific or hardware-specific code, like verifying sizes of data types for compatibility.

4. **Error Handling**: If the expression in `static_assert` is false, the compilation will stop, and the compiler will produce an error message that includes the provided message string. This helps in early detection of issues that could lead to bugs.

5. **Example**:
   ```cpp
   static_assert(sizeof(int) == 4, "Integers are not 4 bytes");
   ```
   This line checks if the size of `int` is 4 bytes. If it's not, the program will fail to compile, and the specified error message will be displayed.

6. **Advantages**:
   - **Early Detection of Errors**: Issues are caught at compile time rather than at runtime.
   - **Self-Documenting Code**: Like `assert`, it can serve as documentation, clearly stating the assumptions made in the code.
   - **No Runtime Overhead**: Since these checks are at compile time, there is no runtime performance cost.

7. **Differences from Runtime `assert`**:
   - `static_assert` is evaluated at compile time, whereas `assert` is evaluated at runtime.
   - `static_assert` can only test constant expressions, while `assert` can test any expression, including those based on runtime data.

In conclusion, `static_assert` is a powerful tool in C++ for enforcing compile-time constraints, ensuring type correctness in templates, and validating assumptions about the program's environment or platform. Its ability to prevent the compilation of code that doesn't meet certain conditions makes it invaluable in early error detection and in writing robust, portable, and error-free code.

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
  
