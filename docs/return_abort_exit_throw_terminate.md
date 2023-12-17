# Program Control Flow and Termination in C++

In C++, the commands `return`, `exit`, `quick_exit`, `_Exit`, `abort`, `throw`, and `terminate`, are used to control the flow of execution in a program, especially for terminating functions or the entire program. Let's explore each of these:

# return
`retrun` is an instruction of the language and primarily used in functions to return a value to the caller and to terminate the execution of the function.

**Example**: Let's say we have the following code:

```cpp
struct Foo {
  std::string id;
  Foo(std::string id) : id(id) {

    std::cout << id << " constructor" << std::endl;
  }
  ~Foo() { std::cout << id << " destructor" << std::endl; }
};
```

If run the program and then `echo $?`, it will give you your return value `5` and also we will see the the destructor
```cpp
Foo fooObject("normal return");
return 5;
```

In Bash, `echo $?` is used to display the exit status of the last command executed. The exit status is a number that indicates whether the command was successful or not. A value of `0` typically means that the command was successful, while any non-zero value indicates an error or an abnormal condition.

### Example of Using `echo $?`:

1. **After a Successful Command**:
```bash
$ ls
file1.txt file2.txt
$ echo $?
0
```
Here, `ls` successfully lists files, so `echo $?` returns `0`.

2. **After a Failed Command**:
```bash
$ ls non_existing_file.txt
ls: cannot access 'non_existing_file.txt': No such file or directory
$ echo $?
2
```

**In Conditional Statements**:
```bash
if some_command; then
    echo "Success"
else
    echo "Failure, exit status was $?"
fi
```


# exit (exit_code)
`exit`: It terminates the current process and returns the `exit_code` to the operating system. It performs cleanup operations before terminating (like flushing output buffers and calling destructors of **global/static objects**). `exit` is a [system call](system_call.md). `exit()` will call `atexit()` function. `atexit()` is a function that register a function to be called before termination of your application, for instance for cleaning up a temporary folder or print a memory [dump](generating_and_debugging_dump_file.md). If you run the following program:

**Example**: 
```cpp
Foo globalFoo("global object");
static Foo staticFoo("static object");

void runBeforeTermination() {
  std::cout << "cleaning up temp dir before termination..." << std::endl;
}

int exitExample() {
  Foo fooObject("function object");
  std::shared_ptr<Foo> fooObject_ptr(
      new Foo("function object shared pointer object"));
  atexit(runBeforeTermination);
  exit(7);
}
```
and then `echo $?`  it will give you `7` but the destructor for for automatic objects won't be called (only destructor of for your **static** and **global** will be called):

```bash
global object Foo constructor
static object Foo constructor
function object constructor
function object shared pointer object constructor
cleaning up temp dir before termination...
static object Foo destructor
global object Foo destructor
```
Now if you use `return` instead of exit:

```
global object Foo constructor
static object Foo constructor
function object constructor
function object shared pointer object constructor
function object shared pointer object destructor
function object destructor
cleaning up temp dir before termination...
static object Foo destructor
global object Foo destructor
```


# quick_exit
`quick_exit` terminates the program quickly without complete cleanup (like `exit`).

**Example**: Use `quick_exit` when you need a faster termination than `exit`, accepting that some cleanup might not occur.

```cpp
#include <cstdlib>
#include <iostream>

void quickCleanup() {
    std::cout << "Quick cleanup operations\n";
}

int main() {
    std::at_quick_exit(quickCleanup); // Register quick cleanup function
    // ... program code ...
    if (some_condition) {
        std::cout << "Quick exiting.\n";
        quick_exit(EXIT_SUCCESS);
    }
    // ... more program code ...
}
```


# _Exit
`_Exit` terminates the program immediately without calling any cleanup functions.

**Example**: Use `_Exit` in scenarios where you need to terminate the program immediately and do not want any cleanup.

```cpp
#include <cstdlib>
#include <iostream>

int main() {
    // ... program code ...
    if (immediate_exit_condition) {
        std::cout << "Immediate exit without cleanup.\n";
        _Exit(EXIT_FAILURE);
    }
    // ... more program code ...
}
```


# abort
`abort`: Immediately terminates the program abnormally. It does not perform any cleanup operations (like flushing output buffers or calling destructors). It sends a `SIGABRT` [signal](signals.md) and will dump core (if the you have enabled core dumps). 
You can handle an abort signal however you want, but the default behavior is to close the application as well with an error code.
`abort()` exits your program **without** calling functions registered using `atexit()`.
`abort` will not perform object destruction. `exit()` does both (only calling  destructor of for your **static** and **global**) before exiting your program. 


**Example**: If you run the followings:
```cpp
Foo globalFoo("global object");
static Foo staticFoo("static object");

void runBeforeTermination() {
  std::cout << "cleaning up temp dir before termination..." << std::endl;
}
void abortExample()
{
  Foo fooObject("abortExample");
  std::shared_ptr<Foo> fooObject_ptr(new Foo("abortExample_ptr"));
  atexit(runBeforeTermination);
  abort();
}
```
you will get:
```
global object constructor
static object constructor
abortExample constructor
abortExample_ptr constructor
Aborted (core dumped)
```


Sometimes your program goes into state such that its becomes inconsistent and therefore `exit()` will not work because it would cause global objects destruction and the this would not work properly when the state is inconsistent. In such situations `abort()` is to be preferred.
so use `abort()` if your program is in a possibly corrupt state and you consider it too dangerous to try to do anything further.
calling `exit()` mean your application should finish all task gracefully.


# terminate

`terminate` is called by C++ runtime when an exception is not caught. Like `abort`, it terminates the program without performing any cleanup. It can also be called directly.


**Example**: It's rarely called directly but is invoked when an exception is thrown and not caught.

```cpp
#include <exception>
#include <iostream>

void myTerminate() {
    std::cout << "Custom terminate handler\n";
    abort();  // ensure program termination
}

int main() {
    std::set_terminate(myTerminate); // Set custom terminate handler

    throw std::runtime_error("Unhandled exception"); // This will trigger terminate
}
```


# throw
`throw` is used to raise an [exception](exception_handling.md).

**Example**: Use `throw` to signal error conditions that can be caught and handled by a `try-catch` block.

```cpp
#include <stdexcept>
#include <iostream>

void someFunction() {
    // ... some code ...
    if (error_condition) {
        throw std::runtime_error("Error occurred in someFunction");
    }
    // ... more code ...
}

int main() {
    try {
        someFunction();
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << '\n';
    }
}
```



Refs: [1](https://en.cppreference.com/w/cpp/utility/program/abort), [2](https://stackoverflow.com/questions/397075/what-is-the-difference-between-exit-and-abort)

[code](../src/return_abort_exit.cpp)
