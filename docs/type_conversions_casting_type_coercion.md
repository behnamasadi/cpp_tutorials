- [Type conversions](#type-conversions)
  * [Implicit conversion](#implicit-conversion)
  * [Explicit conversion](#explicit-conversion)
- [Type casting](#type-casting)
  * [static cast](#static-cast)
  * [dynamic cast](#dynamic-cast)
  * [const cast](#const-cast)
  * [reinterpret cast](#reinterpret-cast)
- [Type comparision](#type-comparision)

In C++, type conversions, casting, and comparison are fundamental concepts that are important for writing robust programs. Let's go through each of these concepts:

### 1. Type Conversions

Type conversions in C++ can be either implicit or explicit:

#### a. Implicit Conversion (Automatic Type Conversion)
- **Description**: This happens automatically when a value of one data type is assigned to a variable of another data type.
- **Example**:
    ```cpp
    int i = 42;
    double d = i; // Implicit conversion from int to double
    ```

#### b. Explicit Conversion (Type Casting)
- **Description**: This is when you manually convert one data type to another. This is done using casting operators.
- **Types of Casting in C++**:
    - **C-Style Casting**: `(new_type)value`
    - **Function-Style Casting**: `new_type(value)`
    - **Static Cast**: `static_cast<new_type>(value)`
    - **Dynamic Cast**: `dynamic_cast<new_type>(value)` (mostly used with pointers/references to objects in class hierarchies)
    - **Const Cast**: `const_cast<new_type>(value)` (used to remove the `const` qualifier)
    - **Reinterpret Cast**: `reinterpret_cast<new_type>(value)` (used for low-level reinterpreting of bit patterns; use with caution)
- **Example**:
    ```cpp
    double d = 42.50;
    int i = static_cast<int>(d); // Explicit conversion from double to int
    ```

### 2. Type Casting
- **Definition**: Type casting is the process of converting a variable from one data type to another.
- **Usage**: It's used when you need a value of one data type in the form of another data type.
- **Example**: Converting an integer to a float, or a base class pointer to a derived class pointer.

### 3. Type Comparison
- **Definition**: This involves comparing the types of variables or objects.
- **Usage**: It's rarely done directly in C++ (unlike dynamically typed languages).
- **Example**: Generally, type comparison is not a frequent operation in C++. However, dynamic type checking can be done using `typeid` and `dynamic_cast`.

#### Example of Type Comparison:
```cpp
#include <typeinfo>

int main() {
    int i = 5;
    double d = 10.0;

    if (typeid(i) == typeid(int)) {
        // Do something
    }

    if (typeid(d) == typeid(double)) {
        // Do something else
    }
}
```

Remember, while implicit conversion can be convenient, it can also lead to unexpected results or loss of precision, so it's essential to use it judiciously. On the other hand, explicit conversions are clearer and safer as they make the programmer's intentions more evident. Type casting is a powerful tool but should be used carefully to avoid issues like data corruption or undefined behavior. Type comparison in C++ is mostly used in polymorphic scenarios, particularly when dealing with object-oriented programming.



# Type conversions
## Implicit conversion

## Explicit conversion


# Type casting

## static cast
It is a compile time cast. static_cast performs a tight type checking.  

```cpp
class Int 
{ 
    int x; 
  
public:

//     Int(int x_in = 0): x(x_in){}
     Int(int x_in = 0): x{x_in}{}
 
    operator std::string() 
    { 
        std::cout << "Conversion Operator" << std::endl; 
        return std::to_string(x); 
    } 
}; 
```


First example:
```cpp
float f = 3.5; 
int a = f; // c style 
int b = static_cast<int>(f);// recommended
std::cout <<"b: "<<b <<"a:"<<a << std::endl;
```

Second example:
```cpp
char c = 'a'; 

//c style casting,pass at compile time, may fail at run time 
int* q = (int*)&c;
std::cout<< "q"<<* q<<std::endl;
// this is will fail in comile time
//int* p = static_cast<int*>(&c);
```

Third example:
```cppp
Int obj(3);
// compiler will not thrown an error as we have defined the Conversion operator.
std::string str = obj; 

obj = 20; 
std::string str2 = static_cast<std::string>(obj); 
obj = static_cast<Int>(30); 
```
    

## dynamic cast
This cast is executed at run-time, not compile time. It is exclusively used for handling polymorphism.

You can use it for more than just casting downwards - you can cast sideways or even up another chain. The dynamic_cast will seek out the desired object and return it if possible. If it can't, it will return `nullptr `in the case of a pointer, or throw `std::bad_cast` in the case of a reference.

RTTI (Run-time type Information): In C++, RTTI (Run-time type information) is a mechanism that exposes information about an object’s data type at run-time and is available only for the classes which have at least one virtual function. It allows the type of an object to be determined during program execution.


```cpp
class Base
{
	virtual void f() { }
	public:
	void whoAmI()
	{
		std::cout<<"Base" <<std::endl;
	}
};
class Derived:public Base
{
	public:
	void whoAmI()
	{
		std::cout<<"Derived" <<std::endl;
	}
};

class Foo{};
```

Now consider the followings:

```cpp
Base base;
Derived derived;


Derived* derived1 = dynamic_cast<Derived*> (&base);  // nullptr, because 'base' is not a 'derived'
std::cout<<"derived1 is: " <<(derived1 ==nullptr ? "nullptr":"object of Derived class")<<std::endl;



Base* basePointer = new Base;
Derived* derived2 = dynamic_cast<Derived*> (basePointer); 
std::cout<<"derived2 is: " <<(derived2 ==nullptr ? "nullptr":"object of Derived class")<<std::endl;


Base *b = new Derived; 
Derived *d = dynamic_cast<Derived*>(b);
std::cout<<"d is: " <<(d ==nullptr ? "nullptr":"object of Derived class")<<std::endl;


Foo* foo = dynamic_cast<Foo*> (basePointer);  // nullptr
std::cout<<"foo is: " <<(foo ==nullptr ? "nullptr":"object of Derived class")<<std::endl;


Base& baseReference = dynamic_cast<Base&> (*basePointer); // Ok.


//     Derived& derivedReference = dynamic_cast<Derived&> (*basePointer); // Ok.

//this will create a std::bad_cast
//Foo& fooReference = dynamic_cast<Foo&> (*basePointer);

```

## const cast

Refs: [1](//https://www.geeksforgeeks.org/const_cast-in-c-type-casting-operators/)

## reinterpret cast

It is used to convert one pointer of another pointer of any type, no matter either the class is related to each other or not. It does not check if the pointer type and data pointed by the pointer is same or not.


```cpp
struct mystruct { 
    int x; 
    int y; 
    char c; 
    bool b; 
}; 

{
int* p = new int(65); 
char* ch = reinterpret_cast<char*>(p); 
std::cout << *p << std::endl; 
std::cout << *ch << std::endl; 
std::cout << p << std::endl; 
std::cout << ch << std::endl;
}


{

mystruct s; 

// Assigning values 
s.x = 5; 
s.y = 10; 
s.c = 'a'; 
s.b = true; 

// data type must be same during casting 
// as that of original 

// converting the pointer of 's' to, 
// pointer of int type in 'p'. 
int* p = reinterpret_cast<int*>(&s); 

std::cout << sizeof(s) << std::endl; 

// printing the value currently pointed by *p 
std::cout << *p << std::endl; 

// incrementing the pointer by 1 
p++; 

// printing the next integer value 
std::cout << *p << std::endl; 

p++; 

// we are casting back char * pointed 
// by p using char *ch. 
char* ch = reinterpret_cast<char*>(p); 

// printing the character value 
// pointed by (*ch) 
std::cout << *ch << std::endl; 

ch++; 

/* since, (*ch) now points to boolean value, 
so it is required to access the value using  
same type conversion.so, we have used  
data type of *n to be bool. */

bool* n = reinterpret_cast<bool*>(ch); 
std::cout << *n << std::endl; 

// we can also use this line of code to 
// print the value pointed by (*ch). 
std::cout << *(reinterpret_cast<bool*>(ch));
}
```


# Type comparision
```cpp
 std::is_same<T1, T2>
 ```
The putput is true:
```cpp
std::cout << std::boolalpha;
std::cout<< std::is_same<int, std::remove_pointer<int>::type>();
std::cout<< std::is_same<int, std::remove_reference<int &>::type>();
```
Refs: [1](https://en.cppreference.com/w/cpp/types/remove_pointer)


[code](../src/cast.cpp)


Refs: [1](https://www.cplusplus.com/doc/tutorial/typecasting/), [2](https://www.geeksforgeeks.org/const_cast-in-c-type-casting-operators/)



## Type coercion

Type coercion in C++ refers to the process of converting one data type into another. This can happen automatically (implicit coercion) or can be explicitly specified by the programmer (explicit coercion). Here's an overview of both:

### Implicit Type Coercion

C++ performs implicit type coercion when it converts a value from one type to another automatically. This often happens in mixed-type expressions or when passing arguments to functions. For example:

```cpp
int a = 10;
double b = 5.5;
double result = a + b;  // 'a' is implicitly converted to double
```

In this example, the integer `a` is implicitly converted to a double before the addition.

#### Rules of Implicit Type Conversion

1. **Promotion:** Converting a smaller integer type to a larger integer type (e.g., `char` to `int`).
2. **Floating-point promotion:** Converting `float` to `double`.
3. **Mixed-type expressions:** In expressions involving different types, C++ promotes the smaller type to the larger type (e.g., `int` to `double`).
4. **Boolean conversion:** Non-zero values are converted to `true` and zero to `false`.

### Explicit Type Coercion

Explicit type coercion, also known as type casting, is when the programmer specifies the conversion. There are several ways to do this in C++:

1. **C-style cast:**

   ```cpp
   int a = 10;
   double b = (double)a;  // Explicitly cast 'a' to double
   ```

2. **C++ cast operators:** These are safer and more precise than C-style casts.

   - `static_cast`: Most common cast for well-defined and non-polymorphic conversions.

     ```cpp
     int a = 10;
     double b = static_cast<double>(a);
     ```

   - `dynamic_cast`: Used for safe downcasting in class hierarchies.

     ```cpp
     Base *basePtr = new Derived();
     Derived *derivedPtr = dynamic_cast<Derived*>(basePtr);
     ```

   - `const_cast`: Adds or removes `const` qualifier.

     ```cpp
     const int a = 10;
     int *b = const_cast<int*>(&a);
     ```

   - `reinterpret_cast`: For low-level casts, such as converting a pointer to an integer.

     ```cpp
     int a = 10;
     void *ptr = &a;
     int *b = reinterpret_cast<int*>(ptr);
     ```

### Example

Here is an example demonstrating implicit and explicit type coercion:

```cpp
#include <iostream>

class Base {
public:
    virtual void show() { std::cout << "Base class\n"; }
};

class Derived : public Base {
public:
    void show() override { std::cout << "Derived class\n"; }
};

int main() {
    // Implicit Type Coercion
    int i = 42;
    double d = i;  // int to double
    std::cout << "Implicit coercion: " << d << std::endl;

    // Explicit Type Coercion
    double x = 3.14;
    int y = static_cast<int>(x);  // double to int
    std::cout << "Explicit coercion: " << y << std::endl;

    // Polymorphic example with dynamic_cast
    Base *basePtr = new Derived();
    Derived *derivedPtr = dynamic_cast<Derived*>(basePtr);
    if (derivedPtr) {
        derivedPtr->show();  // Output: Derived class
    } else {
        std::cout << "Failed to cast.\n";
    }

    delete basePtr;
    return 0;
}
```

In this example:
- Implicit coercion occurs when `int` is converted to `double`.
- Explicit coercion is demonstrated using `static_cast` to convert `double` to `int`.
- `dynamic_cast` is used for safe downcasting in a polymorphic class hierarchy.

