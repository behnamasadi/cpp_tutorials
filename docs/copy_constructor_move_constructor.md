# Copy Constructor 

# When the **copy constructor** is called

Copy constructor means: **a new `wallet` object is being created** from an existing `wallet`.

## 1) Copy-initialization from another object

```cpp
wallet a(1, 10.0);
wallet b = a;              // [copy-ctor]
```

## 2) Direct-initialization from another object

```cpp
wallet a(1, 10.0);
wallet b(a);               // [copy-ctor]
```

## 3) Pass-by-value into a function

```cpp
void f(wallet w) { }       // w is constructed by copy or move

wallet a(1, 10.0);
f(a);                      // [copy-ctor] into parameter (unless elided / moved)
```

## 4) Return-by-value from a function (when copy elision doesn’t apply)

```cpp
wallet make() {
  wallet t(7, 70.0);
  return t;                // often elided; otherwise [move-ctor] or [copy-ctor]
}
```

Notes:

* Many returns are **elided** (no copy/move happens at runtime).
* If not elided, returning by value usually prefers **move** when available.

## 5) Throwing an exception by value / catching by value

```cpp
wallet a(1, 10.0);
throw a;                   // [copy-ctor] (or [move-ctor] if thrown object is an rvalue)

catch (wallet w) {         // [copy-ctor] (or [move-ctor]) into catch object
}
```

## 6) Initializing elements into containers (copying into storage)

```cpp
std::vector<wallet> v;
wallet a(1, 10.0);

v.push_back(a);            // [copy-ctor] (because a is an lvalue)
```

If you do this instead:

```cpp
v.push_back(std::move(a)); // [move-ctor]
```

## 7) Capturing by value in a lambda

```cpp
wallet a(1, 10.0);

auto lam = [a]() {         // [copy-ctor] into closure
  // ...
};
```

If you do:

```cpp
auto lam2 = [x = std::move(a)]() {  // [move-ctor] into closure
  // ...
};
```

## 8) `std::pair`, `std::tuple`, aggregates that store a copy

```cpp
wallet a(1, 10.0);

std::pair<wallet, int> p{a, 3};     // [copy-ctor]
```

---

# When the **copy assignment operator** is called

Copy assignment means: an **already-existing** `wallet` is overwritten from another.

## 1) Assigning to an already-existing object

```cpp
wallet a(1, 10.0);
wallet b(2, 20.0);

b = a;                     // [copy-assign]
```

## 2) Assignment inside a function (destination already exists)

```cpp
void g(wallet& dst, const wallet& src) {
  dst = src;               // [copy-assign]
}
```

## 3) Assigning to an element already stored in a container

```cpp
std::vector<wallet> v;
v.emplace_back(1, 10.0);
v.emplace_back(2, 20.0);

v[1] = v[0];               // [copy-assign]
```

## 4) Reassigning a previously assigned object

```cpp
wallet a(1, 10.0), b(2, 20.0), c(3, 30.0);

b = a;                     // [copy-assign]
b = c;                     // [copy-assign] again
```

## 5) Copy-assigning a member inside another type’s assignment

```cpp
struct S {
  wallet w;

  S& operator=(const S& other) {
    w = other.w;           // wallet [copy-assign]
    return *this;
  }
};
```

---

# Easy rule to remember

✅ **Copy constructor**: a **new object** is created from an existing one.
✅ **Copy assignment**: an **existing object** is overwritten from another existing one.

---

# Common confusion: this is NOT assignment

```cpp
wallet a(1, 10.0);
wallet b = a;              // [copy-ctor], NOT [copy-assign]
```

Because `b` is being **created**, not overwritten.

---


# When the **move constructor** is called `[move-ctor]`

Move constructor means: a **new `wallet` object is being created** by **stealing resources** from an rvalue (typically via `std::move` or a temporary).

## 1) Move-initialization from an existing object using `std::move`

```cpp
wallet a(1, 10.0);
wallet b = std::move(a);        // [move-ctor]
```

or

```cpp
wallet a(1, 10.0);
wallet b(std::move(a));         // [move-ctor]
```

## 2) Constructing from a temporary (prvalue)

```cpp
wallet b = wallet(2, 20.0);     // often elided; otherwise [move-ctor]
```

## 3) Pass-by-value when the argument is an rvalue

```cpp
void f(wallet w) { }

f(wallet(3, 30.0));             // [move-ctor] into parameter (often elided)
```

or

```cpp
wallet a(1, 10.0);
f(std::move(a));                // [move-ctor] into parameter
```

## 4) Returning a temporary / rvalue (when copy elision doesn’t apply)

```cpp
wallet make() {
  return wallet(4, 40.0);       // often elided; otherwise [move-ctor]
}
```

## 5) Containers: inserting an rvalue

```cpp
std::vector<wallet> v;
wallet a(1, 10.0);

v.push_back(std::move(a));      // [move-ctor]
```

Also:

```cpp
v.push_back(wallet(5, 50.0));   // [move-ctor] (or elided)
```

## 6) Lambda capture by move (init-capture)

```cpp
wallet a(1, 10.0);

auto lam = [x = std::move(a)]() {  // [move-ctor] into closure
  // ...
};
```

## 7) `std::pair`, `std::tuple` constructed from rvalues

```cpp
wallet a(1, 10.0);

std::pair<wallet, int> p{std::move(a), 3};  // [move-ctor]
```

---

# When the **move assignment operator** is called `[move-assign]`

Move assignment means: an **already-existing `wallet`** is overwritten by **stealing resources** from an rvalue.

## 1) Assigning from `std::move(...)`

```cpp
wallet a(1, 10.0);
wallet b(2, 20.0);

b = std::move(a);               // [move-assign]
```

## 2) Assigning from a temporary (prvalue)

```cpp
wallet b(2, 20.0);

b = wallet(3, 30.0);            // [move-assign] (or elided/optimized)
```

## 3) Move-assigning container elements

```cpp
std::vector<wallet> v;
v.emplace_back(1, 10.0);
v.emplace_back(2, 20.0);

v[1] = std::move(v[0]);         // [move-assign]
```

## 4) Move-assigning a member inside another type’s move assignment

```cpp
struct S {
  wallet w;

  S& operator=(S&& other) noexcept {
    w = std::move(other.w);     // wallet [move-assign]
    return *this;
  }
};
```

---

# Easy rule to remember

✅ **Move constructor**: a **new object** is created by stealing from an rvalue.
✅ **Move assignment**: an **existing object** steals from an rvalue.

---

# Common confusion: this is NOT move assignment

```cpp
wallet a(1, 10.0);
wallet b = std::move(a);        // [move-ctor], NOT [move-assign]
```

Because `b` is being **created**, not overwritten.












```cpp
struct S
{
    S(){std::cout<<"constructor" <<std::endl;}
    ~S(){std::cout<<"deconstructor" <<std::endl;}
};    
```


it happens at:

`S s2 = s1;` 

or

`S s3(s2);`
```cpp

S(const S& rhs){std::cout<<"copy constructor" <<std::endl;}
```

or for instance here, you create a `std::unique_ptr<std::vector<int>>` that points to a new `std::vector<int>` which is constructed using the copy constructor of `std::vector<int>`. This means you are copying `localVector` into a new `std::vector<int>` that is managed by the `std::unique_ptr`.

```cpp
std::unique_ptr<std::vector<int>> foo() {
    std::vector<int> localVector;
    localVector.push_back(2);
    localVector.push_back(4);
    localVector.push_back(5);

    return std::make_unique<std::vector<int>>(localVector);
}
```


# Copy Assignment Operator
it happens at  `s2 = s1;`

```cpp
S& operator = (const S& rhs){std::cout<<"copy assignment operator" <<std::endl;}
};
```

# Move Constructor
It happens at: 
`S s2(std :: move(s1));`
    
or
`S s2 = std :: move(s1);`

```cpp 
S(S&& rhs)
{
	std::cout<<"move constructor" <<std::endl;
}
```

# Move Assignment Operator

it happens at: 

`s1 = std :: move(s2);`

```cpp
S& operator = (S&& rhs){std::cout<<"move assignment operator" <<std::endl;}
```

[Full code](../src/class/copy_move.cpp)


# Rule of all (Rule of Five) or nothing

None of them defined: all autogenerated
Any of them defined: none autogenerated

Try to define none of the special functions
If you must define one of them define all
Use =default to use default implementation



Refs: [1](https://stackoverflow.com/questions/3106110/what-is-move-semantics)




# How to make class non-copyable, non-moveable
Refs: [1](https://ariya.io/2015/01/c-class-and-preventing-object-copy)


# Value Categories
Every c++ expression (an operator with its operands, a literal, a variable name, etc.)  is characterized by two properties: 
1. **type**. 
2. **value category (i.e rvalue, lvalue)**.

## lvalue
lvalue is an object that occupies some identifiable address in memory (heap, stack) and **not** in CPU register.
lvalues can be written on the left of assignment operator (=). 

## rvalue
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



# Rvalue references
An rvalue reference is a new kind of reference that only binds to rvalues, and the syntax is `X&&`. The good old reference `X&` is now known as an lvalue reference. (Note that `X&&` is not a reference to a reference; there is no such thing in C++.). If we throw const into the mix, we already have four different kinds of references. What kinds of expressions of type X can they bind to?

```
            lvalue   const lvalue   rvalue   const rvalue
---------------------------------------------------------              
X&          yes
const X&    yes      yes            yes      yes
X&&                                 yes
const X&&                           yes      yes
```

Refs: [1](https://stackoverflow.com/questions/3106110/what-is-move-semantics)


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



