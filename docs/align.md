# Align
memory is loaded into the CPU cache in chunks called **cache lines**. This takes time,
and generally speaking the more cache lines loaded for your object, the longer it takes.
Otherwise, you might get away with sometimes only having part of your object in cache,
and the rest in main memory.



Alignment of 16 means that memory addresses that are a multiple of 16 are the only valid addresses.



For primitive data type of size `x`, the address must be multiple of `x`.
size of int is `4`. That means it can only be stored at addresses like `0, 4, 8, 12,...`

```cpp
std::cout << "size of char: " <<sizeof(char) << std::endl;//1
std::cout << "size of int: "<<sizeof(int) << std::endl;//4
```

1. In the case of `foo1` we have the followings:

```cpp
class foo1
{
    char c1;
    int i1;
};
std::cout << "size of size of foo1 " <<sizeof(foo1) << std::endl; // 8
```

the reason the size of `foo1` is not `1+4` and is `8` is becuase of the memory layout for `foo1`:

```
               +--+--+--+--+--+--+--+--+
               |c1|  |  |  |i1|i1|i1|i1|
               +--+--+--+--+--+--+--+--+
memory address  #0 #1 #2 #3 #4 #5 #6 #7
```



2. In the case of `foo2` we have the followings:

```cpp
class foo2
{
    char c1;
    int i1;
    char c2;
    int i2;
};

std::cout << "size of size of foo2 " <<sizeof(foo2) << std::endl;// 16
```
the memory layout for `foo2` is:

```
               +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
               |c1|  |  |  |i2|i2|i2|i2|c2|  |  |  |i2|i2|i2|i2|
               +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
memory address  #0 #1 #2 #3 #4 #5 #6 #7 #8 #9 #a #b #c #d #e #f
```

3. In the case of `foo3` we have the followings:

```cpp
class foo3
{
    char c1;
    char c2;
    int i1;
    int i2;
};
std::cout << "size of size of foo3 " <<sizeof(foo3) << std::endl; // 12
```

the memory layout for `foo3` is:

```
               +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
               |c1|c2|  |  |i1|i1|i1|i1|i2|i2|i2|i2|  |  |  |  |
               +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
memory address  #0 #1 #2 #3 #4 #5 #6 #7 #8 #9 #a #b #c #d #e #f
```

The starting address of struct or class is **aligned to the maximum alignment requirement of it's member**, so for class `foo3` it is `4`, so it's better to put the larger members first.
Refs: [1](https://www.youtube.com/watch?v=ZvYsXQe-kSY)
# alignas

```cpp
class alignas(8) foo4
{
    char c1;
    int i1;
    char c2;
    int i2;
};


```cpp
align of char: 1
align of int: 4
align of foo1: 4
align of foo2: 4
align of foo3: 4
```

# alignof


Alignment is a restriction on which memory positions a value's first byte can be stored. (It is needed to improve performance on processors and to permit use of certain instructions that works only on data with particular alignment, for example SSE need to be aligned to 16 bytes, while AVX to 32 bytes.)

Alignment of 16 means that memory addresses that are a multiple of 16 are the only valid addresses.



Refs: [1](https://stackoverflow.com/questions/7053190/what-are-the-alignas-and-alignof-keywords-used-for), [2](https://stackoverflow.com/questions/15788947/where-can-i-use-alignas-in-c11), [3](https://stackoverflow.com/questions/17091382/memory-alignment-how-to-use-alignof-alignas)
# Performance Benchmarking

Lets define a 2d matrix:
```cpp
int m,n;
m=10;
n=10;

int **myarray;
myarray=new int*[m];
for(int j=0;j<m;j++)
    myarray[j]=new int[j];
```

row major traverse:

```
------------------>
➘[][][][][][][][][][]
------------------>
➘[][][][][][][][][][]
------------------>
➘[][][][][][][][][][]
------------------>
➘[][][][][][][][][][]
------------------>
➘[][][][][][][][][][]
------------------>
➘[][][][][][][][][][]
```


```cpp
for(int i=0;i<m;i++)
    for(int j=0;j<n;j++)
        myarray[i][j]=myarray[i][j]+1;
```



column major:
```
[]|➚[]|➚[]|➚[]|➚[]
[]| []| []| []| []
[]| []| []| []| []
[]| []| []| []| []
[]| []| []| []| []
[]| []| []| []| []
[]| []| []| []| []
[]| []| []| []| []
  ▼   ▼   ▼   ▼
```

col major traverse:
```cpp
for(int i=0;i<m;i++)
    for(int j=0;j<n;j++)
        myarray[j][i]=myarray[j][i]+1;
```        

If we draw perfomace for this code, for step=1..cache_line it is almost fixed and afterwards it start getting slower
the reason is cpu fetch data from memory( our array) on the chunk size of cache_line.

```cpp
int step=1;//2,3,...2048

int *array=new int[size];
for(int i=0;i<int(size);i+=step)
    array[i]++;
std::cout<<"size: " <<size <<std::endl;

delete[] array;
```
Refs: [1](https://www.youtube.com/watch?v=BP6NxVxDQIs)

[source code](../src/align.cpp)
