- [Memory Layout of C Programs](#memory-layout-of-c-programs)
  * [Text](#text)
  * [Data:](#data-)
  * [BSS:](#bss-)
  * [Heap](#heap)
  * [Stack](#stack)
- [Memory segmentation](#memory-segmentation)
- [Segmentation fault](#segmentation-fault)


# Memory Layout of C Programs 
## Text

All codes in the functions variable like `char* p1 = "behnam";` go into the code (Text) region which **read only**.
 
consider the following variable:
```cpp
char* p1 = "behnam";
```

This will complies:
```cpp
p1[0] = 'C';
```
but it will cause `segmentation fault` as the variable is on the `code section` and code section is read only
(`"behnam"`  is a string literal and `p1` holds the starting address of that.)


This is allowed (Value of `p1` can be changed):
```cpp
p1 = "Margarethe";
```
However is we define it as follows:
```cpp
char* const p1 = "John";
```
since `p1` is fixed we can not change it and it is not valid:
```cpp
p1 = "Margarethe";
```

 
## Data:

global/static initialized data

## BSS:

global/static uninitialized data

## Heap

## Stack

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

The "size" command reports the sizes (in bytes) of the Text, Data, and BSS segments.
```cpp
int main(void)
{
    return 0;
}
```
Output of size:
```
text       data        bss
960        248          8
```

```cpp
int global_var;
int main(void)
{
    return 0;
}
```
Output of size: (because of "global_var" 4 bytes added to bss)
```
text       data        bss
 960        248         12
 ```
```cpp 
int global_var;
int main(void)
{
    static int i;
    return 0;
}
```


```
Output of size: (because of "i" 4 bytes added to bss)
text       data        bss
 960        248         16
```

```cpp 
int main(void)
{
    static int i = 100;
    return 0;
}
```
Output of size: (because "i" initialized 4 bytes added removed from bss and added to data)
```
text       data        bss
 960        252         12
```
When you access an array index, C and C++ don't do bound checking. Segmentation faults only happen when you try to
read or write to a page that was not allocated (or try to do something on a page which isn't permitted,
e.g. trying to write to a read-only page), but since pages are usually pretty big
(multiples of a few kilobytes), it often leaves you with lots of room to overflow.
If your array is on the stack, it can be even worse as the stack is usually pretty large (up to several megabytes).
 This is also the cause of security concerns: writing past the bounds of an array on the stack may overwrite the return address of the function
and lead to arbitrary code execution (the famous "buffer overflow" security breaches).
By setting the follwong flag you can find the issue:

```
set(CMAKE_CXX_FLAGS "-fsanitize=address ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "-fno-omit-frame-pointer ${CMAKE_CXX_FLAGS}")
```
# Memory segmentation


 


#  Segmentation fault  
The following are some typical causes of a segmentation fault:
Attempting to access a nonexistent memory address (outside process's address space)
Attempting to access memory the program does not have rights to (such as kernel structures in process context)
Attempting to write read-only memory (such as code segment)
These often happens while dereferencing or assigning null pointer/ wild pointer/ dangling pointer, heap overflow, stack overflow
When you access an array index, C and C++ don't do bound checking. Segmentation faults only happen when you try to
read or write to a page that was not allocated (or try to do something on a page which isn't permitted,
e.g. trying to write to a read-only page), but since pages are usually pretty big
(multiples of a few kilobytes), it often leaves you with lots of room to overflow.
By setting the followings flag you can find the issue:
```
set(CMAKE_CXX_FLAGS "-fsanitize=address ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "-fno-omit-frame-pointer ${CMAKE_CXX_FLAGS}")
```




