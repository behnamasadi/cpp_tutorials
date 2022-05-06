# [C++](#)

<!---[![Build Status](https://app.travis-ci.com/behnamasadi/cpp_tutorials.svg?branch=master)](https://app.travis-ci.com/behnamasadi/cpp_tutorials)-->
[![Build status](https://ci.appveyor.com/api/projects/status/i4omlfovqaswpj41/branch/master?svg=true)](https://ci.appveyor.com/project/behnamasadi/cpp-tutorials/branch/master)
![build workflow](https://github.com/behnamasadi/cpp_tutorials/actions/workflows/cmake.yml/badge.svg)
![alt text](https://img.shields.io/badge/license-BSD-blue.svg)

This repository contains my C++ snippets code on **C++ concepts/ idioms**, **optimized C++**, **modern C++** and **advance C++**. I have included snippets and samples code for using third party libs for parsing **CSV**, **YAML** and **JASON** files. Example of code benchmarking with **Google Benchmark** is available. There is also a tutorial on using **CMake** for building and exporting your project.

## [C++ Tutorials](#)
   * [Align](docs/align.md)  
   * [Allocator](src/allocator.cpp)  
   * [Algorithms library](src/algorithms_library.cpp)  

   * [Assert](src/assert.cpp)  
   * [Basic IO Operation, filesystem, Streams, Reading/Writing Files, Formating Output, cin,scanf,gets,getline,printf](docs/basic_IO_operation.md)  
   * [Bindinng](src/bind.cpp)  
   * [Bitset, Bit field](src/bitset_bit_field.cpp)  
   * [Bitwise Operations](src/bitwise_operations.cpp)  
   * [Conditional Compilation](src/conditional_compilation.cpp)  
   * [Containers](docs/containers.md)  
      * [Vector](docs/vector.md)  
      * [Lists](src/lists.cpp)  
      * [C arrays, std::array, span](docs/array_span.md)  
      * [set, map, pair, tuple, unordered_map, multimap, unordered_set, multiset](src/set_map_pair_tuple.cpp)  
      * [stack, queue, priority_queue, deque](stack_queue_priority_queue_deque.md) 
   * [Const, Constexpr and Mutable](docs/const_constexpr_mutable.md)    
   * [Data Types, Numerical Limits, Precision](docs/primitive_data_types_numerical_limits_accuracy.md)    
   * [Data Types Conversions, Casting](docs/type_conversions_casting.md)  
   * [Date, Time](src/date_time.cpp)   
   * [Dynamic Memory Allocation (C Style)](src/dynamic_memory_allocation.cpp)  
   * [Enum](src/enum.cpp)  
   * [Error Handling](src/error_handling.cpp)  
   * [Exception Handling](docs/exception_handling.md)  
   * [Extern Variables](src/extern/variable/main.cpp)  
   * [Forking](src/fork.cpp)  
   * [Forward](src/forward.cpp)  
   * [Functions](docs/functions.md)  
     * [Callbacks, Callable Objects](docs/callbacks.md)  
     * [Extern Function](src/extern/function/main.cpp)  
     * [Function Objects](docs/function_objects.md)  
     * [Function Pointer](docs/function_pointer.md)
     * [Inline Functions](docs/inline_functions.cpp)  
   * [Hash](src/hash.cpp)    
   * [Heap And Stack, Memory Layout of C Programs](docs/heap_and_stack_memory_layout_of_C_programs.md)  
   * [Iterator, for_each loop, range-for loop, Loop optimization](docs/iterator_loop.md)  
   * [Lambda](docs/lambda.md)     
   * [Literals](src/literals.cpp)  
   * [Loading Libraries](src/loadeding_libraries.cpp)  
   * [Macro](src/macro.cpp)  
   * [Memory Leaking Detection With_Valgrind](src/memory_leaking_valgrind.cpp)  
   * [Nested Namespaces, Unnamed/Anonymous Namespaces](docs/nested_unnamed_anonymous_namespaces.md)     
   * [noexcept](src/noexcept_operator_specifier.cpp)  
   * [Register Keyword](src/register.cpp)  
   * [Raw Pointers,  Reference, Smart (Shared, Unique, Weak) Pointers, addressof, reference_wrapper](docs/pointers.md)  
   * [Return Abort Exit](src/return_abort_exit.cpp)  
   * [Scope Resolution Operator](src/scope_resolution_operator.cpp)  
   * [Setting Argc, Argv](src/set_argv_argc.cpp)  
   * [Signals](src/signals.cpp)  
   * [String C/C++, string_view, string literal, string conversion, ASCII, Unicode](docs/string.md)  
   * [Structs](src/structs.cpp)  
   * [Switch Case](src/switch_case.cpp)  
   * [Template, Expression templates](src/template.cpp)  
   * [Ternary (One Line If)](src/ternary.cpp)  
   * [Track Memory Allocations](src/track_memoy_allocations.cpp)  
   * [Trivial, standard-layout, Passive/ Plain old data structure (PDS)/ (POD), and literal types](docs/pds_pod.md)  
   * [Type Traits](src/type_traits.cpp)
   * [Typedef, Type alias (using keyword))](src/typedef.cpp)
   * [type_dispatch, integral_constant, true/false type](src/type_dispatch_integral_constant_true_false_type.cpp)    
   * [Unions](src/unions.cpp)  
   * [Value Categories: rvalue, lvalue, prvalue, xvalue, glvalue](src/rvalue_lvalue.cpp)     
   * [Variadic Templates Function](src/variadic_templates.cpp)     
   * [Volatile Keyword](src/volatile.cpp)  
- [C++ Classes](class/)
   * [Abstract Class Vs Interface](src/class/abstract_class_vs_interface.cpp)   
   * [Cast Base Class To Derived And Derived To Base](src/class/cast_Base_to_Derived_to_Base.cpp)   
   * [Class Forward Declaration](docs/class_forward_declaration.cpp)  
   * [Class Constructor Initializationlist](src/class/constructor_initializationlist.cpp)  
   * [Class Aggregate/ Copy/ Default/ Direct/ Value/ Uniform/ Zero Initialization, Initializer List](docs/aggregate-copy-default-direct-value-zero.md)  
   * [Copy Constructor, Move Constructor](docs/copy_constructor_move_constructor.md)  
   * [Cyclic (Circular) Dependency](src/class/Circular_Dependency)  
   * [Default(=default), Deleted (=delete) Constructors](docs/default_constructors_=default_0_delete.md)  
   * [Diamond Problem Virtual Inheritance](src/class/diamond_problem_virtual_inheritance.cpp)  
   * [Explicit Constructor, Converting Constructor](docs/explicit_constructor.md)  
   * [Friend Functions](src/class/friend_functions.cpp)  
   * [Header Guard](src/class/header_guard)    
   * [Inheritance, Inheritance Prevention (final)](src/class/inheritance.cpp)  
   * [Multiple Inheritance Polymorphism](src/class/multiple_inheritance_polymorphism.cpp)  
   * [Operator Overloading](src/class/operator_overloading.cpp)  
   * [Object Slicing](src/class/object_slicing.cpp)  
   * [Passing Arguments To Parent Constructor](src/class/passing_arg_to_parent_constructor.cpp)  
   * [Private Public Protected Inheritance](src/class/private_public_protected_inheritance.cpp)  
   * [Protected Friend Class Function](src/class/protected_friend_class_function.cpp)  
   * [Shadowing](src/class/shadowing.cpp)  
   * [Static Member Function, Static Initialization Order Fiasco](src/class/static_member_function.cpp)  
   * [Special Member Functions](docs/class_special_member_functions.md)  
   * [Virtual Function Abstract Class](docs/virtual_function_abstract_class.md)  
   * [Virtual Destructor, Virtual Constructor](docs/virtual_destructor_virtual_constructor.md)  
- [Multithreading](src/multithreading/)
   * [Creating (Thread Callable Objects), Terminating Threads](src/multithreading/README.md#creation_and_termination)  
   * [Over Subscrition](src/multithreading/README.md#over_subscrition)  
   * [Differentiating Between Threads](src/multithreading/README.md#differentiating_between_threads)  
   * [Sleeping Threads](src/multithreading/README.md#sleeping_threads)  
   * [Joining/ Detaching Threads, Joinable Detachable](src/multithreading/README.md#join_detach_threads)  
   * [Threads Yield](src/multithreading/README.md#yield)  
   * [Thread Synchronization](src/multithreading/README.md#thread_synchronization)  
   * [Racing Condition](src/multithreading/README.md#racing_condition)  
   * [Mutex](src/multithreading/README.md#mutex)  
   * [Semaphor](src/multithreading/README.md#semaphor)  
   * [Thread Safe](src/multithreading/README.md#thread_safe)  
   * [Dead Lock](src/multithreading/README.md#dead_lock)  
   * [Lock Guard](src/multithreading/README.md#lock_guard)  
   * [Scoped Lock](src/multithreading/README.md#scoped_lock)  
   * [Unique Lock](src/multithreading/README.md#unique_lock)  
   * [Condition Variable](src/multithreading/README.md#condition_variable)  
   * [Future, Promise and async](src/multithreading/README.md#async_future_promise)  
   * [Packaged Task](src/multithreading/README.md#packaged_task)  
   * [Printing Process Tree](src/multithreading/README.md#printing_process_tree)  

## [Advance C++ Concepts/ Idiom](#)

   * [Buffer overflow](src/buffer_overflow.cpp)  
   * [Copy and Swap](docs/copy-and-swap_idiom.md)  
   * [Static Cassting, Dynamic Casting](src/cast.cpp)  
   * [Run-Time Type Information(RTTI)](src/RTTI.cpp)  
   * [Curiously Recurring Template Pattern (CRTP)](src/CRTP.cpp)  
   * [Copy Elision, Return value optimization (RVO,NRVO)](src/RVO_NRVO_copy_elision.cpp)  
   * [Double Dispatch](src/)
   * [Most vexing parse](src/most_vexing_parse.cpp)
   * [Pointer to implementation (PIMPL)](src/pimpl/)  
   * [Return Type Resolver](src/return_type_resolver.cpp)
   * [Resource Acquisition Is Initialization (RAII)](src/RAII.cpp)  
   * [Substitution failure is not an error (SFINAE)](src/SFINAE.cpp)    
   * [Stack overflow](src/stack_overflow.cpp)    
   * [Stack Unwinding](src/stack_unwinding.cpp)    
   * [Type Erasure](src/type_erasure.cpp)
   * [Template Specialization, Tag Dispatch](src/template_specialization_tag_dispatch.cpp)
   * [Virtual Method Table(VTABLE), Virtual Table Pointer(VPTR)](docs/VTABLE_and_VPTR.md)  
   * [Virtual address space](docs/virtual_address_space.md)  
## [Optimizing C++](src/optimizing_cpp)
## [Data File Storage Parsing](#)
   * [CSV ](src/third_party_tools/csv)
   * [YAML ](src/third_party_tools/yaml)
   * [JASON ](src/third_party_tools/jason)
   * [XML ](src/third_party_tools/xml/)
## [Code Benchmarking](src/third_party_tools/benchmark)
   * [Google Benchmark ](src/third_party_tools/benchmark)





