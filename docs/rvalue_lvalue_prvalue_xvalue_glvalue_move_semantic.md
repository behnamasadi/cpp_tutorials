
# Value Categories
Every c++ expression (an operator with its operands, a literal, a variable name, etc.)  is characterized by
two properties: a **type** and a **value category (i.e rvalue, lvalue)**.

# lvalue
lvalue is an object that occupies some identifiable address in memory (heap, stack) and **not** in CPU register.
lvalues can be written on the left of assignment operator (=). 

# rvalue
rvalue is, the simple explanation is that it doesn't have a memory address. E.g. the number 6
Explicit rvalue defined using **&&**
most useful place for rvalue is overloading copy constructor and copy assignment operator.


consider the following vector class:

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

//Error, since i+2 is rvalue
//int *m=&(a+2);

std::string hello="hello";
std::cout<<"std::string hello is: " ;
print(hello);


std::cout<<"\"hello\"is: " ;
print("hello");



std::cout<<"std::move(hello) is: " ;
print(std::move(hello));
std::cout << hello << std::endl; // Undefined.


std::cout<<"getValue(): " ;
print(getValue());



std::string hello2 = "hello";
std::vector <std::string > owner;
owner.emplace_back (hello2); // Copy.
owner.emplace_back (move(hello2)); // Move.




# Move Semantics
Move semantic is implemented for all STL container
Move semantic is useful when you need to have both copy by value and copy by reference at the same time,
if you have only copy by ref then you don't need it.
