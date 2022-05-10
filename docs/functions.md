# std::function
Class template std::function is a general-purpose polymorphic function wrapper. Instances of std::function can store, copy, and invoke any Callable target:
1. functions
2. lambda expressions
3. bind expressions
4. functors

```cpp
void print(int value)
{
    std::cout<< value<<std::endl;
}
```

storing a functions:
```cpp
std::function<void(int)> funcPrint=print;
funcPrint(3);
```

store a bind expressions:

```cpp
std::function<void (int)> funcPrint=std::bind(&print,std::placeholders::_1);
std::function<void (int)> f(funcPrint);
f(3);
```

store lambda expressions:
```cpp
auto lambda=[](int value){std::cout<< value<<std::endl;};
std::function<void(int)>f(lambda);
f(3);
```

store a functors:
```cpp
void (*printfunctor)(int);
printfunctor=print;
std::function<void(int)>f(printfunctor);
f(3);
```

# std::bind

Refs: [1](https://stackoverflow.com/questions/6610046/stdfunction-and-stdbind-what-are-they-and-when-should-they-be-used), [2](https://en.wikipedia.org/wiki/Partial_application)

# Sending a function as parameter to an other function
Here in this case planner might use various solver for planning:

```cpp
int planner( int(*fn_solver)(int,int))
{
    return fn_solver(10,12);
}

int solver1(int a,int b)
{
    return a+b;
}

int solver2(int a,int b)
{
    return a-b;
}
```

Sending `solver1` or `solver2`:
    
```cpp
std::cout<< "solver1"<<std::endl;
planner( solver1);
std::cout<< "solver2"<<std::endl;
planner( solver2);
```

function pointer
```cpp
auto func_ptr1=std::bind( &solver1, std::placeholders::_1, std::placeholders::_2);
std::function<int (int,int)> func_ptr2=std::bind( &solver1, std::placeholders::_1, std::placeholders::_2);

std::cout<< func_ptr1(1,2)<<std::endl;
std::cout<< func_ptr2(1,2)<<std::endl;
```

# Scope of return values
When you return a value, a copy is made. The scope of the local variable ends, but a copy is made.

 when a function is called, a temporary space is made for the function to put its local variables, called a frame. When the function (callee) returns its value, it puts the return value in the frame of the function that called it (caller), and then the callee frame is destroyed.

The "frame is destroyed" part is why you can't return pointers or references to local variables from functions. A pointer is effectively a memory location, so returning the memory location of a local variable (by definition: a variable within the frame) becomes incorrect once the frame is destroyed. Since the callee frame is destroyed as soon as it returns its value, any pointer or reference to a local variable is immediately incorrect.

```cpp
int *myBadAddingFunction(int a, int b)
{
    int result;

    result = a + b;
    return &result; // this is very bad and the result is undefined
}

char *myOtherBadFunction()
{
    char myString[256];

    strcpy(myString, "This is my string!");
    return myString; // also allocated on the stack, also bad
}
```

Refs: [1](https://stackoverflow.com/questions/275214/scope-and-return-values-in-c)
# Return smart pointers from functions
# Passing smart pointers to functions
  
  
