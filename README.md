# [C++](#)

[![Windows](https://ci.appveyor.com/api/projects/status/github/behnamasadi/cpp_tutorials?branch=master&svg=true)](https://ci.appveyor.com/project/behnamasadi/cpp-tutorials)
![Ubuntu](https://github.com/behnamasadi/cpp_tutorials/actions/workflows/docker-build.yml/badge.svg)
![Windows](https://github.com/behnamasadi/cpp_tutorials/actions/workflows/windows-build.yml/badge.svg)
![alt text](https://img.shields.io/badge/license-BSD-blue.svg)
![GitHub Issues or Pull Requests](https://img.shields.io/github/issues/behnamasadi/cpp_tutorials)
![GitHub Release](https://img.shields.io/github/v/release/behnamasadi/cpp_tutorials)
![GitHub Repo stars](https://img.shields.io/github/stars/behnamasadi/cpp_tutorials)
![GitHub forks](https://img.shields.io/github/forks/behnamasadi/cpp_tutorials)

This repository contains my C++ snippets on **C++ concepts/idioms**, **optimized C++**, **modern C++** and **advanced C++**. It includes snippets for using third-party libraries to parse **CSV**, **YAML**, and **JSON** files, an example of code benchmarking with **Google Benchmark**, and a tutorial on using **CMake** to build and export your project.

## Building

See **[BUILDING.md](BUILDING.md)** for CMake configuration, Docker usage (including GUI apps), and VSCode setup.

## C++ Tutorials

### Language Essentials

- [Data Types, Numerical Limits, Machine Epsilon, Precision, std::nan](docs/primitive_data_types_numerical_limits_machine_epsilon_precision.md)
- [Data Type Conversions, Casting, Type Coercion](docs/type_conversions_casting_type_coercion.md)
- [Static Casting, Dynamic Casting](src/cast.cpp)
- [Const, Constexpr, consteval, constinit, Mutable](docs/const_constexpr_mutable.md)
- [Immutable Objects](docs/immutable_objects.md)
- [Decay](docs/decay.md)
- [Enum](docs/enum.md)
- [Functions, Function Objects, Function Pointer, Inline Functions](docs/functions.md)
- [Extern Variables, Extern Functions](docs/extern.md)
- [Lambda](docs/lambda.md)
- [Callable Objects, Callbacks](docs/callable_callbacks.md)
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
- [References, Lvalue/Rvalue, reference_wrapper, std::ref / std::cref](docs/references.md)
- [Forwarding Reference](docs/forward.md)
- [Move Semantics, Copy Constructor, Move Constructor, Assignment](docs/copy_constructor_move_constructor.md)
- [Scope Resolution Operator](docs/scope_resolution_operator.md)
- [Scope and Life Cycle of Objects Returned by Functions](docs/scope_and_life_cycle_of_objects_returned_by_functions.md)
- [Nested Namespaces, Unnamed/Anonymous Namespaces](docs/nested_unnamed_anonymous_namespaces.md)
- [Macros](docs/macros.md)
- [Literals](docs/literals.md)
- [Numeral Systems in C++ Decimal, Binary, Octal, Hexadecimal](docs/numeral_system.md)
- [Big-endian, Little-endian](docs/big-endian_little-endian.md)
- [Bitset, Bit field, Bitwise Operations](docs/bitset_bit_field_bitwise_operations.md)
- [Post-increment, Pre-increment, Unary plus](docs/post-increment_pre-increment.md)
- [Ternary (One Line If)](src/ternary.cpp)
- [Switch Case](src/switch_case.cpp)
- [Setting argc, argv](src/set_argv_argc.cpp)
- [Simple Command Argument Parser](src/arguments_parser.hpp)
- [Assert](docs/assert.md)
- [Attribute specifier sequence `[[attribute-list]]`](docs/attribute_specifier_sequence.md)
- [Register Keyword](docs/register.md)
- [Volatile Keyword](docs/volatile.md)
- [Signals](docs/signals.md)
- [System Call](docs/system_call.md)
- [Three-way comparison `<=>` (C++20)](docs/three_way_comparison.md)
- [Structured binding declaration](docs/structured_binding_declaration.md)

### Memory & Pointers

- [Heap and Stack, Memory Layout of C Programs](docs/heap_and_stack_memory_layout_of_C_programs.md)
- [Dynamic Memory Allocation in C](docs/dynamic_memory_allocation.md)
- [Raw Pointers, Wild/Dangling, Memory Safety](docs/pointers.md)
- [Smart Pointers (unique_ptr, shared_ptr, weak_ptr, atomic smart pointers)](docs/smart_pointers.md)
  - [When to Use shared_ptr — 12 Concrete Patterns](docs/shared_ptr_use_cases.md)
  - [Passing / Returning Smart Pointers to/from Functions](docs/passing_returning_smart_pointers_to_from_functions.md)
  - [Smart Pointers as Class Members (PIMPL, getters, worked examples)](docs/smart_pointers_class_member.md)
- [Memory Alignment (`alignas`, `alignof`, cache lines, Eigen)](docs/align.md)
- [Allocator](docs/allocator.md)
- [Track Memory Allocations (overriding new operator)](docs/track_memory_allocations_overriding_new_operator.md)

### Containers, Iteration, Ranges

- [Containers](docs/containers.md)
  - [vector](docs/vector.md)
  - [lists](src/lists.cpp)
  - [C arrays, std::array, std::span](docs/array_span.md)
  - [set, map, pair, tuple, tie, unordered_map, multimap, unordered_set, multiset](docs/set_map_pair_tuple.md)
  - [Queue, Priority queue, deque](docs/queue_priority_queue_deque.md)
  - [Stack](docs/stack.md)
- [Iterator, for_each loop, range-for loop, Loop optimization](docs/iterator_loop.md)
- [Algorithms Library](docs/algorithms.md)
- [Ranges and Views (C++20)](docs/ranges.md)
- [Execution Policies](docs/execution_policies.md)
- [Hash Functions, Hash Data Structure (Hash Table)](docs/hash_function_hash_table.md)
- [Function objects: std::less, std::greater, std::not1, std::unary_negate](docs/std_greater_less.md)
  - [std::not1, std::unary_negate](docs/not1_unary_negate.md)

### Strings, I/O, and Formatting

- [String C/C++, string_view, string literal, string conversion, ASCII, Unicode](docs/string.md)
- [String View](docs/string_view.md)
- [Basic IO Operation, Streams, Reading/Writing Files, cin, scanf, gets, getline, printf](docs/basic_IO_operation.md)
- [std::format and std::print (C++20/23)](docs/format.md)
- [File System](docs/filesystem.md)
- [Regex](docs/regex.md)
- [Pseudo-random Number Generation, Distributions](docs/random_number.md)
- [Clock, Date, Time](docs/date_time.md)

### Concurrency, Async, and Coroutines

- [Process, Inter Process Communication (IPC), Process Tree, Process Control Block (PCB)](docs/process.md)
- [Forking](docs/fork.md)
- [Multithreading — Overview](docs/multithreading.md#1-overview)
  - [Creating and Terminating Threads (function pointer, member function, functor, lambda)](docs/multithreading.md#2-creating-and-terminating-threads)
  - [Joining and Detaching Threads](docs/multithreading.md#27-joining-threads)
  - [std::jthread and std::stop_token (C++20)](docs/multithreading.md#29-stdjthread-c20)
  - [Hardware Concurrency and Oversubscription](docs/multithreading.md#32-hardware-concurrency-and-oversubscription)
  - [Thread Identity, Sleeping, Yielding](docs/multithreading.md#3-thread-properties)
  - [Race Conditions vs Data Races](docs/multithreading.md#411-race-condition-vs-data-race)
  - [Mutex (std::scoped_lock, lock_guard, unique_lock)](docs/multithreading.md#42-mutex)
  - [Deadlock and How to Avoid It](docs/multithreading.md#423-deadlock)
  - [Semaphores (counting, binary)](docs/multithreading.md#43-semaphores)
  - [Condition Variables and Spurious Wakeups](docs/multithreading.md#44-condition-variables)
  - [std::future, std::async, std::promise, std::packaged_task](docs/multithreading.md#5-async-tasks-and-stdfuture)
  - [std::atomic, Memory Ordering, ABA Problem](docs/multithreading.md#6-stdatomic)
  - [Designing Thread-Safe Classes](docs/multithreading.md#7-designing-thread-safe-classes)
- [Atomic Operations and Atomic Types](docs/atomic.md)
- [Asynchronous Programming](docs/asynchronous_programming.md)
  - [std::launch::async, std::future](docs/asynchronous_programming.md#std--launch--async--std--future)
  - [Parallelization with async](docs/asynchronous_programming.md#parallelization-with-ascync)
  - [Parallelization with std::packaged_task](docs/asynchronous_programming.md#parallelization-with-std--packaged-task)
- [Packaged Task](docs/packaged_task.md)
- [Event handling / Concurrency / Thread Design Patterns](docs/thread_design_pattern.md)
- [Coroutines (C++20)](docs/coroutines.md)

### Templates and Generics

- [Templates, Expression Templates](docs/templates.md)
- [Concepts (C++20)](docs/concepts.md)
- [Template Specialization, Tag Dispatch](docs/template_specialization.md)
- [Variadic Templates Function](docs/variadic_templates.md)
- [Parameter Pack Expansion `...`](<docs/parameter_pack_expansion_(...).md>)
- [Substitution failure is not an error (SFINAE)](docs/SFINAE.md)
- [Template argument deduction](https://en.cppreference.com/w/cpp/language/template_argument_deduction)
- [Type Traits](docs/type_traits.md)
- [Type Erasure](docs/type_erasure.md)
- [Typedef, Type alias (using keyword)](docs/typedef.md)
- [type_dispatch, integral_constant, true/false type](src/type_dispatch_integral_constant_true_false_type.cpp)
- [Argument-dependent lookup (ADL)](docs/argument_dependent_lookup.md)

### Error Handling

- [Error Handling](docs/error_handling.md)
- [Error Code](docs/error_code.md)
- [Exception Handling, noexcept](docs/exception_handling.md)
- [Return, Abort, Exit, Throw, Terminate](docs/return_abort_exit_throw_terminate.md)
- [Stack Unwinding](docs/stack_unwinding.md)

### Modules and Build

- [C++20 Modules](docs/cpp_modules.md)
- [C++ Translation Units](docs/translation_units.md)
- [Conditional Compilation From CMakeLists](docs/conditional_compilation.md)
- [Printing List of All Included Headers](docs/print_all_included_headers.md)
- [fPIE (Position Independent Executable) and fPIC (Position Independent Code)](docs/fPIE_and_fPIC.md)

### Misc Utilities

- [Optional, Variant, Any — when to use which](docs/any_variant_optional.md)
  - [Optional](docs/optional.md)
  - [Variant](docs/std_variant.md)
  - [Visit](docs/std_visit.md)
  - [Any](docs/any.md)
- [Loading Classes Dynamically From Plugins](docs/loading_classes_dynamically_from_plugins.md)
- [Lexical Analyzer](docs/lexical_analyzer.md)
- [Tricky Questions](docs/tricky_questions.md)
- [Undefined behavior, Unspecified and Implementation-Defined](docs/undefined_unspecified_implementation_defined.md)
- [Trivial, standard-layout, POD, and literal types](docs/trivial_standard_pds_pod_layout.md)

## C++ Classes

- [Abstract Class Vs Interface](src/class/abstract_class_vs_interface.cpp)
- [Cast Base Class to Derived / Derived to Base](src/class/cast_Base_to_Derived_to_Base.cpp)
- [Class Forward Declaration](docs/class_forward_declaration.md)
- [Class Constructor Initialization List](src/class/constructor_initialization_list.cpp)
- [Class Constructor Aggregate / Copy / Default / Direct / Value / Uniform / Zero Initialization](docs/initialization.md)
- [Copy Constructor, Copy Assignment, Move Constructor, Assignment](docs/copy_constructor_move_constructor.md)
- [Shallow Copy, Deep Copy](docs/shallow_copy_deep_copy.md)
- [Cyclic (Circular) Dependency](docs/circular_dependencies.md)
- [Default(=default), Deleted (=delete) Constructors](docs/default_constructors_=default_0_delete.md)
- [Diamond Problem Virtual Inheritance](src/class/diamond_problem_virtual_inheritance.cpp)
- [Explicit Constructor, Converting Constructor](docs/explicit_constructor.md)
- [Friend Functions](src/class/friend_functions.cpp)
- [Header Guard](src/class/header_guard)
- [Inheritance, Inheritance Prevention (final)](src/class/inheritance.cpp)
- [Multiple Inheritance Polymorphism](src/class/multiple_inheritance_polymorphism.cpp)
- [Operator Overloading](docs/operator_overloading.md)
- [Object Slicing](src/class/object_slicing.cpp)
- [Passing Arguments To Parent Constructor](src/class/passing_arg_to_parent_constructor.cpp)
- [Private Public Protected Inheritance](src/class/private_public_protected_inheritance.cpp)
- [Protected Friend Class Function](src/class/protected_friend_class_function.cpp)
- [Shadowing](src/class/shadowing.cpp)
- [Static Member Function, Static Initialization Order Fiasco](docs/static_member_function_order_fiasco.md)
- [Special Member Functions](docs/class_special_member_functions.md)
- [Structs](src/structs.cpp)
- [Virtual Function Abstract Class](docs/virtual_function_abstract_class.md)
- [Virtual Destructor, Virtual Constructor](docs/virtual_destructor_virtual_constructor.md)

## Advanced C++ Concepts and Idioms

- [Buffer overflow](docs/buffer_overflow.md)
- [Copy and Swap](docs/copy-and-swap_idiom.md)
- [Curiously Recurring Template Pattern (CRTP)](src/CRTP.cpp)
- [Copy Elision, Return value optimization (RVO, NRVO)](docs/copy_elision.md)
- [Double Dispatch](docs/double_dispatch.md)
- [Most Vexing Parse](docs/most_vexing_parse.md)
- [Metaprogramming](docs/metaprogramming.md)
- [Pointer to implementation (PIMPL)](docs/PIMPL.md) — see also [smart_pointers_class_member.md §3](docs/smart_pointers_class_member.md#3-the-pimpl-idiom)
- [Return Type Resolver](docs/return_type_resolver.md)
- [Resource Acquisition Is Initialization (RAII)](docs/RAII.md)
- [Rule of 3, Rule of 5](docs/rule_of_3_rule_of_5.md)
- [Run-Time Type Information (RTTI)](docs/RTTI.md)
- [Stack overflow](docs/stack_overflow.md)
- [Temporary Objects](docs/temporary_objects.md)
- [Virtual Method Table (VTABLE), Virtual Table Pointer (VPTR)](docs/VTABLE_and_VPTR.md)
- [Virtual address space](docs/virtual_address_space.md)

## Optimizing C++

- [Optimizing C++ source files](src/optimizing_cpp)

## Tooling and Debugging

- [Compiler Explorer (godbolt.org)](docs/godbolt.md)
- [Sanitizers (ASan, TSan, UBSan, MSan, LSan)](docs/sanitizers.md)
- [Memory Error Detection with AddressSanitizer and Valgrind](docs/memory_leaking_valgrind.md)
- [Generating and Debugging Dump File](docs/generating_and_debugging_dump_file.md)
- [Static analysis and linting with clang-tidy](docs/clang_tidy.md)

## Data File Storage Parsing

- [CSV](docs/csv.md)
- [YAML](docs/yaml-cpp.md)
- [JSON](docs/json.md)
- [XML](docs/tinyxml2.md)

## REST API, Microservices, and Communication Libraries

- [Monolithic Architecture vs REST API and Microservices](docs/microservices/REST_API_microservices.md)
- [REST APIs / Webhooks with cURL (libcurl)](https://github.com/curl/curl)
- [gRPC C++ (official)](https://github.com/grpc/grpc)
- [WebSockets with IXWebSocket](https://github.com/machinezone/IXWebSocket)
- [WebRTC with libdatachannel](https://github.com/paullouisageneau/libdatachannel)
- [GraphQL with cppgraphqlgen](https://github.com/microsoft/cppgraphqlgen)
- [XML SOAP with tinyxml2 + cURL](docs/microservices/xml_soap.md)
- [REST API with crow](docs/microservices/REST_API_with_crow.md)
- [Mocking APIs with Mockoon](docs/microservices/mockoon.md)

## Event Streaming and Message Queuing

- [Apache Kafka](docs/event_streaming_message/apache_kafka.md)
- [RabbitMQ](docs/event_streaming_message/rabbitMQ.md)

## Logging

- [spdlog](docs/spdlog.md)

## Code Benchmarking and Profiling

- [Google Benchmark](docs/google_benchmark.md)
- [Tracy Profiler](docs/tracy_profiler.md)

## C++ Package Managers

- [Conan](docs/conan.md)
- [vcpkg](docs/vcpkg.md)
- [Chocolatey](https://chocolatey.org/install)
- [Winget](https://learn.microsoft.com/en-us/windows/package-manager/winget/)

## Documentation, Packaging, Versioning

- [Doxygen — API documentation, UML diagrams, GitHub Action integration](docs/doxygen.md)
- [CPack Packaging](docs/cpack_packaging.md)
- [Semantic Versioning](docs/semantic_versioning.md)
- [Getting Version From Git in CMake](docs/getting_version_from_git_in_CMake.md)
- [Getting Release Version From Git (check for update)](docs/get_the_release_version_github.md)

## C++ Coding Standards and Style

- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [isocpp Coding Standards](https://isocpp.org/wiki/faq/coding-standards)
- [Bjarne Stroustrup's C++ Style](https://www.stroustrup.com/bs_faq2.html)

## C++ System Design

### Principles and Foundations

- [SOLID Principles in C++](docs/system_design/solid_principles.md)
- [Design a Reusable Library](docs/system_design/design_a_reusable_library.md)
- [API and ABI Design for C++ Libraries (versioning, symbol visibility, stable ABI)](docs/system_design/api_abi_design.md)
- [Exception Safety Guarantees (basic, strong, no-throw)](docs/system_design/exception_safety_guarantees.md)
- [Error Handling Strategies and Fault Tolerance (error codes, exceptions, `std::expected`, `tl::expected`)](docs/system_design/error_handling_strategies.md)
- [Dependency Injection and Inversion of Control](docs/system_design/dependency_injection.md)
- [Resource Management Patterns (RAII, scope guards, `unique_resource`)](docs/system_design/resource_management_patterns.md)

### Design Patterns

- [Object-Oriented Design Patterns (Factory, Singleton, Observer, Strategy, Visitor, Decorator, Adapter)](docs/system_design/design_patterns.md)
- [Concurrency Patterns (Thread Pool, Producer-Consumer, Active Object, Reactor, Proactor)](docs/system_design/concurrency_patterns.md)
  - [Producer–Consumer: backpressure vs. drop](docs/system_design/concurrency_patterns.md#21-backpressure-vs-drop-choosing-the-policy)
  - [Latest-value (mailbox) buffer](docs/system_design/concurrency_patterns.md#22-latest-value-mailbox)
  - [Worked example: bounded queue vs. latest-value](docs/system_design/concurrency_patterns.md#23-worked-example-bounded-queue-vs-latest-value)
  - [Cross-process: ROS2 QoS mapping](docs/system_design/concurrency_patterns.md#24-cross-process-same-patterns-different-transport)
- [Event-Driven Architecture and State Machines (HSM, `boost::sml`)](docs/system_design/event_driven_state_machines.md)
- [Pipeline and Dataflow Architectures](docs/system_design/pipeline_dataflow.md)

### Memory and Performance

- [Memory Management Strategies (Pool, Arena, Slab, Bump/Linear Allocators)](docs/system_design/memory_management.md)
- [Cache-Friendly Data Structures and Data-Oriented Design](docs/system_design/cache_friendly_design.md)
- [False Sharing, Cache Line Padding, NUMA Awareness](docs/system_design/false_sharing_numa.md)
- [Lock-Free Data Structures (SPSC/SPMC/MPMC, ring buffers, hazard pointers, RCU)](docs/system_design/lock_free_data_structures.md)
- [Branch Prediction, Prefetching, and SIMD](docs/system_design/branch_prediction_simd.md)
- [High-Performance Computing (HPC) Patterns (SPMD, fork-join, work stealing)](docs/system_design/hpc_patterns.md)

### Architecture and Modularity

- [Plugin Architecture and Dynamic Loading](docs/system_design/plugin_architecture.md)
- [Modular Design with C++20 Modules and Build-System Architecture](docs/system_design/modular_design.md)
- [Serialization and Deserialization (Protobuf, FlatBuffers, Cap'n Proto, MessagePack)](docs/system_design/serialization.md)
- [Inter-Process Communication and Shared Memory (mmap, Boost.Interprocess)](docs/system_design/ipc_shared_memory.md)
- [Configuration Management (compile-time vs runtime, feature flags)](docs/system_design/configuration_management.md)

### Observability and Testability

- [Logging, Metrics, and Distributed Tracing (OpenTelemetry C++)](docs/system_design/observability.md)
- [Testing Strategies (mocking, fuzzing, property-based, contract testing)](docs/system_design/testing_strategies.md)
- [Designing for Testability (seams, dependency injection, fakes vs mocks)](docs/system_design/designing_for_testability.md)

### Domain-Specific Architectures

- [Real-Time Systems Design (deadlines, priority inversion, deterministic latency)](docs/system_design/realtime_systems.md)
- [Embedded Systems Design (no-heap, no-exceptions, freestanding C++)](docs/system_design/embedded_systems.md)
- [Game Engine Architecture (ECS, scene graphs, frame pacing)](docs/system_design/game_engine_architecture.md)
- [Distributed Systems Patterns in C++ (RPC, sharding, consensus, backpressure)](docs/system_design/distributed_systems_patterns.md)

## VSCode

- [`tasks.json`, `settings.json`, `launch.json`](docs/vscode.md)
