Temporary objects in C++ are objects that are created automatically by the compiler during the evaluation of expressions and they typically exist for a short duration. They are not explicitly created by the programmer using new or similar constructs. Here are some key points about temporary objects, followed by examples:

1. **Lifetime**: A temporary object is destroyed at the end of the full expression in which it was created. This means it usually exists only for a very short time.

2. **Use Cases**: They often arise when objects are returned from functions, during type conversions, or in complex arithmetic or logical expressions.

3. **Efficiency**: Modern C++ compilers often optimize the creation and destruction of temporaries through techniques like Return Value Optimization (RVO) and Copy Elision.

Let's go through some examples to understand this concept better:

### Example 1: Function Return
```cpp
class MyClass {
public:
    MyClass() { cout << "Constructor called\n"; }
    ~MyClass() { cout << "Destructor called\n"; }
};

MyClass CreateObject() {
    return MyClass();
}

int main() {
    MyClass obj = CreateObject(); // Temporary object is created and then copied to 'obj'
}
```
In this example, when `CreateObject` is called, it creates a temporary `MyClass` object which is then copied to `obj` in `main`. The temporary object is destroyed at the end of the statement.

### Example 2: Type Conversion
```cpp
class MyString {
public:
    MyString(const char* str) { cout << "Conversion constructor called\n"; }
    ~MyString() { cout << "Destructor called\n"; }
};

void PrintString(const MyString &s) {
    // Function body
}

int main() {
    PrintString("Hello"); // Temporary MyString object is created from const char*
}
```
In this case, when calling `PrintString` with a C-style string, a temporary `MyString` object is created using the conversion constructor.

### Example 3: Arithmetic Expressions
```cpp
class ComplexNumber {
public:
    ComplexNumber(double real, double imag) : r(real), i(imag) {}
    ComplexNumber operator+(const ComplexNumber& other) const {
        return ComplexNumber(r + other.r, i + other.i);
    }
    // Members and other methods
private:
    double r, i;
};

int main() {
    ComplexNumber a(1.0, 2.0), b(3.0, 4.0);
    ComplexNumber c = a + b; // Temporary object created during the addition
}
```
In this example, the addition of `a` and `b` results in a temporary `ComplexNumber` object, which is then assigned to `c`.

These examples demonstrate the creation and use of temporary objects in different scenarios in C++. Understanding their behavior is crucial for writing efficient and bug-free C++ code.

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
