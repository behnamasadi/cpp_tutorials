# Copy Constructor

Before c++11, copying was the only way to transfer data from one object to an other object. To move data, you had to copy data and then delete the old data. 
With c++11 you can use move semantic to transfer the ownership of a resource held by one object to an other object.



```cpp

struct S
{
    S(){std::cout<<"constructor" <<std::endl;}
    ~S(){std::cout<<"deconstructor" <<std::endl;}
};    
```

## Copy Constructor 
it happens at:

`S s2 = s1;` 

or

`S s3(s2);`
```cpp

S(const S& rhs){std::cout<<"copy constructor" <<std::endl;}
```

## Copy Assignment Operator
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



# All The Assignment Operators
```cpp

S& operator =(const S &)& =default;
S& operator =(S &&)& =default;
S& operator =(S &)& =default;

S& operator =(const S &)&& =default;
S& operator =(S &&)&& =default;
S& operator =(S &)&& =default;

S& operator =(const S &&) & =delete;
S& operator =(const S &&) && =delete;

S& operator =(const S &) const& =delete;
S& operator =(S &&) const & =delete;
S& operator =(S &)const & =delete;
S& operator =(const S &&) const& =delete;


S& operator =(const S &)const && =delete;
S& operator =(S &&)const && =delete;
S& operator =(S &) const && =delete;
S& operator =(const S &&)const && =delete;
```

Refs: [1](https://www.youtube.com/watch?v=2gjroKLyWKE)

# Move Constructor
The move constructor is used instead of the copy constructor, if the object has type "rvalue-reference" (Type &&).
rvalues: denotes temporary objects created at runtime which doesnt have name.
Exampl: money4+money3 in the

    money  money10(money4+money3);


Rule of all or nothing

None of them defined: all autogenerated
Any of them defined: none autogenerated

Try to define none of the special functions
If you must define one of them define all
Use =default to use default implementation

/******************************** short snippet ********************************/


Refs: [1](https://stackoverflow.com/questions/3106110/what-is-move-semantics)




# How to make class non-copyable, non-moveable
Refs: [1](https://ariya.io/2015/01/c-class-and-preventing-object-copy)