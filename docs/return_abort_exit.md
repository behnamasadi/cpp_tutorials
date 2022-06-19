# return
`retrun` is an instruction of the language that returns from a function call.

Let's say we have the following code:

```cpp
struct Foo
{
    std::string id;
    Foo(std::string id)
    {
        this->id= id;
        std::cout<<id<<" constructor" <<std::endl;
    }
    ~Foo()
    {
        std::cout<<id <<" destructor" <<std::endl;
    }
};
```

if run the program and then `echo $?`, it will give you your return value `5` and also we will see the the destructor
```cpp
foo fooObject;
return 5;
```


# exit
`exit` is a [system call](system_call.md) and not a language statement. It  terminates the current process. `exit()` will call `atexit()` function.
`atexit()` is a function that register a function to be called before termination of your application, for instance for cleaning up a temporary folder or print a memory dump. If you run the following program:

```cpp
Foo globalFoo("globalFoo");
static Foo staticFoo("staticFoo");

void runBeforeTermination()
{
    std::cout<<"cleaning up temp dir before termination..." <<std::endl;
}

int exitExample()
{
    Foo fooObject("exitExample");
    std::shared_ptr<Foo> fooObject_ptr(new Foo("exitExample_ptr") );
    atexit (runBeforeTermination);
    exit(7) ;
}
```
and then `echo $?`  it will give you `7` but the destructor for for automatic objects won't be called (only destructor of for your **static** and **global** will be called):

```
globalFoo constructor
staticFoo constructor
exitExample constructor
exitExample_ptr constructor
cleaning up temp dir before termination...
staticFoo destructor
globalFoo destructor
```


# abort
`abort` sends a `SIGABRT` [signal](signals.md) and will dump core (if the you have enabled core dumps). 
You can handle an abort signal however you want, but the default behavior is to close the application as well with an error code.
`abort()` exits your program **without** calling functions registered using `atexit()`.
`abort` will not perform object destruction. `exit()` does both (only calling  destructor of for your **static** and **global**) before exiting your program. 

If you run the followings:
```cpp
Foo globalFoo("globalFoo");
static Foo staticFoo("staticFoo");

void abortExample()
{
    Foo fooObject("abortExample");
    std::shared_ptr<Foo> fooObject_ptr(new Foo("abortExample_ptr") );
    atexit (runBeforeTermination);
    abort() ;
}
```
you will get:
```
globalFoo constructor
staticFoo constructor
abortExample constructor
abortExample_ptr constructor
Aborted (core dumped)
```


Sometimes your program goes into state such that its becomes inconsistent and therefore `exit()` will not work because it would cause global objects destruction and the this would not work properly when the state is inconsistent. In such situations `abort()` is to be preferred.
so use `abort()` if your program is in a possibly corrupt state and you consider it too dangerous to try to do anything further.
calling `exit()` mean your application should finish all task gracefully.


Refs: [1](https://en.cppreference.com/w/cpp/utility/program/abort), [2](https://stackoverflow.com/questions/397075/what-is-the-difference-between-exit-and-abort)

[code](../src/return_abort_exit.cpp)

