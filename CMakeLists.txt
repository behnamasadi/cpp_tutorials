cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(cpp_tutorials)

if(NOT "${CMAKE_GENERATOR}" STREQUAL "Ninja Multi-Config")
    # message(WARNING "It's recommended to use -G \"Ninja Multi-Config\" for this project.")
    # OR, if you want to force-stop the configuration:
    message(FATAL_ERROR "Please use -G \"Ninja Multi-Config\" for this project.")
endif()

set(CMAKE_CXX_STANDARD 20) # Replace 20 with the version number of the latest C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message("using Visual Studio C++")

    # set warning level into errors
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    endif()
endif()

message("CMAKE_CXX_COMPILER_VERSION: " ${CMAKE_CXX_COMPILER_VERSION})

option(ENABLE_JSON "enable the json example with nlohmann library" ON)
option(ENABLE_CSV "enable the csv example with fast-cpp-csv-parser library" ON)
option(ENABLE_YAML "enable the yaml example with yaml-cpp" ON)
option(ENABLE_BENCHMARKING "enable the code benchmarking with Goolge benchmarking" OFF)
option(ENABLE_TRACY "enable the code profiling with tracy" OFF)
option(ENABLE_XML "enable the xml example with tinyxml2" ON)
option(ENABLE_TESTING "Enable testing" OFF)
option(ENABLE_SANITIZE "Add sanitizer flags" OFF)
option(ENABLE_SPDLOG "Add spdlog for logging" ON)
option(ENABLE_CPP_REST_SDK "Add CppRestSDK for microservices" OFF)


add_executable(pointers src/pointers/pointers.cpp)

add_executable(string  src/string.cpp )

add_executable(exception_handling src/exception_handling.cpp)

#add_executable(heap_vs_stack  src/heap_vs_stack.cpp)

add_executable(assert src/assert.cpp)

add_executable(return_abort_exit src/return_abort_exit.cpp)

add_executable(rvalue_lvalue src/rvalue_lvalue.cpp)

add_executable(primitive_data_types_numerical_limits_accuracy src/primitive_data_types_numerical_limits_accuracy.cpp)

add_executable(arrays src/arrays.cpp)

add_executable(dynamic_memory_allocation src/dynamic_memory_allocation.cpp)

# ADD_LIBRARY(add MODULE src/add.cpp)


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


if(ENABLED_SANITIZE)
    set(CMAKE_CXX_FLAGS "-fsanitize=address ${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS "-fno-omit-frame-pointer ${CMAKE_CXX_FLAGS}")
endif()
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

#add_executable(packaged_task src/packaged_task.cpp)

add_executable(std_visit src/std_visit.cpp)

add_executable(error_code src/error_code.cpp)


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





add_executable(any src/any.cpp)


option(PRINT_INCLUDES "Printing List of All Included Headers" OFF)
add_executable(copy_and_swap src/copy_and_swap.cpp)
if(PRINT_INCLUDES)
	if(MSVC)
	    target_compile_options(copy_and_swap PRIVATE "/showIncludes")
	elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	    target_compile_options(copy_and_swap PRIVATE "-H")
	endif()
endif()




if(ENABLED_TESTING)
    add_subdirectory(tests)
else()
    message("testing is not enabled")
endif()

if(ENABLE_XML)
    message("\n########################################## tinyxml2 ##########################################\n")
    
    include(FetchContent)
    
    FetchContent_Declare(
        tinyxml2
        GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
        GIT_TAG master
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    
    FetchContent_MakeAvailable(tinyxml2)

    message("TINYXML2_FOUND: ${TINYXML2_FOUND}")
    message("TINYXML2_VERSION: ${TINYXML2_VERSION}")
    message("TINYXML2_LIBRARIES: ${TINYXML2_LIBRARIES}")
    message("TINYXML2_INCLUDE_DIRS: ${TINYXML2_INCLUDE_DIRS}")
    message("TINYXML2_LIBRARY_DIRS: ${TINYXML2_LIBRARY_DIRS}")

    include_directories(${TINYXML2_INCLUDE_DIRS})
    link_directories(${TINYXML2_LIBRARY_DIRS})

    add_executable(tinyxml2_demo src/tinyxml2_demo.cpp)
    target_link_libraries(tinyxml2_demo tinyxml2)

else()
    message("XML is not enabled")
endif()


if(ENABLE_YAML)
    message("\n########################################## yaml-cpp ##########################################\n")

    FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG master # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
    )
    FetchContent_GetProperties(yaml-cpp)

    if(NOT yaml-cpp_POPULATED)
    message(STATUS "Fetching yaml-cpp...")
    FetchContent_Populate(yaml-cpp)
    add_subdirectory(${yaml-cpp_SOURCE_DIR} ${yaml-cpp_BINARY_DIR})
    endif()

    FetchContent_MakeAvailable(yaml-cpp)
    FetchContent_GetProperties(yaml-cpp)


    message("yaml-cpp_FOUND:" ${yaml-cpp_FOUND})
    message("yaml-cpp_VERSION:" ${yaml-cpp_VERSION})
    add_executable(yaml-cpp_example src/yaml-cpp_example.cpp)
    target_link_libraries(yaml-cpp_example yaml-cpp)
else()
    message("yaml is not enabled")
endif()



if(ENABLE_BENCHMARKING)
    message("\n########################################## Google benchmark ##########################################\n")

    FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark
        GIT_TAG main # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
    )
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
    FetchContent_GetProperties(benchmark)

    if(NOT benchmark_POPULATED)
        message(STATUS "Fetching benchmark...")
        FetchContent_Populate(benchmark)
        add_subdirectory(${benchmark_SOURCE_DIR} ${benchmark_BINARY_DIR})
    endif()

    message("BENCHMARK_FOUND:" ${BENCHMARK_FOUND})
    message("BENCHMARK_VERSION:" ${BENCHMARK_VERSION})
    message("BENCHMARK_LIBRARIES:" ${BENCHMARK_LIBRARIES})
    message("BENCHMARK_INCLUDE_DIRS:" ${BENCHMARK_INCLUDE_DIRS})
    message("BENCHMARK_LIBRARY_DIRS:" ${BENCHMARK_LIBRARY_DIRS})

    include_directories(${BENCHMARK_INCLUDE_DIRS}) # Corrected to use benchmark include directories
    link_directories(${BENCHMARK_LIBRARY_DIRS})    # Corrected to use benchmark library directories
    add_executable(benchmark_demo src/benchmark_demo.cpp)
    target_link_libraries(benchmark_demo ${BENCHMARK_LIBRARIES} pthread)
else()
    message("Benchmarking is not enabled")
endif()



if(ENABLE_JSON)
    message("\n########################################## json ##########################################\n")

    FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG v3.11.3
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )

    FetchContent_MakeAvailable(json)

    add_executable(json_example src/json_example.cpp)

    # Define a macro CMAKE_CURRENT_SOURCE_DIR with the value of CMAKE_CURRENT_SOURCE_DIR
    target_compile_definitions(json_example PRIVATE CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

    # Include the nlohmann/json directory
    target_include_directories(json_example PRIVATE ${json_SOURCE_DIR})
    target_link_libraries(json_example PRIVATE nlohmann_json::nlohmann_json)


else()
    message("json is not enabled")
endif()






if(ENABLE_TRACY)
    message("\n########################################## tracy ##########################################\n")
    include(FetchContent)
    FetchContent_Declare (tracy
    GIT_REPOSITORY https://github.com/wolfpld/tracy.git
    GIT_TAG v0.11.0
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE)

    FetchContent_MakeAvailable(tracy)



    FetchContent_GetProperties(tracy)
    # if(NOT tracy_POPULATED)
    # #   FetchContent_Populate(tracy)
    # #   add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
    # message("-------------------")
    # else()
    # message("++++++++++++++++")
    # endif()


    add_executable(tracy_profile src/tracy_profile.cpp)
    target_link_libraries ( tracy_profile PUBLIC TracyClient )

else()
    message("tracy is not enabled")
endif()


if(ENABLE_CSV)
message("\n########################################## fast-cpp-csv-parser ##########################################\n")
include(FetchContent)
FetchContent_Declare (fast-cpp-csv-parser
GIT_REPOSITORY https://github.com/ben-strasser/fast-cpp-csv-parser
GIT_TAG master
GIT_SHALLOW TRUE
GIT_PROGRESS TRUE)

FetchContent_MakeAvailable(fast-cpp-csv-parser)
FetchContent_GetProperties(fast-cpp-csv-parser)

add_executable(csv_reading_example src/csv_reading_example.cpp)
target_compile_definitions(csv_reading_example PRIVATE CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(csv_reading_example PRIVATE ${fast-cpp-csv-parser_SOURCE_DIR})

else()
message("fast-cpp-csv-parser is not enabled")

endif()


if(ENABLE_SPDLOG)
    message("\n########################################## spdlog ##########################################\n")
# Fetch spdlog
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.14.1  # Use a specific tag, branch, or commit
  )
  
  FetchContent_MakeAvailable(spdlog)
  
  # Create an executable or library
  add_executable(spdlog_example src/spdlog_example.cpp)
  
  # Link spdlog to your target
  target_link_libraries(spdlog_example PRIVATE spdlog::spdlog)
  
  # Optionally, if you want to disable spdlog's automatic formatting, add the following:
  # target_compile_definitions(MyExecutable PRIVATE SPDLOG_FMT_EXTERNAL)
    
else()
    message("spdlog is not enabled")
endif()


if(ENABLE_CPP_REST_SDK)
    message("\n########################################## CppRestSDK ##########################################\n")
    # Download and add CppRestSDK as a dependency
# Fetch CppRestSDK

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error")

FetchContent_Declare(
  CppRestSDK
  GIT_REPOSITORY https://github.com/microsoft/cpprestsdk.git
  GIT_TAG        v2.10.18 # Replace with the desired version
)

FetchContent_MakeAvailable(CppRestSDK)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error")


# # Print some CMake variables related to CppRestSDK
# get_target_property(cpprestsdk_INCLUDE_DIR cpprestsdk::cpprest INTERFACE_INCLUDE_DIRECTORIES)
# get_target_property(cpprestsdk_LIBRARIES cpprestsdk::cpprest INTERFACE_LINK_LIBRARIES)

# message(STATUS "CppRestSDK include directories: ${cpprestsdk_INCLUDE_DIR}")
# message(STATUS "CppRestSDK libraries: ${cpprestsdk_LIBRARIES}")


    # # Add your executable
    # add_executable(user_service src/user_service.cpp)

    # # Link CppRestSDK with your application
    # target_link_libraries(user_service PRIVATE cpprest)
    # target_compile_options(user_service PRIVATE -Wno-error)



    #payment_service.cpp
    #product_service.cpp
    #order_service.cpp


else()
    message("CppRestSDK is not enabled")
endif()





if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
    add_executable(main src/main.cpp)
endif()
