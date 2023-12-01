# Template Meta-programming
Meta-programming: your code generate some code for you

Refs: [1](http://www.codeproject.com/Articles/257589/An-Idiots-Guide-to-Cplusplus-Templates-Part-1)




# Function Templates


It is not required to use `typename` keyword if a function returning something.
For template programming, these two keywords are very much the same.

However, there are instances where you can only use the newer keyword - `typename`.


`template<class TYPE>`

- tells the compiler that this is a function-template. The actual meaning of TYPE would be deduced by compiler depending on
- the argument passed to this function.
- When you attach template with a generic function, compiler would compile only for required set of data-types!.


## Pointers, References and Arrays with Templates

```cpp
template<class T>
double GetAverage(T tArray[], int nElements)
{}
```
this line: `T tSum = T();` This is not template specific code - this comes under C++ language itself.
It essentially means: Call the default constructor for this datatype. For int, it would be: `int tSum = int();`
Which effectively initializes the variable with 0. Similarly, for float, it would set this variable to 0.0f.
Though not covered yet, if a user defined class type comes from T, it would call the default constructor of
that class (If callable, otherwise relevant error).
As you can understand that `T` might be any data-type, we cannot initialize `tSum` simply with an integer zero (0).
In real case, it may be a some string class, which initializes it with empty string ("").

```cpp
template<class T>
double GetAverage(T tArray[], int nElements)
{
	T tSum = T(); // tSum = 0
	for (int nIndex = 0; nIndex < nElements; ++nIndex)
	{
		tSum += tArray[nIndex];
	}
	// Whatever type of T is, convert to double
	return double(tSum) / nElements;
}
```


```cpp
	template<class T>
	void GetAverage(T* tArray, int nElements){}
```

```cpp
	template<class T>
	void TwiceIt(T& tData){}
```
 this point, you now clearly understand that template parameter type `T` may be inferred from `T&`, `T*` or `T[]`.




For the first call of GetAverage, where IntArray is passed, compiler would instantiate this function as:
`double GetAverage(int tArray[], int nElements);`
```cpp
int  IntArray[5] = {100, 200, 400, 500, 1000};
float FloatArray[3] = { 1.55f, 5.44f, 12.36f};
cout << GetAverage(IntArray, 5);
cout << GetAverage(FloatArray, 3);
```


## Multiple Types with Function Templates

```cpp
template<class T1, class T2>
void PrintNumbers(const T1& t1Data, const T2& t2Data)
{
	 cout << "First value:" << t1Data;
	 cout << "Second value:" << t2Data;
}
```

And we can simply call it as:

```cpp
PrintNumbers(10, 100);    // int, int
PrintNumbers(14, 14.5);   // int, double
PrintNumbers(59.66, 150); // double, int
```

## Explicit Template Argument Specification
```cpp
PrintNumbers<double, double>(10, 100);
PrintNumbers<double, double>(14, 14.5);
PrintNumbers<double, double>(59.66, 150);
```


## Function Template - Template Function

## Default Arguments with Function Templates
 The default-ness may only go from right to left, meaning, if nth argument is required to be default,
 (n+1)th must also be default, and so on till last argument of function.


# Class Templates
When working with templates, you need to have the ** full definition** in the header.
In practice it means implementation inside `.hpp` file.
There is never an implementation file `(*.cpp)` for a template class.
All of the member functions should be declared in the header file.

Refs: [1](http://www.bgsu.edu/departments/compsci/docs/templates.html)
```cpp
template<class T>
class Item
{
    T Data;
public:
    Item() : Data( T() )
    {}

    void SetData(T nValue)
    {
        Data = nValue;
    }

    T GetData() const
    {
        return Data;
    }

    void PrintData()
    {
        cout << Data;
    }
};
```
	
And usage as also quite simple:

```cpp
Item<int> item1;
item1.SetData(120);
item1.PrintData();
Item<float> item2;
float n = item2.GetData();
```




//int TemplatesFunctionmain()


template<class TYPE>
void PrintTwice(TYPE data)
{
    cout<<"Twice: " << data * 2 << endl;
}

template<class T>
T Add(T n1, T n2)
{
    return n1 + n2;
}

## Template Specialization

An Example Program for function template specialization

```cpp
template <typename T>
void add(T a, T b)
{
    std::cout<<a+b <<std::endl;

}
template <>
void add (std::string a, std::string b)
{
    std::cout<<a.append(b) <<std::endl;
}
```

function template specialization:
```
add(10,3);
add(std::string ("a"),std::string ("b"));
```


An Example Program for class template specialization
```cpp
template <typename T>
class foo
{
public:
    static void read()
    {std::cout<<"general"<<"\n";}
};


template<>
class foo<int>
{
    public:
    static void read()
    {
      std::cout<<"special"<<"\n";
    }
};
```

class template specialization
```
foo<double>::read();
foo<int>::read();
```


## Template specialization of a single method from a templated class


```cpp
template <class T>
class TClass 
{
public:
  void doSomething(std::vector<T> * v);
};
```
header declaration:

```cpp
template <>
void TClass<int>::doSomething(std::vector<int> * v);
```

implementation into cpp file

```cpp
template <>
void TClass<int>::doSomething(std::vector<int> * v) 
{

}
```



Refs: [1](https://stackoverflow.com/questions/1723537/template-specialization-of-a-single-method-from-a-templated-class)




## default argument cannot be specified for an explicit template specialization

Refs: [1](https://stackoverflow.com/questions/4050202/why-default-argument-cannot-be-specified-for-an-explicit-template-specialization)

It's perfectly possible to template a class on an integer rather than a type. We can assign the templated value to a variable, or otherwise manipulate it in a way we might with any other integer literal:


```cpp
template <int N>
struct Factorial
{
     //enum { value = N * Factorial<N - 1>::value };
    static constexpr int value = N * Factorial<N - 1>::value;
};

template <>
struct Factorial<0>
{
    //enum { value = 1 };
    static constexpr int value = 1;
};


void templateIntegralParameter()
{
    int x = Factorial<4>::value; // == 24
    int y = Factorial<0>::value; // == 1

    std::cout<< "x:"<<x <<std::endl;
    std::cout<< "y:"<<y <<std::endl;

}
```

You can have several kinds of template parameters

- Type Parameters.
- Types
- Templates (only classes and alias templates, no functions or variable templates)
- Non-type Parameters
- Pointers
- References
- Integral constant expressions
What you have there is of the last kind. It's a compile time constant (so-called constant expression) and is of type integer or enumeration. After looking it up in the standard, i had to move class templates up into the types section - even though templates are not types. But they are called type-parameters for the purpose of describing those kinds nonetheless. You can have pointers (and also member pointers) and references to objects/functions that have external linkage (those that can be linked to from other object files and whose address is unique in the entire program). Examples:

Template type parameter:

```cpp
template<typename T>
struct Container {
    T t;
};
```

pass type "long" as argument.
```cpp
Container<long> test;
Template integer parameter:

template<unsigned int S>
struct Vector {
    unsigned char bytes[S];
};
```

pass 3 as argument.
```cpp
Vector<3> test;
Template pointer parameter (passing a pointer to a function)

template<void (*F)()>
struct FunctionWrapper {
    static void call_it() { F(); }
};
```
pass address of function do_it as argument.
```cpp
void do_it() { }
FunctionWrapper<&do_it> test;
Template reference parameter (passing an integer)

template<int &A>
struct SillyExample {
    static void do_it() { A = 10; }
};
```
pass flag as argument
```cpp
int flag;
SillyExample<flag> test;
Template template parameter.

template<template<typename T> class AllocatePolicy>
struct Pool {
    void allocate(size_t n) {
        int *p = AllocatePolicy<int>::allocate(n);
    }
};
```
pass the template "allocator" as argument.

```cpp
template<typename T>
struct allocator { static T * allocate(size_t n) { return 0; } };
Pool<allocator> test;


```
A template without any parameters is not possible. But a template without any explicit argument is possible - it has default arguments:

```
template<unsigned int SIZE = 3>
struct Vector {
    unsigned char buffer[SIZE];
};

Vector<> test;

```
Syntactically, `template<>` is reserved to mark an explicit template specialization, instead of a template without parameters:


```cpp
template<>
struct Vector<3> {
    // alternative definition for SIZE == 3
};
```

# Templated class in cpp

In C++, it's generally recommended to define templated classes in a header file, not in a .cpp file. The reason for this lies in how the C++ compiler handles templates.

When you use a template class in your code, the compiler generates an instance of the template for the specific types used in your code. This process is called template instantiation. For the compiler to do this, it needs to see the full definition of the template class at the point of instantiation. 

If the template definition is in a .cpp file, it will only be available in that translation unit. This means that if you try to use the template in another .cpp file, the compiler won't have access to the necessary code to instantiate the template, leading to a linker error.

By placing the template definition in a header file, you ensure that it is included in every .cpp file that uses the template, allowing the compiler to generate the needed instances.

However, there are techniques to define templates in a .cpp file if you know all the types that will be used with the template ahead of time. This is known as explicit instantiation. In this case, you can define the template in a .cpp file and then explicitly instantiate it for the specific types you need. But this approach is less flexible and generally less common than defining templates in header files.

Let's illustrate this with a simple example of a templated class in C++. 

First, I'll show how to correctly place a template class in a header file. Then, I'll demonstrate the less common approach of defining it in a .cpp file with explicit instantiation for specific types.

### Example with Template in Header File

1. **MyTemplateClass.h**
   ```cpp
   #ifndef MY_TEMPLATE_CLASS_H
   #define MY_TEMPLATE_CLASS_H

   // Template class definition
   template <typename T>
   class MyTemplateClass {
   public:
       MyTemplateClass(T value) : value_(value) {}

       T getValue() const {
           return value_;
       }

   private:
       T value_;
   };

   #endif // MY_TEMPLATE_CLASS_H
   ```

2. **main.cpp**
   ```cpp
   #include <iostream>
   #include "MyTemplateClass.h"

   int main() {
       MyTemplateClass<int> intInstance(5);
       MyTemplateClass<double> doubleInstance(3.14);

       std::cout << "Integer value: " << intInstance.getValue() << std::endl;
       std::cout << "Double value: " << doubleInstance.getValue() << std::endl;

       return 0;
   }
   ```

In this setup, the template class is defined in the header file (`MyTemplateClass.h`). This way, any .cpp file that includes this header will have access to the complete template definition, allowing the compiler to instantiate it for any type used in those .cpp files.

### Example with Explicit Instantiation in CPP File

1. **MyTemplateClass.cpp**
   ```cpp
   #include "MyTemplateClass.h"

   // Explicit instantiation of the template for int and double
   template class MyTemplateClass<int>;
   template class MyTemplateClass<double>;
   ```

2. **MyTemplateClass.h**
   ```cpp
   // Same as before
   ```

3. **main.cpp**
   ```cpp
   // Same as before
   ```

In this case, `MyTemplateClass.cpp` contains explicit instantiations of the template class for `int` and `double`. This means that the compiler will generate the template code for these two types in this translation unit. However, if you try to use `MyTemplateClass` with a type other than `int` or `double` in another .cpp file, you will get a linker error unless you add an explicit instantiation for that type as well.

The first approach (defining the template in the header) is more flexible and is the commonly used practice in C++. The second approach (explicit instantiation in the .cpp file) is more restrictive and is generally used in specific scenarios where template instantiation overhead needs to be controlled, or for reducing compilation times in large projects.



# Expression Templates


Expression templates in C++ are an advanced programming technique used primarily for optimizing the performance of numerical computations. They leverage the C++ template and operator overloading features to defer and optimize the execution of expressions, especially in the context of array or vector operations. Here's a breakdown of how they work and their benefits:

### How Expression Templates Work

1. **Template Metaprogramming**: Expression templates are a form of template metaprogramming. They use templates to generate code at compile time based on the types and expressions used in the code.

2. **Operator Overloading**: By overloading operators (like `+`, `-`, `*`, etc.), expression templates capture mathematical expressions as types rather than immediately computing their results.

3. **Deferred Evaluation**: Instead of performing computations immediately, expression templates build a representation of the entire expression in a type. This defers the computation until the value is actually needed.

4. **Compile-Time Optimization**: The compiler can analyze and optimize the entire expression at once, potentially eliminating temporary variables and reducing the number of operations.

5. **Example Use Case**: Consider the operation `a = b + c + d;` in the context of vectors. Without expression templates, each `+` operation might create a temporary vector, leading to multiple iterations over the vectors. With expression templates, the entire expression is evaluated in a single pass, without temporary vectors.

### Benefits of Expression Templates

- **Performance Efficiency**: By minimizing temporary objects and redundant computations, expression templates can significantly improve performance, especially in numerical computations.

- **Memory Optimization**: Reduces memory overhead by eliminating unnecessary temporary objects.

- **Expressive Code**: Allows for writing mathematical operations in a natural and readable way, making the code easier to understand and maintain.

### Challenges and Considerations

- **Complexity**: Implementing expression templates can be complex and requires a deep understanding of C++ templates and operator overloading.

- **Compile-Time Overhead**: They can increase the compile time due to the complex template metaprogramming involved.

- **Debugging Difficulty**: Debugging expression templates can be challenging due to the complexity of the generated types and the compile-time nature of the errors.

Expression templates are a powerful technique in C++, especially for libraries dealing with linear algebra, numerical methods, or any domain where complex mathematical expressions are common. However, due to their complexity, they are more suited for advanced C++ programmers and library developers.


Let's create a simplified example to illustrate how expression templates work in C++, focusing on a scenario involving vector arithmetic. This example will demonstrate the principles of deferred evaluation and compile-time optimization.

### Scenario: Vector Arithmetic

Suppose we have a basic `Vector` class and want to perform operations like addition of vectors. Normally, each operation like `v1 + v2` would return a temporary `Vector` object, leading to inefficiency, especially in chained expressions like `v1 + v2 + v3`.

### Step-by-Step Implementation of Expression Templates

1. **Vector Class Definition**

    ```cpp
    class Vector {
    public:
        std::vector<double> data;

        Vector(std::initializer_list<double> values) : data(values) {}

        // Traditional way of adding vectors
        Vector operator+(const Vector& other) const {
            Vector result(data.size());
            for (size_t i = 0; i < data.size(); ++i) {
                result.data[i] = data[i] + other.data[i];
            }
            return result;
        }

        // Other methods...
    };
    ```

2. **Expression Template for Addition**

    ```cpp
    template <typename L, typename R>
    class VectorAddition {
        const L& l;
        const R& r;

    public:
        VectorAddition(const L& left, const R& right) : l(left), r(right) {}

        double operator[](size_t index) const {
            return l[index] + r[index];
        }

        size_t size() const { return l.size(); /* assuming both have same size */ }
    };
    ```

3. **Overloading Operator for Expression Template**

    ```cpp
    template <typename L, typename R>
    VectorAddition<L, R> operator+(const L& left, const R& right) {
        return VectorAddition<L, R>(left, right);
    }
    ```

4. **Using the Expression Templates in Main Function**

    ```cpp
    int main() {
        Vector v1 = {1.0, 2.0, 3.0};
        Vector v2 = {4.0, 5.0, 6.0};
        Vector v3 = {7.0, 8.0, 9.0};

        // The following line does not perform any addition yet.
        auto expr = v1 + v2 + v3;

        // The actual addition happens here, in a single loop.
        Vector result(expr.size());
        for (size_t i = 0; i < expr.size(); ++i) {
            result.data[i] = expr[i];
        }

        // Use result...
    }
    ```

### Explanation

- **Deferred Evaluation**: The expression `v1 + v2 + v3` does not perform any addition immediately. Instead, it creates an instance of `VectorAddition`, which merely stores references to the operands.

- **Compile-Time Optimization**: The compiler can optimize the entire expression, potentially inlining the `operator[]` calls and avoiding temporary `Vector` objects.

- **Efficiency**: When we finally loop over `expr` to compute the result, the additions for the entire expression are performed in a single pass, which is more efficient than creating multiple temporary vectors.


