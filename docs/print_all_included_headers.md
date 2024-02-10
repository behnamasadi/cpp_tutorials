# Printing List of All Included Headers

Using the `/showIncludes` flag with CMake to see all the headers included for a specific file in a project primarily compiled with Microsoft Visual C++ (MSVC) involves modifying the CMake configuration to pass this flag to the compiler. The `/showIncludes` option is specific to MSVC and is used to print to stdout a list of all `#include` files as they are processed by the compiler. Here's how you can add this flag for your target or globally within your CMake setup:

### Target-specific Configuration

If you want to apply this flag to a specific target (e.g., a library or executable defined in your CMake project), you can use the `target_compile_options` function. This approach is useful if you only need to trace header dependencies for a particular part of your codebase.

```cmake
add_executable(MyTarget main.cpp) # Or add_library if you're creating a library
target_compile_options(MyTarget PRIVATE "/showIncludes")
```

### Global Configuration

To apply the `/showIncludes` flag globally to all targets in your CMake project, you can modify the `CMAKE_CXX_FLAGS` variable. This approach affects every target in your project, which can be helpful if you're trying to get a comprehensive overview of header dependencies across your entire codebase.

```cmake
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /showIncludes")
```

### Using `-H` with GCC or Clang in CMake

for non-Microsoft compilers like GCC or Clang, you can achieve similar functionality to the MSVC `/showIncludes` flag by using the `-H` option. This option tells the compiler to print the names of header files as they are used during the compilation process. It provides a way to see which headers are included and how they are nested.

To use this option in a CMake project for targets compiled with GCC or Clang, you can add it to your compile options similarly to how you would with MSVC's `/showIncludes`. However, you'll use the `-H` option instead.

#### Target-specific Configuration

For a specific target, use `target_compile_options`:

```cmake
add_executable(MyTarget main.cpp) # Or add_library for a library
target_compile_options(MyTarget PRIVATE "-H")
```

#### Global Configuration

To apply it globally within your CMake project, modify the `CMAKE_CXX_FLAGS`:

```cmake
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -H")
```

### Conditional Compilation Flags

Since different compilers have different options, it's a good practice to conditionally add compiler-specific options to ensure your CMake scripts are portable. Here's an example that checks the compiler being used and applies the appropriate flags:

```cmake
option(PRINT_INCLUDES "Printing List of All Included Headers" OFF)
add_executable(copy_and_swap src/copy_and_swap.cpp)
if(PRINT_INCLUDES)
	if(MSVC)
	    target_compile_options(copy_and_swap PRIVATE "/showIncludes")
	elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	    target_compile_options(copy_and_swap PRIVATE "-H")
	endif()
endif()
```

Now run the followings: 

```
cmake  -DPRINT_INCLUDES=ON --preset ninja-multi  
cmake --build --preset ninja-multi-debug
```

