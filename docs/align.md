Sure, `alignas` and `alignof` are keywords introduced in C++11 to provide more control over memory alignment.

### alignas
The `alignas` specifier allows you to specify the alignment requirement of a variable or a type. Memory alignment refers to the way data is arranged in memory. Aligning data can lead to better performance due to the way the processor accesses memory.

#### Syntax
```cpp
alignas(expression) type variable_name;
```
- `expression`: A constant expression that evaluates to a valid alignment value (which must be a power of two).
- `type`: The type of the variable.
- `variable_name`: The name of the variable.

#### Example
```cpp
#include <iostream>
#include <memory>

struct alignas(16) AlignedStruct {
    int a;
};

int main() {
    std::cout << "Alignment of AlignedStruct: " << alignof(AlignedStruct) << std::endl;
    AlignedStruct myStruct;
    std::cout << "Address of myStruct: " << &myStruct << std::endl;
    return 0;
}
```
In this example, `AlignedStruct` is aligned to a 16-byte boundary, which means the start of each `AlignedStruct` object will be at an address that's a multiple of 16.

### alignof
The `alignof` operator returns the alignment requirement of its operand type.

#### Syntax
```cpp
alignof(type)
```
- `type`: The type whose alignment requirement you want to query.

#### Example
```cpp
#include <iostream>

int main() {
    std::cout << "Alignment of int: " << alignof(int) << std::endl;
    std::cout << "Alignment of double: " << alignof(double) << std::endl;
    return 0;
}
```
This code snippet prints the alignment requirements of `int` and `double` types.

### Applications
1. **Optimization**: Proper alignment can lead to more efficient memory access, improving performance, especially in low-level, performance-critical code.
2. **Hardware Interfacing**: Some hardware interfaces require data to be aligned in a certain way.
3. **Standard Compliance**: Certain standards (like SIMD) may have specific alignment requirements.

Remember, over-aligning can lead to wasted memory, so it's important to align only as strictly as necessary.





### Example 1: Performance Optimization with alignas

Suppose you are working with SIMD (Single Instruction, Multiple Data) operations which require data to be aligned to specific boundaries (like 16 or 32 bytes) for optimal performance. Here, `alignas` can be used to ensure that the data structure aligns with these requirements.

```cpp
#include <iostream>
#include <vector>

struct alignas(16) Vec3 {
    float x, y, z;
};

int main() {
    std::vector<Vec3> points(100);

    // Assuming SIMD operations here for demonstration
    for (auto& point : points) {
        // SIMD-optimized operations
    }

    std::cout << "Alignment of Vec3: " << alignof(Vec3) << std::endl;
    return 0;
}
```

In this example, each `Vec3` structure is aligned to a 16-byte boundary, which is beneficial for SIMD operations.

### Example 2: Ensuring Hardware Requirements with alignas

Some hardware devices or protocols might require data to be aligned in a certain way for proper functioning. For example, a network packet structure might need to be aligned to 8-byte boundaries.

```cpp
#include <iostream>

struct alignas(8) NetworkPacket {
    uint32_t header;
    uint32_t payload;
    // Other packet data...
};

int main() {
    NetworkPacket packet;

    std::cout << "Alignment of NetworkPacket: " << alignof(NetworkPacket) << std::endl;
    // Network operations...
    return 0;
}
```

This alignment ensures that the `NetworkPacket` meets the hardware alignment requirements.

### Example 3: Checking Alignment with alignof

When working with libraries or APIs that have specific alignment requirements, you can use `alignof` to check if your data structures meet these requirements.

```cpp
#include <iostream>

struct MyData {
    char data[100];
    // Other members...
};

int main() {
    static_assert(alignof(MyData) >= 8, "MyData must be aligned to at least 8 bytes");

    MyData myData;
    // Use myData with an API requiring at least 8-byte alignment
    return 0;
}
```

In this example, the `static_assert` checks if `MyData` meets the required alignment and will trigger a compile-time error if it doesn't.

### Applications in Context

1. **SIMD and High-Performance Computing**: Aligning data structures for SIMD operations can lead to significant performance gains in scientific computing, graphics processing, and real-time systems.
2. **Hardware Communication**: Aligning data structures as per hardware protocols is critical in embedded systems and device driver development.
3. **Memory Management**: Understanding and utilizing alignment can lead to more efficient memory usage and avoid issues like false sharing in multithreaded applications.

These examples demonstrate the practical benefits of using `alignas` and `alignof` in various scenarios, highlighting their importance in system-level and performance-critical applications in C++.


# Examples with Eigen

The Eigen library is a popular C++ template library for linear algebra. Eigen makes extensive use of memory alignment for performance optimization, especially for vectorized operations. When using Eigen, it's important to respect its alignment requirements, especially when creating fixed-size vectorizable Eigen types.

Here's an example that demonstrates the use of Eigen with alignment considerations:

### Example: Using Eigen with Alignment

First, ensure you have the Eigen library available. You can typically include it in your project via:

```cpp
#include <Eigen/Dense>
```

Now, let's create a simple example where we perform operations on Eigen matrices and vectors, paying attention to alignment:

```cpp
#include <iostream>
#include <Eigen/Dense>

int main() {
    // Eigen provides several typedefs for common matrix and vector types
    Eigen::Matrix4f mat4; // 4x4 float matrix
    Eigen::Vector4f vec4; // 4-element float vector

    // Initializing matrix and vector
    mat4 << 1, 2, 3, 4,
            5, 6, 7, 8,
            9, 10, 11, 12,
            13, 14, 15, 16;
    vec4 << 1, 2, 3, 4;

    // Performing a matrix-vector multiplication
    Eigen::Vector4f result = mat4 * vec4;

    std::cout << "Result:\n" << result << std::endl;

    return 0;
}
```

In this example:
- We use `Eigen::Matrix4f` and `Eigen::Vector4f`, which are Eigen types for a 4x4 matrix and a 4-element vector, respectively, both containing `float` values.
- We initialize these with some values and perform a matrix-vector multiplication.
- Eigen takes care of the alignment automatically for these fixed-size types.

### Special Considerations for Alignment

If you're creating custom structures or classes that contain Eigen types, you need to ensure proper alignment. Here's an example:

```cpp
#include <Eigen/Dense>

struct MyStruct {
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    Eigen::Matrix4f matrix;
    Eigen::Vector4f vector;
    // Other members...
};

int main() {
    MyStruct myStruct;
    // Operations on myStruct...
    return 0;
}
```

In this struct:
- We use `EIGEN_MAKE_ALIGNED_OPERATOR_NEW` to ensure that the memory allocation is properly aligned for the Eigen types.
- This is especially important if your structure will be allocated dynamically (using `new`).

### Note on Dynamic-Sized Eigen Types

For dynamic-sized matrices and vectors (like `Eigen::MatrixXd` or `Eigen::VectorXd`), Eigen handles alignment internally, and you typically don't need special alignment considerations. However, for fixed-size, vectorizable types (like `Eigen::Matrix4f`), proper alignment is crucial for optimal performance.


# Cache Lines

The concept of memory alignment is deeply intertwined with the architecture of computer memory, particularly cache lines. Understanding this relationship is crucial in high-performance computing, where proper alignment can significantly impact the efficiency of memory access.

### Cache Lines

1. **What are Cache Lines?**
   - Modern CPUs have multiple levels of cache (L1, L2, L3) which are smaller but faster than the main memory (RAM).
   - A cache line is the smallest unit of data that can be transferred between the main memory and the cache.
   - The size of a cache line varies depending on the CPU architecture, but common sizes are 64 bytes or 128 bytes.

2. **Why Cache Lines Matter?**
   - When a CPU needs to access data, it first checks if the data is in the cache (cache hit). If not (cache miss), it loads a whole cache line from the main memory.
   - Accessing the cache is much faster than accessing the main memory. Therefore, maximizing cache hits and minimizing cache misses is a key to high performance.

### Alignment and Cache Lines

1. **Memory Alignment:**
   - Memory alignment means placing data in memory at address multiples of some power of two. This is where the size of a cache line becomes crucial.
   - If your data is misaligned with respect to cache lines, it might straddle two cache lines. This can lead to two cache misses instead of one for a single data access, effectively doubling the latency.

2. **Benefits of Alignment:**
   - **Reduced Cache Misses:** Properly aligned data structures ensure that they fit within cache lines more efficiently, reducing the likelihood of cache misses.
   - **Vectorized Operations:** Many modern processors use SIMD (Single Instruction, Multiple Data) instructions that operate on multiple data points simultaneously. For optimal performance, the data processed by SIMD instructions often needs to be aligned with cache lines.

3. **Alignas and Eigen Example:**
   - In the context of the Eigen library and `alignas` in C++, aligning data structures like vectors and matrices with the cache line boundary can improve performance.
   - For instance, if you have a `Matrix4f` (16 bytes for each float, so 64 bytes in total), aligning it to a 64-byte boundary ensures that the entire matrix sits in a single cache line, optimizing cache access.

### Practical Implications

- **Cache-Friendly Code:** When writing high-performance code, it's important to structure your data and access patterns in a cache-friendly way. This means not only aligning data structures but also considering the order of accessing data to maximize cache hits.
- **Memory Padding:** Sometimes adding padding to data structures can improve performance by ensuring that frequently accessed data does not straddle cache lines.
- **Hardware-Specific Optimization:** Optimal alignment can vary based on the specific hardware. Profiling and testing are often necessary to find the best alignment for a given scenario.

In conclusion, understanding and leveraging the relationship between memory alignment and cache lines is a key aspect of writing efficient, high-performance software, especially in systems programming, scientific computing, and real-time applications where every microsecond counts.
When the processor accesses a part of memory that is not already in the cache it loads a chunk of the memory around the accessed address into the cache, hoping that it will soon be used again.

The chunks of memory handled by the cache are called cache lines. The size of these chunks is called the cache line size. Common cache line sizes are 32, 64 and 128 bytes.

A cache can only hold a limited number of lines, determined by the cache size. For example, a 64 kilobyte cache with 64-byte lines has 1024 cache lines.


CPUs are word oriented, not byte oriented. In a simple CPU, memory is generally configured to return one word (32bits, 64bits, etc) per address strobe
Alignment depends on the CPU word size (16, 32, 64bit)




Refs: [1](https://stackoverflow.com/questions/3025125/cpu-and-data-alignment),[2](http://www.nic.uoregon.edu/~khuck/ts/acumem-report/manual_html/ch03s02.html), [4](https://stackoverflow.com/questions/8469427/how-and-when-to-align-to-cache-line-size), [5](https://thewolfsound.com/what-is-data-alignment/), [6](https://www.youtube.com/watch?v=Y7q2ECeFWE8), [7](https://www.youtube.com/watch?v=xNxjRW1ZYng)

Data is transferred between memory and cache in blocks of fixed size, called **cache lines** or **cache blocks**. A typical size for this seems to be 64 bytes.

On Linux you can get cache line size by checking  [sysconf(3)](https://man7.org/linux/man-pages/man3/sysconf.3.html).

```cpp
#include <unistd.h>
std::cout<<sysconf (_SC_LEVEL1_DCACHE_LINESIZE)<<std::endl;
```

You can get cache line size by using `getconf`:

`getconf LEVEL1_DCACHE_LINESIZE`

Data that are located closer to each other than this may end up on the same cache line.

If these data are needed by different cores, the system has to work hard to keep the data consistent between the copies residing in the cores' caches. Essentially, while one thread modifies the data, the other thread is blocked by a lock from accessing the data.


memory is loaded into the CPU cache in chunks called **cache lines**. This takes time,
and generally speaking the more cache lines loaded for your object, the longer it takes.
Otherwise, you might get away with sometimes only having part of your object in cache,
and the rest in main memory.


When the processor needs to read or write a location in memory, it first checks for a corresponding entry in the cache.
The cache checks for the contents of the requested memory location in any cache lines that might contain that address. If the processor finds that the memory location is in the cache, a cache hit has occurred. However, if the processor does not find the memory location in the cache, a cache miss has occurred. In the case of a cache hit, the processor immediately reads or writes the data in the cache line. For a cache miss, the cache allocates a new entry and copies data from main memory, then the request is fulfilled from the contents of the cache.
# Alignment

Every object type has the property called **alignment requirement**, which is an integer value (of type std::size_t, always a power of 2) representing the number of bytes between successive addresses at which objects of this type can be allocated.

The alignment requirement of a type can be queried with `alignof` or `std::alignment_of`

## Querying the alignment of a type


```cpp
std::cout<<"align of char: " <<alignof(char)  <<std::endl;
std::cout<<"align of int: " << alignof(int)  <<std::endl;
std::cout<<"align of foo1: " << alignof(foo1)  <<std::endl;
```

stricter alignment can be requested using `alignas`.


Every object type has the property called alignment requirement, which is an integer value (of type std::size_t, always a power of 2) representing the number of bytes between successive addresses at which objects of this type can be allocated.




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
```


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
Refs: [1](https://www.youtube.com/watch?v=BP6NxVxDQIs), [2](https://gcc.gnu.org/onlinedocs/gcc/Structure-Layout-Pragmas.html)
[3](https://stackoverflow.com/questions/14179748/what-is-the-difference-between-pragma-pack-and-attribute-aligned), [4](https://stackoverflow.com/questions/3318410/pragma-pack-effect)

#pragma pack(n) is roughly equivalent to __attribute__((packed,aligned(n))): 


[source code](../src/align.cpp)
