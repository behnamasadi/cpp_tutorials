cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(cpp_tutorials)

if(NOT "${CMAKE_GENERATOR}" STREQUAL "Ninja Multi-Config")
    # message(WARNING "It's recommended to use -G \"Ninja Multi-Config\" for this project.")
    # OR, if you want to force-stop the configuration:
    message(FATAL_ERROR "Please use -G \"Ninja Multi-Config\" for this project.")
endif()

option(USE_CLANG "build application with clang" OFF) # OFF is the default

if(USE_CLANG)
    set(CMAKE_C_COMPILER "/usr/bin/clang")

    # set (CMAKE_C_FLAGS                "-Wall -std=c99")
    set(CMAKE_C_FLAGS_DEBUG "-g")
    set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG")
    set(CMAKE_C_FLAGS_RELEASE "-O4 -DNDEBUG")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

    set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
    set(CMAKE_CXX_FLAGS "-Wall")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O4 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

    set(CMAKE_AR "/usr/bin/llvm-ar")
    set(CMAKE_LINKER "/usr/bin/llvm-ld")
    set(CMAKE_NM "/usr/bin/llvm-nm")
    set(CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
    set(CMAKE_RANLIB "/usr/bin/llvm-ranlib")
endif()

set(CMAKE_CXX_STANDARD 20) # Replace 20 with the version number of the latest C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message("using Clang")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message("using GCC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    message("using Intel C++")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message("using Visual Studio C++")

    # set warning level into errors
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    endif()
endif()

message("CMAKE_CXX_COMPILER_VERSION: " ${CMAKE_CXX_COMPILER_VERSION})

add_executable(pointers src/pointers/pointers.cpp)

#add_executable(string  src/string.cpp )

add_executable(exception_handling src/exception_handling.cpp)

#add_executable(heap_vs_stack  src/heap_vs_stack.cpp)

add_executable(assert src/assert.cpp)

add_executable(return_abort_exit src/return_abort_exit.cpp)

add_executable(rvalue_lvalue src/rvalue_lvalue.cpp)

add_executable(primitive_data_types_numerical_limits_accuracy src/primitive_data_types_numerical_limits_accuracy.cpp)

add_executable(arrays src/arrays.cpp)

add_executable(dynamic_memory_allocation src/dynamic_memory_allocation.cpp)

# ADD_LIBRARY(add MODULE src/add.cpp)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_library(add SHARED src/add.cpp)
    add_executable(loadeding_libraries src/loadeding_libraries.cpp)
    target_link_libraries(loadeding_libraries dl)

    add_executable(align src/align.cpp)
    
    add_executable(signals src/signals.cpp)
    add_executable(system_call src/system_call.cpp)

    # add_executable(date_time src/date_time.cpp)

    add_executable(fork src/fork.cpp)
    
endif()

add_executable(template src/template.cpp)

add_definitions(-DLOGING=0)
add_executable(macro src/macro.cpp)

add_executable(inline_functions src/inline_functions.cpp)

add_executable(algorithms_library src/algorithms_library.cpp)

add_executable(vector src/vector.cpp)

add_executable(variadic_templates src/variadic_templates.cpp)

add_executable(enum src/enum.cpp)

add_executable(diamond_problem_virtual_inheritance src/class/diamond_problem_virtual_inheritance.cpp)

add_executable(shadowing src/class/shadowing.cpp)

add_executable(protected_friend_class_function src/class/protected_friend_class_function.cpp)

add_executable(virtual_function_abstract_class src/class/virtual_function_abstract_class.cpp)

add_executable(multiple_inheritance_polymorphism src/class/multiple_inheritance_polymorphism.cpp)

add_executable(private_public_protected_inheritance src/class/private_public_protected_inheritance.cpp)

add_executable(cast_Base_to_Derived_to_Base src/class/cast_Base_to_Derived_to_Base.cpp)

add_executable(operator_overloading src/class/operator_overloading.cpp)

add_executable(default_0_delete_meaning src/class/default_0_delete_meaning.cpp)

add_executable(object_slicing src/class/object_slicing.cpp)

add_executable(static_member_function src/class/static_member_function.cpp)

add_executable(constructor_initialization_list src/class/constructor_initialization_list.cpp)

add_executable(inheritance src/class/inheritance.cpp)

add_executable(switch_case src/switch_case.cpp)

add_executable(set_map_pair_tuple src/set_map_pair_tuple.cpp)

add_executable(error_handling src/error_handling.cpp)

add_executable(bitwise_operations src/bitwise_operations.cpp)

add_executable(bitset_bit_field src/bitset_bit_field.cpp)

add_executable(hash src/hash.cpp)

add_executable(containers src/containers.cpp)

add_executable(iterator_loop src/iterator_loop.cpp)

############################# Advance C++ Concepts/ Idiom #############################
add_executable(RTTI src/RTTI.cpp)

add_executable(CRTP src/CRTP.cpp)

add_executable(double_dispatch src/double_dispatch.cpp)

add_executable(RVO_NRVO_copy_elision src/RVO_NRVO_copy_elision.cpp)

add_executable(pimpl src/pimpl/main.cpp src/pimpl/studentpimpl.cpp)

add_executable(return_type_resolver src/return_type_resolver.cpp)

add_executable(RAII src/RAII.cpp)
target_link_libraries(RAII -pthread)

add_executable(SFINAE src/SFINAE.cpp)

add_executable(type_erasure src/type_erasure.cpp)

add_executable(buffer_overflow src/buffer_overflow.cpp)

add_executable(stack_overflow src/stack_overflow.cpp)

add_executable(stack_unwinding src/stack_unwinding.cpp)

# ############################ Multithreading #############################
add_executable(creating_and_terminating_threads src/multithreading/creating_and_terminating_threads.cpp)
target_link_libraries(creating_and_terminating_threads -pthread)

add_executable(differentiating_between_threads src/multithreading/differentiating_between_threads.cpp)
target_link_libraries(differentiating_between_threads -pthread)

add_executable(sleeping_threads src/multithreading/sleeping_threads.cpp)
target_link_libraries(sleeping_threads -pthread)

add_executable(join_detach_threads src/multithreading/join_detach_threads.cpp)
target_link_libraries(join_detach_threads -pthread)

add_executable(mutex src/multithreading/mutex.cpp)
target_link_libraries(mutex -pthread)

# add_executable(condition_variable src/multithreading/condition_variable.cpp)
# target_link_libraries(condition_variable -pthread)
add_executable(async_future_promise src/multithreading/async_future_promise.cpp)
target_link_libraries(async_future_promise -pthread)

add_executable(dead_lock src/multithreading/dead_lock.cpp)
target_link_libraries(dead_lock -pthread)

add_executable(thread_safe src/multithreading/thread_safe.cpp)
target_link_libraries(thread_safe -pthread)

add_executable(packaged_task src/multithreading/packaged_task.cpp)
target_link_libraries(packaged_task -pthread)

add_executable(race_condition src/multithreading/race_condition.cpp)
target_link_libraries(race_condition -pthread)

add_executable(unique_lock src/multithreading/unique_lock.cpp)
target_link_libraries(unique_lock -pthread)

add_executable(lock_guard src/multithreading/lock_guard.cpp)
target_link_libraries(lock_guard -pthread)

add_executable(thread_pool src/multithreading/thread_pool.cpp)
#target_link_libraries(thread_pool -pthread)

# add_executable(inter_process_communicationshared_memory src/multithreading/inter_process_communicationshared_memory.cpp)

add_executable(function_pointer src/function_pointer.cpp)

add_executable(bind src/bind.cpp)

add_executable(function src/function.cpp)

add_executable(lambda src/lambda.cpp)

add_executable(unions src/unions.cpp)

add_executable(basic_IO_operation_streams src/basic_IO_operation_filesystem_streams_reading_writing_files_formating_output_cin_cout_scanf_printf_gets_puts_getline.cpp)

add_executable(explicit_constructor src/class/explicit_constructor.cpp)

add_executable(const_constexpr_mutable src/const_constexpr_mutable.cpp)

add_executable(literals src/literals.cpp)

add_executable(ternary src/ternary.cpp)

add_executable(lists src/lists.cpp)

add_executable(type_traits src/type_traits.cpp)

add_executable(abstract_class_vs_interface src/class/abstract_class_vs_interface.cpp)

add_executable(typedef_type_alias_using_keyword src/typedef_type_alias_using_keyword.cpp)

add_executable(most_vexing_parse src/most_vexing_parse.cpp)

add_executable(VTABLE_and_VPTR src/VTABLE_and_VPTR.cpp)

# add_executable(allocator src/allocator.cpp)

add_executable(noexcept_operator_specifier src/noexcept_operator_specifier.cpp)

add_executable(callbacks src/callbacks.cpp)

add_executable(template_specialization_tag_dispatch src/template_specialization_tag_dispatch.cpp)

#add_executable(copy_move src/class/copy_move.cpp)

add_executable(forward src/forward.cpp)

add_executable(virtual_destructor_virtual_constructor src/class/virtual_destructor_virtual_constructor.cpp)

#add_executable(csv_reading_example src/third_party_tools/csv/fast-cpp-csv-parser/csv_reading_example.cpp)

add_executable(json_example src/third_party_tools/jason/nlohmann/json_example.cpp)

add_executable(header_guard src/class/header_guard/main.cpp)

add_executable(optimizing_cpp src/optimizing_cpp/index.cpp)

add_executable(track_memory_allocations src/track_memory_allocations.cpp)

add_executable(nested_namespaces src/nested_namespaces.cpp)

add_executable(type_dispatch_integral_constant_true_false_type src/type_dispatch_integral_constant_true_false_type.cpp)

add_executable(set_argv_argc src/set_argv_argc.cpp)

add_executable(arguments_parser_example src/arguments_parser_example.cpp)

add_executable(aggregate-copy-default-direct-value-zero src/{}-operator-aggregate-copy-default-direct-value-zero.cpp)

add_executable(aggregate_initialization src/aggregate_initialization.cpp)

add_executable(default-initialized src/default-initialized.cpp)

add_executable(scope_resolution_operator src/scope_resolution_operator.cpp)

add_executable(random_number_generation src/random_number_generation.cpp)

# set(CMAKE_CXX_FLAGS "-Wall -Wextra")
# set(CMAKE_CXX_FLAGS_DEBUG "-g")
# set(CMAKE_CXX_FLAGS_RELEASE "-O3")
add_executable(heap_and_stack_memory_layout_of_C_programs src/heap_and_stack_memory_layout_of_C_programs.cpp)

# target_compile_options(heap_and_stack_memory_layout_of_C_programs PRIVATE -Wall -Wextra -g -O0)

# set(CMAKE_CXX_FLAGS "-fsanitize=address ${CMAKE_CXX_FLAGS}")
# set(CMAKE_CXX_FLAGS "-fno-omit-frame-pointer ${CMAKE_CXX_FLAGS}")
add_executable(memory_checking src/memory_checking.cpp)

add_executable(tricky_questions src/tricky_questions.cpp)

add_executable(metaprogramming src/metaprogramming.cpp)

add_executable(regex_mathch_search src/regex_mathch_search.cpp)

# add_executable(filesystem src/filesystem.cpp)
# target_link_libraries(filesystem)
add_executable(optional src/optional.cpp)

add_executable(conditional_compilation src/conditional_compilation.cpp)

add_executable(std_invoke src/std_invoke.cpp)

add_executable(structured_binding_declaration src/structured_binding_declaration.cpp)

add_executable(asynchronous_programming src/asynchronous_programming.cpp)
target_link_libraries(asynchronous_programming -pthread)

add_executable(circular_dependency src/class/circular_dependency/circular_dependency.cpp src/class/circular_dependency/classA.cpp src/class/circular_dependency/classB.cpp)

add_executable(core_dump src/core_dump.cpp)


if(${CMAKE_GNU_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} GREATER_EQUAL 13)
    add_executable(printing_with_format src/printing_with_format.cpp)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND ${CMAKE_CXX_COMPILER_VERSION} GREATER_EQUAL 14)
    add_executable(printing_with_format src/printing_with_format.cpp)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND ${CMAKE_CXX_COMPILER_VERSION} GREATER_EQUAL 13)
    add_executable(printing_with_format src/printing_with_format.cpp)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND ${CMAKE_CXX_COMPILER_VERSION} GREATER_EQUAL 1900)
    add_executable(printing_with_format src/printing_with_format.cpp)
endif()

find_package(PkgConfig)

if(EXISTS "$ENV{HOME}/usr/lib/pkgconfig")
    set(ENV{PKG_CONFIG_PATH} "$ENV{HOME}/usr/lib/pkgconfig:" $ENV{PKG_CONFIG_PATH})
endif()

message("PKG_CONFIG_PATH:" $ENV{PKG_CONFIG_PATH})

message("\n########################################## tinyxml2 ##########################################\n")
pkg_check_modules(TINYXML2 tinyxml2)

if(${TINYXML2_FOUND})
    message("TINYXML2_FOUND:" ${TINYXML2_FOUND})
    message("TINYXML2_VERSION:" ${TINYXML2_VERSION})
    message("TINYXML2_LIBRARIES:" ${TINYXML2_LIBRARIES})
    message("TINYXML2_INCLUDE_DIRS:" ${TINYXML2_INCLUDE_DIRS})
    message("TINYXML2_LIBRARY_DIRS:" ${TINYXML2_LIBRARY_DIRS})
    include_directories(${TINYXML2_INCLUDE_DIRS})
    link_directories(${TINYXML2_LIBRARY_DIRS})
    add_executable(tinyxml2_demo src/third_party_tools/xml/tinyxml2/tinyxml2_demo.cpp)
    target_link_libraries(tinyxml2_demo ${TINYXML2_LIBRARIES})
else()
    message("tinyxml2 not found")
endif()

message("\n########################################## yaml-cpp ##########################################\n")

set(yaml-cpp_DIR "$ENV{HOME}/usr/share/cmake/yaml-cpp")
find_package(yaml-cpp)

if(${yaml-cpp_FOUND})
    message("yaml-cpp_FOUND:" ${yaml-cpp_FOUND})
    message("yaml-cpp_VERSION:" ${yaml-cpp_VERSION})
    add_executable(yaml-cpp_example src/third_party_tools/yaml/yaml-cpp/yaml-cpp_example.cpp)
    target_link_libraries(yaml-cpp_example yaml-cpp)
endif()

pkg_check_modules(BENCHMARK benchmark)

if(${BENCHMARK_FOUND})
    message("BENCHMARK_FOUND:" ${BENCHMARK_FOUND})
    message("BENCHMARK_VERSION:" ${BENCHMARK_VERSION})
    message("BENCHMARK_LIBRARIES:" ${BENCHMARK_LIBRARIES})
    message("BENCHMARK_INCLUDE_DIRS:" ${BENCHMARK_INCLUDE_DIRS})
    message("BENCHMARK_LIBRARY_DIRS:" ${BENCHMARK_LIBRARY_DIRS})
    include_directories(${TINYXML2_INCLUDE_DIRS})
    link_directories(${TINYXML2_LIBRARY_DIRS})
    add_executable(benchmark_demo src/third_party_tools/benchmark/benchmark_demo.cpp)
    target_link_libraries(benchmark_demo ${BENCHMARK_LIBRARIES} pthread)
endif()

option(TESTING "Enable testing" OFF)

if(testing_enabled)
    add_subdirectory(tests)
endif()


if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/sandbox.cpp")
    add_executable(sandbox src/sandbox.cpp)
endif()

