# fPIE (Position Independent Executable) and fPIC(Position Independent Code and)

## -fPIC (Position Independent Code)

- **Purpose**: The `-fPIC` option is used when compiling shared libraries. It generates code that can be loaded at any memory address without needing modification. This is crucial for shared libraries because it allows multiple programs to share the same library code in memory, reducing the overall memory footprint.

- **How It Works**: When compiled with `-fPIC`, the code uses relative addressing for data and function references. This means that the code does not assume a fixed address for its execution and can be relocated in memory without issue.

- **Usage**: Typically used for shared libraries (.so files on Linux, .dylib files on macOS) because these libraries need to be shared between different programs and possibly loaded at different addresses in each program's address space.



### Without `-fPIC`

- **Absolute Addressing**: When you compile code without `-fPIC`, the compiler generates machine code that uses absolute addressing. This means the code contains direct, fixed memory address references for functions, variables, and data. This type of code expects to be loaded at a specific address in memory to function correctly.

- **Relocation Overhead**: If the code is loaded at a different address than it was compiled for, the operating system must perform relocations. This means adjusting the absolute addresses in the code to reflect where it has actually been loaded. This process can be time-consuming and increases the load time of the executable or library.

- **Multiple Copies in Memory**: Without `-fPIC`, each process that uses the shared library may require its own copy of the library in memory if the addresses need to be adjusted differently for each process. This results in higher memory usage.

### With `-fPIC`

- **Relative Addressing**: Compiling with `-fPIC` instructs the compiler to generate code using relative addressing instead of absolute. This means the code uses offsets from the program's current address rather than fixed addresses. As a result, the code can be loaded at any memory address without requiring relocation.

- **Efficiency and Sharing**: Since the code can be loaded anywhere in memory without modifications, it's more efficient for the operating system to manage. Shared libraries compiled with `-fPIC` can be shared between multiple processes more easily, reducing memory usage. Only one copy of the library needs to be loaded into memory, and different programs can use it simultaneously.

- **Compatibility**: Position Independent Code is essential for creating shared libraries on most modern operating systems. It ensures compatibility and efficiency across different systems and software.




### Setting -fPIC flag

```bash
g++ -fPIC -c mysourcefile.cpp
```

Globally set Position Independent Code for all targets:

```cmake
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
```

If you prefer to set POSITION_INDEPENDENT_CODE for a specific target rather than globally, you can do so using the set_target_properties function. Here's an example 

```
add_library(mySpecificLibrary STATIC source_file1.cpp source_file2.cpp)
set_target_properties(mySpecificLibrary PROPERTIES    POSITION_INDEPENDENT_CODE ON)
```

send it as parameter:

```
-DCMAKE_POSITION_INDEPENDENT_CODE=ON
```




## `-fPIE` (Position Independent Executable)

- **Purpose**: The `-fPIE` option is similar to `-fPIC` but is specifically designed for generating executables, not libraries. It instructs the compiler to produce position-independent code for the entire executable, which enhances security through mechanisms like Address Space Layout Randomization (ASLR).

- **How It Works**: Like `-fPIC`, `-fPIE` uses relative addressing for accessing functions and data. This allows the operating system to load the executable at any address in memory, making it harder for attackers to predict the location of specific code or data segments.

- **Usage**: Used when compiling executables that benefit from being position-independent. It's particularly important for systems where security is a concern, as it enables the executable to be loaded at random memory addresses.




## Example

To illustrate the differences in assembly code generated with and without the `-fPIC` (Position Independent Code) option in C++, let's consider a simple example. We'll create a small C++ program and compile it twice: once with `-fPIC` and once without, then we'll use the `objdump` tool to inspect the assembly differences.

### Example C++ Program

Let's start with a simple C++ program that defines a function and uses a static variable. This example will help us see how references to data are handled differently with and without `-fPIC`.

**example.cpp**:
```cpp
#include <iostream>

int counter() {
    static int count = 0;
    return ++count;
}

int main() {
    std::cout << "Counter: " << counter() << std::endl;
    return 0;
}
```

### Compilation Without `-fPIC`

First, let's compile this program without `-fPIC`:

```bash
g++ -o example_nofpic example.cpp
```

### Compilation With `-fPIC`

Now, let's compile the same program with `-fPIC`:

```bash
g++ -fPIC -o example_fpic example.cpp
```

### Inspecting Assembly Differences

To inspect the assembly differences, we can use `objdump` to disassemble the compiled executables or, more precisely, the object files if we want to focus on the function's implementation without the executable's overhead.

Let's disassemble both versions:

**Without `-fPIC`**:
```bash
objdump -d example_nofpic > example_nofpic.asm
```

**With `-fPIC`**:
```bash
objdump -d example_fpic > example_fpic.asm
```

now run: 

```
meld example_fpic.asm example_nofpic.asm
```


