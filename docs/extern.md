In C and C++, the `extern` keyword is used to declare a global variable or function in another file. Here's a comprehensive overview:

### Definition and Usage

1. **Global Variable Declaration**: 
   - `extern` is used to declare a global variable or function in another file. 
   - It's a way of informing the compiler that the variable or function is defined elsewhere.

2. **Function Declaration**:
   - When used with functions, `extern` specifies that the function is defined in another file.

### Syntax

1. **Variable Declaration**: 
   - `extern int var;`
   - This line tells the compiler that the variable `var` is an integer and is defined in some other file.

2. **Function Declaration**: 
   - `extern int func(int, int);`
   - Declares a function `func` which is defined in another file.

### Default Behavior

1. **In C**: 
   - Functions are `extern` by default.
   - When you declare a function without any storage class, it is assumed to be `extern`.

2. **In C++**: 
   - Same as C, functions are `extern` by default.

### Usage in Multi-file Projects

1. **Declaring Global Variables**: 
   - Used to declare variables in header files which are defined in source files.

2. **Sharing Functions Across Files**: 
   - Functions defined in one file can be used in other files via `extern` declarations.

### Linkage

1. **External Linkage**: 
   - Variables and functions with `extern` have external linkage, meaning they can be used in other files.

2. **Opposite Keyword - `static`**:
   - While `extern` extends the visibility to other files, `static` restricts the visibility within the file.

### Examples

1. **Variable Example**:
   - File1.c: `int var = 10;`
   - File2.c: `extern int var;` (Access `var` from File1.c)

2. **Function Example**:
   - File1.c: `void foo() { /* definition */ }`
   - File2.c: `extern void foo();` (Access `foo` from File1.c)

### Common Practices

1. **Header Files**: 
   - Declaring `extern` variables and functions in header files is common to ensure that all source files which include the header are aware of these declarations.

2. **Preventing Multiple Definitions**:
   - Helps in managing large code bases where functions and variables are spread across multiple files.

### Limitations and Considerations

1. **Definition Required**: 
   - `extern` declares but does not define. So, there must be a definition in some other file.

2. **Linker Errors**:
   - If the `extern` variable or function is not defined in any of the linked files, it will lead to linker errors.

3. **Initialization**:
   - `extern` can't be used to initialize variables. It's only for declaration.

### Best Practices

1. **Avoid Global Variables**: 
   - Use `extern` sparingly for variables. Prefer function parameters or other methods to share data.

2. **Clear Module Interfaces**:
   - Use `extern` to clearly define module interfaces in header files, making the code easier to read and maintain.

### C vs. C++

- **Usage Similarity**: 
  - The usage in C and C++ is mostly similar, with differences primarily in language features and not in the concept of `extern`.

- **Name Mangling in C++**: 
  - In C++, `extern "C"` is used to prevent C++ name mangling when calling C functions.

### `extern "C"` in C++

- **Purpose**: 
  - Used to tell C++ compiler to use C-style naming and linkage for the specified code.
- **Common Use**: 
  - When including C headers in C++ programs or calling C functions from C++.


[code](../src/extern/variable/main.cpp)
