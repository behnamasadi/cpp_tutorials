# Temporary objects


A temporary object is an unnamed object created by the compiler to store a temporary value. The following are examples of cases that a temporary object would be created.
1. Store the return value of a function that returns a user-defined type and your program doesn't copy the return value to an object.
```cpp
user-defined-type my-func();
```
after calling `my-func()`:

```cpp
my-func();
```
because we are not storing it in variable after return  an unnamed temporary object would be created.


2. During the evaluation of an expression:
`ExpressionResult = expression1 + expression2 + expression3`. The expression` expression1 +expression2` is evaluated, and the result is stored in a temporary object. Next, the expression `temporary` + `expression3` is evaluated, and the result is copied to `ExpressionResult`

Refs: [1](https://docs.microsoft.com/en-us/cpp/cpp/temporary-objects?view=msvc-170&viewFallbackFrom=vs-2019), [2](https://stackoverflow.com/questions/10897799/temporary-objects-when-are-they-created-how-do-you-recognise-them-in-code), [3](https://www.cplusplus.com/forum/beginner/275448/), [4](https://en.cppreference.com/w/cpp/language/lifetime), [5](https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Temporary_Base_Class)



Note that copy construction can be elided by the compiler as an optimisation.

```cpp
struct S
{
    public:
    S()  { cout << "Constructor is executed\n"; }
    ~S() { cout << "Destructor is executed\n";  }
};
```

Now in the main, this an explicit call to constructor, but nothing is printed:
```cpp
S();  
```
If constructor & destructor have no side effects, then the compiler might be smart enough to figure out that it doesn't actually need to create the temporary, and skip it entirely.
