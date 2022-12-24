# [C++](#)

<!---[![Build Status](https://app.travis-ci.com/behnamasadi/cpp_tutorials.svg?branch=master)](https://app.travis-ci.com/behnamasadi/cpp_tutorials)-->
[![Build status](https://ci.appveyor.com/api/projects/status/i4omlfovqaswpj41/branch/master?svg=true)](https://ci.appveyor.com/project/behnamasadi/cpp-tutorials/branch/master)
![build workflow](https://github.com/behnamasadi/cpp_tutorials/actions/workflows/cmake.yml/badge.svg)
![alt text](https://img.shields.io/badge/license-BSD-blue.svg)

This repository contains my C++ snippets code on **C++ concepts/ idioms**, **optimized C++**, **modern C++** and **advance C++**. I have included snippets and samples code for using third party libs for parsing **CSV**, **YAML** and **JASON** files. Example of code benchmarking with **Google Benchmark** is available. There is also a tutorial on using **CMake** for building and exporting your project.

## Building and Installation
### 1. Building the Image
There is docker file for this project where contains all dependencies and you build the image with :   

`docker build -t cpp_tutorials .`

### 2. Creating the container
Create a container where you mount the checkout code into your container: 

`docker run --name <continer-name> -v <checked-out-path-on-host>:<path-in-the-container> -it <docker-image-name>`

for instance:

`docker run --name cpp_container -v /home/behnam/workspace/cpp_tutorials:/cpp_tutorials -it cpp_tutorials`

### 3. Starting an existing container
If you have already created a container from the docker image, you can start it with:

`docker start -i cpp_container`

### 4. Removing  unnecessary images and containers
You can remove unnecessary images and containers by:

`docker image prune -a`

`docker container prune` 


### GUI application with docker
1. You need to run:

`docker run --name cpp_container -v /home/behnam/workspace/cpp_tutorials:/cpp_tutorials --env="DISPLAY" --env="QT_X11_NO_MITSHM=1" --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw"  -it cpp_tutorials`

2. On the host run the following (every time you run your container):

`export containerId=$(docker ps -l -q)`

<code>  xhost +local: docker inspect --format='{{ .Config.Hostname }}' $containerId </code>


read more [here](https://ros-developer.com/2017/11/08/docker/)

## [C++ Tutorials](#)
   * [Align](docs/align.md)  
   * [Allocator](docs/allocator.md)  
   * [Algorithms library](src/algorithms_library.cpp)  
   * [Assert](docs/assert.md)  
   * [Basic IO Operation, filesystem, Streams, Reading/Writing Files, Formating Output, cin, scanf, gets, getline, printf](docs/basic_IO_operation.md)  
   * [Bitset, Bit field, Bitwise Operations](docs/bitset_bit_field_bitwise_operations.md)  
   * [Callbacks, Callable Objects, std::function, std::bind, std::invoke, Lambda](docs/callbacks.md)  
   * [Clock, Date, Time](src/date_time.cpp)   
   * [Conditional Compilation](src/conditional_compilation.cpp)  
   * [Containers](docs/containers.md)  
      * [vector](docs/vector.md)  
      * [lists](src/lists.cpp)  
      * [C arrays, std::array, span](docs/array_span.md)  
      * [set, map, pair, tuple, tie, unordered_map, multimap, unordered_set, multiset](docs/set_map_pair_tuple.md)  
      * [stack, queue, priority_queue, deque](docs/stack_queue_priority_queue_deque.md) 
   * [Const, Constexpr and Mutable](docs/const_constexpr_mutable.md)    
   * [Data Types, Numerical Limits, Precision](docs/primitive_data_types_numerical_limits_accuracy.md)    
   * [Data Types Conversions, Casting](docs/type_conversions_casting.md)  
   * [Dynamic Memory Allocation in C](docs/dynamic_memory_allocation.md)  
   * [Debugging Core Dumps](docs/debugging_core_dumps.md)  
   * [Enum](docs/enum.md)  
   * [Error Handling](docs/error_handling.md)  
   * [Exception Handling, noexcept](docs/exception_handling.md)  
   * [Extern Variables, Extern Functions](src/extern/variable/main.cpp)  
   * [Forking](docs/fork.md)  
   * [Forwarding Reference](docs/forward.md)  
   * [Functions, Extern Function, Function Objects, Function Pointer, Inline Functions](docs/functions.md)  
   * [Hash](src/hash.cpp)    
   * [Heap and Stack, Memory Layout of C Programs](docs/heap_and_stack_memory_layout_of_C_programs.md)  
   * [Iterator, for_each loop, range-for loop, Loop optimization](docs/iterator_loop.md)  
   * [Lambda](docs/lambda.md)     
   * [Literals](docs/literals.md)  
   * [Loading Classes Dynamically From Plugins](docs/loading_classes_dynamically_from_plugins.md)  
   * [Macro](src/macro.cpp)  
   * [Memory Error Detection With Memory Address Sanitizer and Valgrind](docs/memory_leaking_valgrind.md)  
   * [Nested Namespaces, Unnamed/Anonymous Namespaces](docs/nested_unnamed_anonymous_namespaces.md)     
   * [Parameter Pack Expansion ...](docs/parameter_pack_expansion_(...).md)  
   * [Register Keyword](docs/register.md)  
   * [Pseudo-random Number Generation, Distributions](docs/random_number.md)
   * [Raw Pointers, Smart (Shared, Unique, Weak) Pointers, Reference, addressof, reference_wrapper, std::ref](docs/pointers.md)  
   * [Return, Abort, Exit](docs/return_abort_exit.md)  
   * [Scope Resolution Operator](docs/scope_resolution_operator.md)  
   * [Scope and Life Cycle of Objects Returned by Functions](docs/scope_and_life_cycle_of_objects_returned_by_functions.md)  
   * [Setting argc, argv](src/set_argv_argc.cpp)  
   * [Simple Command Argument Parser](src/arguments_parser.cpp)  
   * [Signals](docs/signals.md)  
   * [System Call](docs/system_call.md)  
   * [String C/C++, string_view, string literal, string conversion, ASCII, Unicode](docs/string.md)  
   * [Structs](src/structs.cpp)  
   * [Switch Case](src/switch_case.cpp)  
   * [Template, Expression templates](docs/templates.md)  
   * [Ternary (One Line If)](src/ternary.cpp) 
   * [Temporary Objects](docs/temporary_objects.md)    
   * [Track memory allocations(overriding new operator)](docs/track_memory_allocations_overriding_new_operator.md)  
   * [Trivial, standard-layout, Passive/ Plain old data structure (PDS)/ (POD), and literal types](docs/trivial_standard_pds_pod_layout.md)  
   * [Type Traits](src/type_traits.cpp)
   * [Typedef, Type alias (using keyword))](src/typedef.cpp)
   * [type_dispatch, integral_constant, true/false type](src/type_dispatch_integral_constant_true_false_type.cpp)    
   * [Unions](docs/union.md)  
   * [Variadic Templates Function](src/variadic_templates.cpp)     
   * [Volatile Keyword](docs/volatile.md)  
- [C++ Classes](class/)
   * [Abstract Class Vs Interface](src/class/abstract_class_vs_interface.cpp)   
   * [Cast Base Class to Derived/  Derived to Base](src/class/cast_Base_to_Derived_to_Base.cpp)   
   * [Class Forward Declaration](docs/class_forward_declaration.md)  
   * [Class Constructor Initializationlist](src/class/constructor_initialization_list.cpp)  
   * [Class Constructor Aggregate/ Copy/ Default/ Direct/ Value/ Uniform/ Zero Initialization, Initializer List](docs/aggregate-copy-default-direct-value-zero.md)  
   * [Copy/ Move Constructor, rvalue, lvalue, move semantic](docs/copy_constructor_move_constructor_rvalue_lvalue_move_semantic.md)  
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
- [Multithreading and Process](#)
   * [Process, Inter Process Communication (IPC), Process Tree, Process Control Block (PCB](docs/process.md)  	
   * [Creating (Thread Callable Objects), Terminating Threads](docs/multithreading.md#creation_and_termination)  
   * [Over Subscription](docs/multithreading.md#over_subscrition)  
   * [Differentiating Between Threads](docs/multithreading.md#differentiating_between_threads)  
   * [Sleeping Threads](docs/multithreading.md#sleeping_threads)  
   * [Joining/ Detaching Threads, Joinable Detachable](docs/multithreading.md#join_detach_threads)  
   * [Threads Yield](docs/multithreading.md#yield)  
   * [Thread Synchronization](docs/multithreading.md#thread_synchronization)  
   * [Racing Condition](docs/multithreading.md#racing_condition)  
   * [Mutex](docs/multithreading.md#mutex)  
   * [Semaphor](docs/multithreading.md#semaphor)  
   * [Thread Safe](docs/multithreading.md#thread_safe)  
   * [Dead Lock](docs/multithreading.md#dead_lock)  
   * [Lock Guard](docs/multithreading.md#lock_guard)  
   * [Scoped Lock](docs/multithreading.md#scoped_lock)  
   * [Unique Lock](docs/multithreading.md#unique_lock)  
   * [Condition Variable](docs/multithreading.md#condition_variable)  
   * [Future, Promise and async](docs/multithreading.md#async_future_promise)  
   * [Packaged Task](docs/multithreading.md#packaged_task)  
   * [Thread design pattern](docs/thread_design_pattern.md)  
   
   

## [Advance C++ Concepts/ Idiom](#)

   * [Argument-dependent lookup](src/argument_dependent_lookup.cpp)  
   * [Buffer overflow](src/buffer_overflow.cpp)  
   * [Copy and Swap](docs/copy-and-swap_idiom.md)  
   * [Static Casting, Dynamic Casting](src/cast.cpp)  
   * [Run-Time Type Information(RTTI)](src/RTTI.cpp)  
   * [Curiously Recurring Template Pattern (CRTP)](src/CRTP.cpp)  
   * [Copy Elision, Return value optimization (RVO,NRVO)](src/RVO_NRVO_copy_elision.cpp)  
   * [Double Dispatch](src/)  
   * [Most vexing parse](src/most_vexing_parse.cpp)  
   * [Metaprogramming](docs/metaprogramming.md)  
   * [Pointer to implementation (PIMPL)](src/pimpl/)  
   * [Return Type Resolver](src/return_type_resolver.cpp)
   * [Resource Acquisition Is Initialization (RAII)](src/RAII.cpp)  
   * [Substitution failure is not an error (SFINAE)](src/SFINAE.cpp)    
   * [Stack overflow](docs/stack_overflow.md)    
   * [Stack Unwinding](src/stack_unwinding.cpp)    
   * [Type Erasure](docs/type_erasure.md)
   * [Template Specialization, Tag Dispatch](src/template_specialization_tag_dispatch.cpp)
   * [Virtual Method Table(VTABLE), Virtual Table Pointer(VPTR)](docs/VTABLE_and_VPTR.md)  
   * [Virtual address space](docs/virtual_address_space.md)
   * [Tricky Questions](src/tricky_questions.cpp)  
   
## [Optimizing C++](src/optimizing_cpp)
## [Data File Storage Parsing](#)
   * [CSV ](src/third_party_tools/csv)
   * [YAML ](src/third_party_tools/yaml)
   * [JASON ](src/third_party_tools/jason)
   * [XML ](src/third_party_tools/xml/)
## [Code Benchmarking](src/third_party_tools/benchmark)
   * [Google Benchmark ](src/third_party_tools/benchmark)  
## [C++ Coding Standards and Style Guide](#)  
   * [Goolge C++ Style Guide](https://google.github.io/styleguide/cppguide.html)  
   * [isocpp](https://isocpp.org/wiki/faq/coding-standards)  
   * [Bjarne Stroustrup's C++ Style](https://www.stroustrup.com/bs_faq2.html)  
   
## [C++ Package Manager](#)   
   * [Conan](docs/conan.md)  
   * [vcpkg](docs/vcpkg.md)  
   
   


  

