# Memory checking with Address Sanitizer

```cpp
set(CMAKE_CXX_FLAGS "-fsanitize=address ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "-fno-omit-frame-pointer ${CMAKE_CXX_FLAGS}")
```

```
void heap_buffer_overflow()
{
    unsigned char  *x=new unsigned char [5];
    for(int i=0;i<10;i++)
    {
        x[i]++;
    }

}

void stack_buffer_overflow()
{
    char x[5];
    for(int i=0;i<10;i++)
    {
        x[i]++;
    }
}
```

# Memory Leaking Detection with Valgrind

disable the followings first:
```cpp
#set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
#set(CMAKE_CXX_FLAGS "-fsanitize=address ${CMAKE_CXX_FLAGS}")
#set(CMAKE_CXX_FLAGS "-fno-omit-frame-pointer ${CMAKE_CXX_FLAGS}")
```
Then run:

`valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./memory_checking`


Here is an example program that causes a stack overflow:

```cpp
int nStack[100000000];
```


memory leaking:
```cpp
char *x = new char[10]; /* or, in C, char *x = malloc(100) */
//    delete[] x;
```


Refs: [1](https://www.osc.edu/book/export/html/5471)
