
# Value Categories
Every c++ expression (an operator with its operands, a literal, a variable name, etc.)  is characterized by two properties: 
1. **type**. 
2. **value category (i.e rvalue, lvalue)**.

# lvalue
lvalue is an object that occupies some identifiable address in memory (heap, stack) and **not** in CPU register.
lvalues can be written on the left of assignment operator (=). 

# rvalue
rvalue is, the simple explanation is that it doesn't have a memory address. i.e. the number 4. Explicit rvalue defined using **&&**
most useful place for rvalue is overloading copy constructor and copy assignment operator.

In C++11 `&&` means a `r-value` reference. If a function has such a parameter, it is a reference to an argument that is about to be destroyed. Now consider the following vector class:

```cpp
class Vector
{
public:
    int m_length;
    int * m_data;
    

    Vector(){}

    Vector(int length):m_length(length),m_data(new int[length]) { }
    

    //copy constructor, deep copy very costly
    Vector(const Vector &rhs)
    {
        m_length=rhs.m_length;
        m_data=new int[m_length];
        for(int i=0;i<rhs.m_length;i++)
            m_data[i]=rhs.m_data[i];
    }

    //assignment operator
    Vector & operator =(Vector const & rhs)
    {
        m_length=rhs.m_length;
        m_data=new int[m_length];
        for(int i=0;i<rhs.m_length;i++)
            m_data[i]=rhs.m_data[i];
        return *this;
    }

    //move constructor
    Vector(Vector &&rhs)
    {
        m_length=rhs.m_length;
        m_data=rhs.m_data;
        rhs.m_data=nullptr;
        rhs.m_length=0;
    }
    
    //move assignment
    Vector & operator =(Vector  && rhs)
    {
        delete[] m_data;
        m_length=rhs.m_length;
        m_data=rhs.m_data;

        rhs.m_length=0;
        rhs.m_data=nullptr;

        return *this;
    }

    ~Vector()
    {
        delete[] m_data;
    }
    
};
```

Now lets have a look at the following expressions:

"a" is an lvalue
```cpp
int a; 
```

"a" is an lvalue  "a_ref" is a lvalue
```cpp
int& a_ref = a; 
```

"a" is an lvalue ,  "2 + 2" is an rvalue
```cpp
a = 2 + 2; 
```

"a + 2" is an rvalue
```cpp
a + 2 ;
```


"c" is an rvalue
```cpp
int&& c = std :: move(a);
```

p is lvalue:
```cpp
int *p=&a;
```


# Rule of three and Rule of Five



# Move Semantics
Move semantic is implemented for all STL container
Move semantic is useful when you need to have both copy by value and copy by reference at the same time,
if you have only copy by ref then you don't need it.

## when std move is being called?

## when to use and not to use move?


# return value optimization (RVO)

Refs [1](https://stackoverflow.com/questions/3106110/what-is-move-semantics), [2](https://stackoverflow.com/questions/6815685/move-semantics-what-its-all-about?noredirect=1&lq=1), [3](https://stackoverflow.com/questions/14486367/why-do-you-use-stdmove-when-you-have-in-c11?noredirect=1&lq=1), [4](https://stackoverflow.com/questions/56039847/what-is-difference-between-stdmove-and-pass-by-reference-in-c?noredirect=1&lq=1), [5](https://andreasfertig.blog/2022/02/why-you-should-use-stdmove-only-rarely/), [6](https://pspdfkit.com/blog/2019/when-cpp-doesnt-move/), [7](https://medium.com/@lucianoalmeida1/a-little-bit-about-std-move-efd9d554c09a), [8](https://stackoverflow.com/questions/3413470/what-is-stdmove-and-when-should-it-be-used), [9](https://developers.redhat.com/blog/2019/04/12/understanding-when-not-to-stdmove-in-c), [10](https://quick-adviser.com/when-should-i-use-move-semantics/), [11](https://www.cprogramming.com/c++11/rvalue-references-and-move-semantics-in-c++11.html)
