# Memory checking with Valgrind

disable the followings first:
```cpp
#set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
#set(CMAKE_CXX_FLAGS "-fsanitize=address ${CMAKE_CXX_FLAGS}")
#set(CMAKE_CXX_FLAGS "-fno-omit-frame-pointer ${CMAKE_CXX_FLAGS}")
```
Then run:

`valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./<you-app>`



heap buffer overflow
```cpp
int *x=new int[3];
for(int i=0;i<100000000;i++)
{
	x[i]++;
}
```

stack buffer overflow
```cpp
char x[2];
for(int i=0;i<100000000;i++)
{
	x[i]++;
}
```
Here is an example program that causes a stack overflow:

```cpp
int nStack[100000000];
```


memory leaking:
```cpp
char *x = new char[10]; /* or, in C, char *x = malloc(100) */
//    delete[] x;
```
