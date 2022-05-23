
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

In C++11 `&&` means a `rvalue` reference. If a function has such a parameter, it is a reference to an argument that is about to be destroyed. 


```cpp
template <typename T>
void print(T &t)
{
    std::cout<<"lvalue" <<std::endl;
}

template <typename T>
void print(T &&t)
{
    std::cout<<"rvalue" <<std::endl;
}
```
Now this will print rvalue

```cpp
print(10);
```

This will print lvalue
```cpp
int i=10;
print(i);
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
A typical use is 'moving' resources from one object to another instead of copying. when you call it on a class, the class needs to know how to do the moving. your class should have a **move-assignment** operator and a **move-constructor** for this to work.

1. vector
consider the following vector class:

```cpp
class Vector
{
public:
    int m_length;
    int * m_data;
    

    Vector(){}

    Vector(int length):m_length(length),m_data(new int[length]) { }
    

    
    Vector(const Vector &rhs)
    {
    	std::cout<<"copy constructor, deep copy very costly"<<std::endl;
        m_length=rhs.m_length;
        m_data=new int[m_length];
        for(int i=0;i<rhs.m_length;i++)
            m_data[i]=rhs.m_data[i];
    }


    Vector & operator =(Vector const & rhs)
    {
	std::cout<<"assignment operator"<<std::endl;
        m_length=rhs.m_length;
        m_data=new int[m_length];
        for(int i=0;i<rhs.m_length;i++)
            m_data[i]=rhs.m_data[i];
        return *this;
    }


    Vector(Vector &&rhs)
    {
    	std::cout<<"move constructor"<<std::endl;
        m_length=rhs.m_length;
        m_data=rhs.m_data;
        rhs.m_data=nullptr;
        rhs.m_length=0;
    }
    
    
    Vector & operator =(Vector  && rhs)
    {
	std::cout<<"move assignment"<<std::endl;
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

In the following swap function:

```cpp
template <class T>
void swap_copy(T& a, T& b) 
{
    T tmp(a);   // we now have two copies of a
    a = b;      // we now have two copies of b (+ discarded a copy of a)
    b = tmp;    // we now have two copies of tmp (+ discarded a copy of b)
}
```

using move allows you to swap the resources instead of copying them around:


```cpp
template <class T>
void swap_move(T& a, T& b) 
{
    T tmp(std::move(a));
    a = std::move(b);   
    b = std::move(tmp);
}
```

In the `swap_copy(T& a, T& b)` you read and write 3*n elements, in the `swap_move` you basically read and write just the 3 pointers to the vectors' buffers, plus the 3 buffers' sizes. 


```cpp
Vector v1(10), v2(20);
swap_copy(v1, v2);
swap_move(v1, v2);
```


Refs: [1](https://stackoverflow.com/questions/3413470/what-is-stdmove-and-when-should-it-be-used)


2. unique_ptr

unique pointer, it has less overhead than shared_ptr, but it has no copy constructor and can not be copied and can be only moved

```cpp
std::unique_ptr<person> entity=std::make_unique<person>();

//this will fail because it is a unique pointer, it can not be copied
//std::unique_ptr<person> secondentity=entity;

//this will work because because we have move constructor
std::unique_ptr<person> secondentity=std::move(entity);
```

3. thread

4. file handler or a similar OS resource

## when to use and not to use move?


# return value optimization (RVO)

[code](../src/rvalue_lvalue.cpp)


Refs [1](https://stackoverflow.com/questions/3106110/what-is-move-semantics), [2](https://stackoverflow.com/questions/6815685/move-semantics-what-its-all-about?noredirect=1&lq=1), [3](https://stackoverflow.com/questions/14486367/why-do-you-use-stdmove-when-you-have-in-c11?noredirect=1&lq=1), [4](https://stackoverflow.com/questions/56039847/what-is-difference-between-stdmove-and-pass-by-reference-in-c?noredirect=1&lq=1), [5](https://andreasfertig.blog/2022/02/why-you-should-use-stdmove-only-rarely/), [6](https://pspdfkit.com/blog/2019/when-cpp-doesnt-move/), [7](https://medium.com/@lucianoalmeida1/a-little-bit-about-std-move-efd9d554c09a), [8](https://developers.redhat.com/blog/2019/04/12/understanding-when-not-to-stdmove-in-c), [9](https://quick-adviser.com/when-should-i-use-move-semantics/), [10](https://www.cprogramming.com/c++11/rvalue-references-and-move-semantics-in-c++11.html)
