# Memory checking with Address Sanitizer

Address Sanitizer is a tool developed by Google to check your code for various memory issues such as overflowing, memory leaking, dangling pointer .. etc.

## Adding Address Sanitizer
First you need to add it by adding the right flag:
```cpp
set(CMAKE_CXX_FLAGS "-fsanitize=address ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "-fno-omit-frame-pointer ${CMAKE_CXX_FLAGS}")
```
## Memory Leaking Detection


```cpp
void memory_leak()
{
    unsigned char  *x=new unsigned char [5];
}
```


## Dangling Pointer (Use After Free)

```cpp
void dangling_pointer()
{
    unsigned char  *x=new unsigned char [5];
    delete[] x;
    x[0]=1;
}
```


## Heap Buffer_overflow


```cpp
void heap_buffer_overflow()
{
    unsigned char  *x=new unsigned char [5];
    for(int i=0;i<10;i++)
    {
        x[i]++;
    }
}
```
## Stack Buffer_overflow

```cpp
void stack_buffer_overflow()
{
    char x[5];
    for(int i=0;i<10;i++)
    {
        x[i]++;
    }
}
```

## Stack Overflow
```cpp

void stack_overflow()
{
    int nStack[100000000];
}
```


Refs: [1](https://github.com/google/sanitizers/wiki/AddressSanitizer)
# Memory Leaking Detection with Valgrind

First disable the Address Sanitizer:
```cpp
#set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
#set(CMAKE_CXX_FLAGS "-fsanitize=address ${CMAKE_CXX_FLAGS}")
#set(CMAKE_CXX_FLAGS "-fno-omit-frame-pointer ${CMAKE_CXX_FLAGS}")
```
Then run:

`valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./memory_checking`


Refs: [1](https://valgrind.org/docs/manual/mc-manual.html)
