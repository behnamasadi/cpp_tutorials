# return
`retrun` is an instruction of the language that returns from a function call.
# exit
exit is a [system call](system_call.md) (not a language statement) that terminates the current process. `exit()` means you want to immediately stops the current process. 


It can be used for error management when you code has encountered an irrecoverable issue that won't allow your code to do anything useful anymore. 

```cpp
class foo
{
    public:
    foo(){std::cout<<"foo constructor" <<std::endl;}

    ~foo(){std::cout<<"foo destructor" <<std::endl;}
};
```

if run the program and then `echo $?`, it will give you your return value (5) and also we will see the the destructor
```cpp
foo fooObject;
return 5;
```
now if run the program and then `echo $?`  it will give you (7) but the destructor hasn't been called:

```cpp
foo fooObject;
exit(7) ;
```
# abort
`abort` sends a `SIGABRT` [signal](signals.md). `abort()` exits your program **without** calling functions registered using `atexit()` (atexit registers a function to be called at normal process termination). `abort` will not perform object destruction of your **static** and **global** members, but `exit` will.

`exit()` does both before exiting your program. It does not call destructors for automatic objects though. So


Refs: [1](https://en.cppreference.com/w/cpp/utility/program/abort), [2](https://stackoverflow.com/questions/397075/what-is-the-difference-between-exit-and-abort)

[code](../src/return_abort_exit.cpp)

