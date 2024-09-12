# [C++](#)

<!---[![Build Status](https://app.travis-ci.com/behnamasadi/cpp_tutorials.svg?branch=master)](https://app.travis-ci.com/behnamasadi/cpp_tutorials)-->
<!---[![Build status](https://ci.appveyor.com/api/projects/status/i4omlfovqaswpj41/branch/master?svg=true)](https://ci.appveyor.com/project/behnamasadi/cpp-tutorials/branch/master)-->
![build workflow](https://github.com/behnamasadi/cpp_tutorials/actions/workflows/docker-build.yml/badge.svg)
![alt text](https://img.shields.io/badge/license-BSD-blue.svg)

This repository contains my C++ snippets code on **C++ concepts/ idioms**, **optimized C++**, **modern C++** and **advance C++**. I have included snippets and sample code for using third-party libs to parse **CSV**, **YAML**, and **JSON** files. An example of code benchmarking with **Google Benchmark** is available. There is also a tutorial on using **CMake** to build and export your project.

## Building and Installation
if you need to update your CMake:

Navigate to [CMake's official](https://cmake.org/download/) website to get the link for the latest source tarball:

```
./bootstrap
make -j$(nproc)
sudo make install
sudo update-alternatives --install /usr/bin/cmake cmake /usr/local/bin/cmake 1 --force
```
### How to build on your machine 
configure it:

```
cmake -G "Ninja Multi-Config"  -S . -B build
```

or specify where to install it:

```
cmake -G "Ninja Multi-Config" -S . -B build -DCMAKE_INSTALL_PREFIX=/path/to/install
```


build it:

```
cmake --build build --config Release
```

or 

```
cmake --build build --config Debug
```

or be more specific:

```
cmake --build build --target all --config Release
```

If you prefer `preset` use:

```
cmake --preset ninja-multi
```
and 

```
cmake --build --preset ninja-multi-debug
```
or 
```
cmake --build --preset ninja-multi-release
```

### 1. Building the Image
There is a docker file for this project that contains all dependencies and you build the image with:   

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


## Configure VSCode to use Ninja Multi-Config for all CMake projects

To configure VSCode to use CMake with the "Ninja Multi-Config" generator for all CMake projects, you can modify the VSCode settings. Here's how you can do it:

1. Open your VSCode workspace or project.
2. Press `Ctrl + ,` to open the settings, or navigate to File > Preferences > Settings.
3. In the settings search bar, type "cmake generator".
4. Locate the "Cmake: Generator" setting.
5. Click on "Edit in settings.json" or manually edit your `settings.json` file.

In your `settings.json` file, add or modify the `"cmake.generator"` setting to specify "Ninja Multi-Config" as the default generator for all CMake projects:

```json
{
    "cmake.generator": "Ninja Multi-Config"
}
```

Save the `settings.json` file.

This change ensures that VSCode uses the "Ninja Multi-Config" generator by default for all CMake projects you work on in that workspace or project.

## [C++ Tutorials](#)
   * [Align](docs/align.md)  
   * [Allocator](docs/allocator.md)  
   * [Algorithms Library](docs/algorithms.md)
   * [Any](docs/any.md)       
   * [Assert](docs/assert.md)   
   * [Atomic operations and Atomic Types](docs/atomic.md)    
   * [Asynchronous programming](docs/asynchronous_programming.md)    
     - [std::launch::async, std::future](docs/asynchronous_programming.md#std--launch--async--std--future)
     - [Parallelization with ascync](docs/asynchronous_programming.md#parallelization-with-ascync)
     - [Parallelization with std::packaged_task](docs/asynchronous_programming.md#parallelization-with-std--packaged-task)
   * [Attribute specifier sequence [[ attribute-list ]] ](docs/attribute_specifier_sequence.md)  
   * [Basic IO Operation, Streams, Reading/Writing Files, Formatting Output, cin, scanf, gets, getline, printf](docs/basic_IO_operation.md)  
   * [Big-endian,_Little-endian](docs/big-endian_little-endian.md)  
   * [Bitset, Bit field, Bitwise Operations](docs/bitset_bit_field_bitwise_operations.md)  
   * [Callable Objects, Callbacks](docs/callable_callbacks.md)  
     - [1. Function Pointers](docs/callable_callbacks.md#1-function-pointers)
     - [2. Functor (Function Objects)](docs/callable_callbacks.md#2-functor--function-objects-)
     - [3. std::function, std::placeholders, and std::bind](docs/callable_callbacks.md#3-std--function--std--placeholders--and-std--bind)
     - [4. Lambda Functions](docs/callable_callbacks.md#4-lambda-functions)
     - [5. Member Function Pointers](docs/callable_callbacks.md#5-member-function-pointers)
     - [6. Signals and Slots](docs/callable_callbacks.md#6-signals-and-slots)
     - [7. std::invoke](docs/callable_callbacks.md#7-std--invoke)
     - [8. Packaged Tasks](docs/callable_callbacks.md#8-packaged-tasks)
     - [9. Coroutines](docs/callable_callbacks.md#9-coroutines)
     - [10. Auto-generated operator() from a Captureless Lambda](docs/callable_callbacks.md#10-auto-generated-operator---from-a-captureless-lambda)
   * [Clock, Date, Time](docs/date_time.md)   
   * [Conditional Compilation From CMakeLists](docs/conditional_compilation.md)  
   * [Containers](docs/containers.md)  
      * [vector](docs/vector.md)  
      * [lists](src/lists.cpp)  
      * [C arrays, std::array, span](docs/array_span.md)  
      * [set, map, pair, tuple, tie, unordered_map, multimap, unordered_set, multiset](docs/set_map_pair_tuple.md)  
      * [stack, queue, priority_queue, deque](docs/stack_queue_priority_queue_deque.md) 
   * [Const, Constexpr and Mutable](docs/const_constexpr_mutable.md)    
   * [Immutable Objects](docs/immutable_objects.md)  
   * [Data Types, Numerical Limits, Machine Epsilon, Precision, std::nan](docs/primitive_data_types_numerical_limits_machine_epsilon_precision.md)    
   * [Data Types Conversions, Casting, Type Coercion](docs/type_conversions_casting_type_coercion.md)  
   * [Decay](docs/decay.md)  
   * [Dynamic Memory Allocation in C](docs/dynamic_memory_allocation.md)  
   * [Enum](docs/enum.md)  
   * [Error Handling](docs/error_handling.md)  
   * [Error Code](docs/error_code.md)     
   * [Exception Handling, noexcept](docs/exception_handling.md)  
   * [Extern Variables, Extern Functions](docs/extern.md)  
   * [Execution Policies](docs/execution_policies.md)     
   * [File System](docs/filesystem.md)  
   * [Forking](docs/fork.md)  
   * [Forwarding Reference](docs/forward.md)  
   * [Functions, Extern Function, Function Objects, Function Pointer, Inline Functions](docs/functions.md)  
   * [Hash Functions, Hash Data Structure (Hash Table)](docs/hash_function_hash_table.md)    
   * [Heap and Stack, Memory Layout of C Programs](docs/heap_and_stack_memory_layout_of_C_programs.md)  
   * [Iterator, for_each loop, range-for loop, Loop optimization](docs/iterator_loop.md)  
   * [std::less, std::greater](docs/std_greater_less.md)   
   * [std::not1 and std::unary_negate](docs/not1_unary_negate.md)   
   * [Lambda](docs/lambda.md)  
   * [Literals](docs/literals.md)  
   * [Loading Classes Dynamically From Plugins](docs/loading_classes_dynamically_from_plugins.md)  
   * [Macros](docs/macros.md)  
   * [Memory Error Detection With Memory Address Sanitizer and Valgrind](docs/memory_leaking_valgrind.md)  
   * [Modules](docs/cpp_modules.md)     
   * [Nested Namespaces, Unnamed/Anonymous Namespaces](docs/nested_unnamed_anonymous_namespaces.md)     
   * [Numeral Systems in C++ Decimal, Binary, Octal, Hexadecimal](docs/numeral_system.md)   
   * [Optional](docs/optional.md)
   * [Parameter Pack Expansion ...](docs/parameter_pack_expansion_(...).md)  
   * [Post-increment, Pre-increment, Unary plus](docs/post-increment_pre-increment.md)     
   * [Packaged Task](docs/packaged_task.md)  
   * [Register Keyword](docs/register.md)  
   * [Regex](docs/regex.md)  
   * [Pseudo-random Number Generation, Distributions](docs/random_number.md)
   * [Raw Pointers, Smart (Shared, Unique, Weak) Pointers, Reference, addressof, reference_wrapper, std::ref](docs/pointers.md)  
   * [Passing/ Returning Smart Pointers To/ From Functions](docs/passing_returning_smart_pointers_to_from_functions.md)  
   * [Smart Pointers Class Member](docs/smart_pointers_class_member.md)  
   * [Return, Abort, Exit, Throw, Terminate](docs/return_abort_exit_throw_terminate.md)  
   * [Scope Resolution Operator](docs/scope_resolution_operator.md)  
   * [Scope and Life Cycle of Objects Returned by Functions](docs/scope_and_life_cycle_of_objects_returned_by_functions.md)  
   * [Setting argc, argv](src/set_argv_argc.cpp)  
   * [Simple Command Argument Parser](src/arguments_parser.hpp)  
   * [Signals](docs/signals.md)  
   * [System Call](docs/system_call.md)  
   * [String C/C++, string_view, string literal, string conversion, ASCII, Unicode](docs/string.md)  
   * [String View](docs/string_view.md)  
   * [Structs](src/structs.cpp)  
   * [Structured binding declaration](docs/structured_binding_declaration.md)  
   * [Switch Case](src/switch_case.cpp)  
   * [Template, Expression Templates](docs/templates.md)  
   * [Template argument deduction](https://en.cppreference.com/w/cpp/language/template_argument_deduction)     
   * [Ternary (One Line If)](src/ternary.cpp) 
   * [Temporary Objects](docs/temporary_objects.md)    
   * [Track memory allocations(overriding new operator)](docs/track_memory_allocations_overriding_new_operator.md)  
   * [Trivial, standard-layout, Passive/ Plain old data structure (PDS)/ (POD), and literal types](docs/trivial_standard_pds_pod_layout.md)  
   * [Type Traits](docs/type_traits.md)
   * [Typedef, Type alias (using keyword)](docs/typedef.md)
   * [type_dispatch, integral_constant, true/false type](src/type_dispatch_integral_constant_true_false_type.cpp)    
   * [Unions](docs/union.md)  
   * [Variant](docs/std_variant.md)  
   * [Visit](docs/std_visit.md)  
   * [Variadic Templates Function](docs/variadic_templates.md)  
   * [Volatile Keyword](docs/volatile.md)  
- [C++ Classes](#)
   * [Abstract Class Vs Interface](src/class/abstract_class_vs_interface.cpp)   
   * [Cast Base Class to Derived/  Derived to Base](src/class/cast_Base_to_Derived_to_Base.cpp)   
   * [Class Forward Declaration](docs/class_forward_declaration.md)  
   * [Class Constructor Initializationlist](src/class/constructor_initialization_list.cpp)  
   * [Class Constructor Aggregate/ Copy/ Default/ Direct/ Value/ Uniform/ Zero Initialization, Initializer List](docs/aggregate-copy-default-direct-value-zero_initialization.md)  
   * [Copy/ Move Constructor, rvalue, lvalue, move semantic](docs/copy_constructor_move_constructor_rvalue_lvalue_move_semantic.md)  
   * [Cyclic (Circular) Dependency](docs/circular_dependencies.md)  
   * [Default(=default), Deleted (=delete) Constructors](docs/default_constructors_=default_0_delete.md)  
   * [Diamond Problem Virtual Inheritance](src/class/diamond_problem_virtual_inheritance.cpp)  
   * [Explicit Constructor, Converting Constructor](docs/explicit_constructor.md)  
   * [Friend Functions](src/class/friend_functions.cpp)  
   * [Header Guard](src/class/header_guard)    
   * [Inheritance, Inheritance Prevention (final)](src/class/inheritance.cpp)  
   * [Multiple Inheritance Polymorphism](src/class/multiple_inheritance_polymorphism.cpp)  
   * [Operator Overloading](docs/operator_overloading.md)  
   * [Object Slicing](src/class/object_slicing.cpp)  
   * [Passing Arguments To Parent Constructor](src/class/passing_arg_to_parent_constructor.cpp)  
   * [Private Public Protected Inheritance](src/class/private_public_protected_inheritance.cpp)  
   * [Protected Friend Class Function](src/class/protected_friend_class_function.cpp)  
   * [Shadowing](src/class/shadowing.cpp)  
   * [Static Member Function, Static Initialization Order Fiasco](docs/static_member_function_order_fiasco.md)  
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
   * [Event handling/ Concurrency/ Thread design pattern](docs/thread_design_pattern.md)  
   
   

## [Advance C++ Concepts and Idioms](#)

   * [Argument-dependent lookup](docs/argument_dependent_lookup.md)  
   * [Buffer overflow](docs/buffer_overflow.md)  
   * [Copy and Swap](docs/copy-and-swap_idiom.md)  
   * [Generating and Debugging Dump File](docs/generating_and_debugging_dump_file.md)     
   * [Static Casting, Dynamic Casting](src/cast.cpp)  
   * [Run-Time Type Information(RTTI)](docs/RTTI.md)  
   * [Curiously Recurring Template Pattern (CRTP)](src/CRTP.cpp)  
   * [Copy Elision, Return value optimization (RVO, NRVO)](docs/copy_elision.md)  
   * [Double Dispatch](docs/double_dispatch.md)  
   * [Most vexing parse](docs/most_vexing_parse.md)  
   * [Metaprogramming](docs/metaprogramming.md)  
   * [Pointer to implementation (PIMPL)](docs/PIMPL.md)  
   * [Return Type Resolver](docs/return_type_resolver.md)
   * [Resource Acquisition Is Initialization (RAII)](docs/RAII.md)  
   * [Rule of 5](docs/rule_of_5.md)  
   * [Substitution failure is not an error (SFINAE)](docs/SFINAE.md)    
   * [Stack overflow](docs/stack_overflow.md)    
   * [Stack Unwinding](docs/stack_unwinding.md)    
   * [Type Erasure](docs/type_erasure.md)  
   * [Template Specialization, Tag Dispatch](docs/template_specialization.md)  
   * [Virtual Method Table(VTABLE), Virtual Table Pointer(VPTR)](docs/VTABLE_and_VPTR.md)  
   * [Virtual address space](docs/virtual_address_space.md)
   * [Tricky Questions](docs/tricky_questions.md)  
   * [C++ Translation Units](docs/translation_units.md)   
   * [Undefined behavior, Unspecified and Implementation-Defined](docs/undefined_unspecified_implementation_defined.md)   
   * [Printing List of All Included Headers](docs/print_all_included_headers.md)   
   * [fPIE (Position Independent Executable) and fPIC(Position Independent Code and) ](docs/fPIE_and_fPIC.md)
   * [Lexical Analyzer](docs/lexical_analyzer.md)     

   
## [Optimizing C++](src/optimizing_cpp)
## [Data File Storage Parsing](#)
   * [CSV](docs/csv.md)  
   * [YAML](docs/yaml-cpp.md)  
   * [JSON](docs/json.md)  
   * [XML](docs/tinyxml2.md)  

## [REST API and Microservices with C++](#)
   * [Monolithic Architecture vs REST API and Microservices](docs/REST_API_microservices.md)   
   * [mock APIs with Mockoon](docs/mockoon.md)
   * [gRPC Microservice](docs/gRPC_microservice.md)   
   * [REST API with Crow](docs/REST_API_with_crow.md)   
   * [Test API with Postman](docs/test_API_with_postman.md)    

## [Logging With C++](#)
   * [spdlog](docs/spdlog.md)  
   
## [Code Benchmarking](#)
   * [Google Benchmark](docs/google_benchmark.md)  

## [Code Profiling](#)
   * [Tracy Profiler](docs/tracy_profiler.md)  
   
## [C++ Package Manager](#)   
   * [Conan](docs/conan.md)  
   * [vcpkg](docs/vcpkg.md)  
   * [Chocolatey](https://chocolatey.org/install)
   * [Winget](https://learn.microsoft.com/en-us/windows/package-manager/winget/)
   
## [Documentation with Doxygen](#)
   * [API documentation and UML diagram](docs/doxygen.md)  	
   * [Integrate Doxygen with GitHub Action](docs/doxygen.md)  	

## [Application Packaging and Deployment](#)
   * [CPack Packaging](docs/cpack_packaging.md)

## [Software versioning](#)
   * [Semantic Versioning](docs/semantic_versioning.md)
   * [Getting Version From Git in CMake](docs/getting_version_from_git_in_CMake.md)  

## [C++ Coding Standards and Style Guide](#)  
   * [Goolge C++ Style Guide](https://google.github.io/styleguide/cppguide.html)  
   * [isocpp](https://isocpp.org/wiki/faq/coding-standards)  
   * [Bjarne Stroustrup's C++ Style](https://www.stroustrup.com/bs_faq2.html)  
