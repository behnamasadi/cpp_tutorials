# Modules

C++20 introduced modules as a significant enhancement to the language, aiming to replace the traditional header-file mechanism. Here's a comprehensive overview of what modules are and how they function in C++20:

1. **Purpose and Advantages**:
    - Modules are intended as an alternative to header files, designed to overcome the limitations of the `#include` preprocessor directive [1](https://grapeprogrammer.com/cpp20_overview_module/).
    - By replacing headers, modules aim to reduce compilation times, isolate macros, and eliminate the need for include guards or pragma once[1](https://grapeprogrammer.com/cpp20_overview_module/).

2. **How They Work**:
    - They turn C++ libraries and programs into components, where each module is a set of source code files compiled separately from the source files that import them  [2](https://learn.microsoft.com/en-us/cpp/cpp/modules-cpp?view=msvc-170#:~:text=In%20this%20article,the%20use%20of%20header%20files).
    - This independence helps to eliminate or significantly reduce the problems traditionally associated with header files, such as repetitive inclusion and compilation overhead.

3. **Structure of Modules**:
    - A module unit is a source file containing a module declaration. There are two primary types of module units:
        - **Module Interface Unit**: It exports a module name with the `export` keyword and specifies which symbols (functions, classes, variables) are available to other parts of the program.
        - **Module Implementation Unit**: It is used to implement the functionality of the modules and does not export symbols directly [2](https://learn.microsoft.com/en-us/cpp/cpp/modules-cpp?view=msvc-170#:~:text=In%20this%20article,the%20use%20of%20header%20files).

4. **Usage**:
    - In practice, a module is similar to a header but also acts as a translation unit. This means that it is compiled separately and only once, which can improve build times and reduce redundancy.
    - You can use the `import` keyword, a new addition to C++20, to gain access to declarations from a module, much like you would include a header file to access its declarations.

5. **Sharing Across Translation Units**:
    - Since many C++ projects consist of multiple translation units, modules provide a way to share declarations and definitions across these units without the pitfalls of header file.

In summary, C++20 modules represent a paradigm shift in how code is organized and shared across different parts of a program, promising to make C++ development more efficient by enhancing compilation times and modularizing codebases.
