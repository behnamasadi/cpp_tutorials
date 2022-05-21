# Stack

It is a dynamic data structure maintained by OS to control the way procedure calling each other and parameter they pass.
Call stack maintained for each thread. The actual implementation of a stack depends on the microprocessor architecture.
It can grow up or down in memory and can move either before or after the push/pop operations

stack is used for functions. It is on top of the memory and moves downward. each function will get its own frame.
Each instance of the function has its own frame. Data that go into frame are:
1) Arguments
2) Local variables (object, data structure)
3) Saved registers.
In high level languages stack will be managed automatically. In low level languages
it is being done with stack pointer and  frame pointer.
Stack pointer:It tell us where is the boundary between allocated and unallocated memory is.
Frame pointer: is a reference pointer allowing us to know where local variable or an argument starts.

```
 ---------------------------------
 |   command line arguments      |
 |   argv[0], argv[1] ...argv[n] |
 ---------------------------------
 |       Allocated memory        |
 |-------------------------------|<--- frame pointer      ┐
 |        Return Address         |                        |
 |          Arguments            |                        |  stack frame1
 |        Local Variables        |                        |
 |        Saved registers        |                        |
 |-------------------------------|<---- stack pointer     ┘
 |                               |
 |                               |  <-stack frame2
 |-------------------------------|
 |                               |
 |                               |  <-stack frame3
 |-------------------------------|
 |                               |
 |                               |
 |                               |
 |                               |
 |                               |
 |                               |
 |              HEAP             |
 |                               |
 |-------------------------------|            ┐
 |  global/static uninitialized  |            |
 |             data              |            |<- BSS
 |                               |            |
 |-------------------------------|            ┘  ┐
 | global/static initialized data|               |<-  Data
 |-------------------------------|               ┘
 |          Code(Text)           |
 |                               | <- Program counter point to line in this region
 |                               |
 ---------------------------------
 |          Reserved             |
 |                               |
 ---------------------------------
```


# stack overflow 
If the call stack pointer exceeds the stack bound, because of:

1. Infinite recursion:

```cpp
int infiniteRecursion()
{
     return infiniteRecursion();
}
```
2. Very large stack variables:

```cpp 
int largeStackVariables()
{
     double x[1000000000];
}
```
# stack frame size

You can then check the stack size with `ulimit -s` and set it to a new value with for example: `ulimit -s 20000`.
The stack size limit on my system is `8192` kilobytes. If I run this program (with no command-line arguments) on my machine,
I get a segfault after around 261978 stack frames. So, each stack frame in this case must be around 32 bytes.

```cpp
int main(int argc, char **argv)
{
    static int frame_count = 0;
    printf("%d\n", frame_count++);
    main(argc, argv);
    return 0;
}
```
You can set the stack size programmatically with (https://linux.die.net/man/2/setrlimit)[setrlimit], e.g.


```cpp
#include <sys/resource.h>

int main (int argc, char **argv)
{
    const rlim_t kStackSize = 16 * 1024 * 1024;   // min stack size = 16 MB
    struct rlimit rl;
    int result;

    result = getrlimit(RLIMIT_STACK, &rl);
    if (result == 0)
    {
        if (rl.rlim_cur < kStackSize)
        {
            rl.rlim_cur = kStackSize;
            result = setrlimit(RLIMIT_STACK, &rl);
            if (result != 0)
            {
                fprintf(stderr, "setrlimit returned result = %d\n", result);
            }
        }
    }

    // ...

    return 0;
}
```

Refs: [1](https://stackoverflow.com/questions/2275550/change-stack-size-for-a-c-application-in-linux-during-compilation-with-gnu-com)
