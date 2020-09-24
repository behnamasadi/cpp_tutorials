cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
set(CMAKE_BUILD_TYPE DEBUG)
#set(CMAKE_BUILD_TYPE RELEASE)


#option(USE_CLANG "build application with clang" ON) # OFF is the default

set (CMAKE_C_COMPILER             "/usr/bin/clang")
#set (CMAKE_C_FLAGS                "-Wall -std=c99")
set (CMAKE_C_FLAGS_DEBUG          "-g")
set (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
set (CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
set (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

set (CMAKE_CXX_COMPILER             "/usr/bin/clang++")
set (CMAKE_CXX_FLAGS                "-Wall")
set (CMAKE_CXX_FLAGS_DEBUG          "-g")
set (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
set (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

set (CMAKE_AR      "/usr/bin/llvm-ar")
set (CMAKE_LINKER  "/usr/bin/llvm-ld")
set (CMAKE_NM      "/usr/bin/llvm-nm")
set (CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
set (CMAKE_RANLIB  "/usr/bin/llvm-ranlib")


project(Tutorials)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#set warning level into errors 
if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()

#Finding Memory leaking, Stack and Heap overflow
#set(CMAKE_CXX_FLAGS "-fsanitize=address ${CMAKE_CXX_FLAGS}")
#set(CMAKE_CXX_FLAGS "-fno-omit-frame-pointer ${CMAKE_CXX_FLAGS}")


#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-private-field")




######################### How to cpack create deb file#########################
 
#set(CPACK_GENERATOR "DEB")
#set(CPACK_DEBIAN_PACKAGE_MAINTAINER "David Doria") #required
 
#INCLUDE(CPack)
 
# To use this:
# make package
# sudo dpkg -i DistanceBetweenPoints-0.1.1-Linux.deb
 
# This will result in the file:
#/usr/distance/DistanceBetweenPoints


# To use this:
# make package
# sudo dpkg -i DistanceBetweenPoints-0.1.1-Linux.deb
 
# This will result in the file:
#/usr/distance/DistanceBetweenPoints

#https://cmake.org/Wiki/CMake:Packaging_With_CPack

add_executable(pointers  src/pointers.cpp )
target_link_libraries( pointers)

add_executable(string  src/string.cpp )
target_link_libraries( string)

add_executable(exception_handling  src/exception_handling.cpp)
target_link_libraries( exception_handling)

add_executable(heap_vs_stack  src/heap_vs_stack.cpp)
target_link_libraries(heap_vs_stack)

add_executable(assert  src/assert.cpp)
target_link_libraries(assert)

add_executable(return_abort_exit  src/return_abort_exit.cpp)
target_link_libraries(return_abort_exit)

add_executable(rvalue_lvalue  src/rvalue_lvalue.cpp)
target_link_libraries(rvalue_lvalue)

add_executable(primitive_data_types_numerical_limits_accuracy  src/primitive_data_types_numerical_limits_accuracy.cpp)
target_link_libraries(primitive_data_types_numerical_limits_accuracy)

add_executable(arrays  src/arrays.cpp)
target_link_libraries(arrays)


add_executable(data_type_info  src/data_type_info.cpp)
target_link_libraries(data_type_info)

add_executable(dynamic_memory_allocation src/dynamic_memory_allocation.cpp)
target_link_libraries(dynamic_memory_allocation)

#ADD_LIBRARY(add MODULE src/add.cpp)
add_executable(loadeding_libraries src/loadeding_libraries.cpp )
target_link_libraries(loadeding_libraries dl)

add_executable(template src/template.cpp )
target_link_libraries(template)

ADD_DEFINITIONS(-DLOGING=0)
add_executable(macro src/macro.cpp)
target_link_libraries(macro)

add_executable(inline_functions src/inline_functions.cpp)
target_link_libraries(inline_functions)

add_executable(align src/align.cpp)
target_link_libraries(align)

add_executable(non_modifing_algorithm src/non_modifing_algorithm.cpp)
target_link_libraries(non_modifing_algorithm)

add_executable(vector src/vector.cpp)
target_link_libraries(vector)

add_executable(variadic_templates src/variadic_templates.cpp)
target_link_libraries(variadic_templates)

add_executable(enum src/enum.cpp)
target_link_libraries(enum)

add_executable(diamond_problem_virtual_inheritance src/class/diamond_problem_virtual_inheritance.cpp)
target_link_libraries(diamond_problem_virtual_inheritance)

add_executable(shadowing src/class/shadowing.cpp)
target_link_libraries(shadowing)

add_executable(protected_friend_class_function src/class/protected_friend_class_function.cpp)
target_link_libraries(protected_friend_class_function)

add_executable(virtual_function_abstract_class src/class/virtual_function_abstract_class.cpp)
target_link_libraries(virtual_function_abstract_class)

add_executable(multiple_inheritance_polymorphism src/class/multiple_inheritance_polymorphism.cpp)
target_link_libraries(multiple_inheritance_polymorphism)

add_executable(private_public_protected_inheritance src/class/private_public_protected_inheritance.cpp)
target_link_libraries(private_public_protected_inheritance)

add_executable(cast_Base_to_Derived_to_Base src/class/cast_Base_to_Derived_to_Base.cpp)
target_link_libraries(cast_Base_to_Derived_to_Base)

add_executable(operator_overloading  src/class/operator_overloading.cpp )
target_link_libraries( operator_overloading)

add_executable(default_0_delete_meaning  src/class/default_0_delete_meaning.cpp)
target_link_libraries(default_0_delete_meaning)

add_executable(circular_dependency
    src/class/Circular_Dependency/circular_dependency.cpp
    src/class/Circular_Dependency/a.cpp
    src/class/Circular_Dependency/b.cpp
    src/class/Circular_Dependency/c.cpp)
target_link_libraries(circular_dependency)

add_executable(switch_case src/switch_case.cpp)
target_link_libraries(switch_case)

add_executable(set_map_pair_tuple src/set_map_pair_tuple.cpp)
target_link_libraries(set_map_pair_tuple)

add_executable(signals src/signals.cpp)
target_link_libraries(signals)

add_executable(error_handling src/error_handling.cpp)
target_link_libraries(error_handling)

add_executable(bitwise_operations src/bitwise_operations.cpp)
target_link_libraries(bitwise_operations)

add_executable(bitset src/bitset.cpp)
target_link_libraries(bitset)

add_executable(inheritance src/class/inheritance.cpp)
target_link_libraries(inheritance)

############################# Advance C++ Concepts/ Idiom #############################
add_executable(RTTI src/RTTI.cpp)
target_link_libraries(RTTI)

add_executable(CRTP src/CRTP.cpp)
target_link_libraries(CRTP)

add_executable(double_dispatch src/double_dispatch.cpp)
target_link_libraries(double_dispatch)

add_executable(RVO_NRVO_copy_elision src/RVO_NRVO_copy_elision.cpp)
target_link_libraries(RVO_NRVO_copy_elision)

add_executable(pimpl src/pimpl/main.cpp src/pimpl/studentpimpl.cpp)
target_link_libraries(pimpl)

add_executable(return_type_resolver src/return_type_resolver.cpp)
target_link_libraries(return_type_resolver)

add_executable(RAII src/RAII.cpp)
target_link_libraries(RAII)

add_executable(SFINAE src/SFINAE.cpp)
target_link_libraries(SFINAE)

add_executable(type_erasure src/type_erasure.cpp)
target_link_libraries(type_erasure)


############################# Multithreading #############################

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


add_executable(condition_variable src/multithreading/condition_variable.cpp)
target_link_libraries(condition_variable -pthread)


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

#add_executable(inter_process_communicationshared_memory src/multithreading/inter_process_communicationshared_memory.cpp)
#target_link_libraries(inter_process_communicationshared_memory -pthread)

add_executable(function_pointer src/function_pointer.cpp)
target_link_libraries(function_pointer)

add_executable(bind src/bind.cpp)
target_link_libraries(bind)

add_executable(function src/function.cpp)
target_link_libraries(function)

add_executable(lambda src/lambda.cpp)
target_link_libraries(lambda)

add_executable(unions src/unions.cpp)
target_link_libraries(unions)

add_executable(date_time src/date_time.cpp)
target_link_libraries(date_time)

add_executable(fork src/fork.cpp)
target_link_libraries(fork)

add_executable(basic_IO_operation_streams src/basic_IO_operation_filesystem_streams_reading_writing_files_formating_output_cin_cout_scanf_printf_gets_puts_getline.cpp)
target_link_libraries(basic_IO_operation_streams stdc++fs)

add_executable(explicit  src/explicit.cpp)
target_link_libraries(explicit)

add_executable(volatile  src/volatile.cpp)
target_link_libraries(volatile)

add_executable(register  src/register.cpp)
target_link_libraries(register)

add_executable(const_constexpr_mutable  src/const_constexpr_mutable.cpp)
target_link_libraries(const_constexpr_mutable)

add_executable(literals  src/literals.cpp)
target_link_libraries(literals)

add_executable(ternary src/ternary.cpp)
target_link_libraries(ternary)

add_executable(lists src/lists.cpp)
target_link_libraries(lists)

add_executable(type_traits src/type_traits.cpp)
target_link_libraries(type_traits)

add_executable(abstract_class_vs_interface src/class/abstract_class_vs_interface.cpp)
target_link_libraries(abstract_class_vs_interface)

add_executable(typedef_type_alias_using_keyword src/typedef_type_alias_using_keyword.cpp)
target_link_libraries(typedef_type_alias_using_keyword)

add_executable(most_vexing_parse src/most_vexing_parse.cpp)
target_link_libraries(most_vexing_parse)

add_executable(VTABLE_and_VPTR src/VTABLE_and_VPTR.cpp)
target_link_libraries(VTABLE_and_VPTR)


add_executable(aggregate_Initialization  src/{}_aggregate_Initialization.cpp )
target_link_libraries( aggregate_Initialization)

add_executable(allocator src/allocator.cpp)
target_link_libraries(allocator)

add_executable(noexcept_operator_specifier src/noexcept_operator_specifier.cpp)
target_link_libraries(noexcept_operator_specifier)

add_executable(callbacks src/callbacks.cpp)
target_link_libraries(callbacks)

add_executable(template_specialization_tag_dispatch src/template_specialization_tag_dispatch.cpp)
target_link_libraries(template_specialization_tag_dispatch)

add_executable(copy_move src/class/copy_move.cpp)
target_link_libraries(copy_move)

add_executable(forward src/forward.cpp)
target_link_libraries(forward)

add_executable(virtual_copy_constructor_destructor src/class/virtual_copy_constructor_destructor.cpp)
target_link_libraries(virtual_copy_constructor_destructor)


add_executable(range_for_loop src/range_for_loop.cpp)
target_link_libraries(range_for_loop)

add_executable(cpp_17 src/cpp_17.cpp)
target_link_libraries(cpp_17 stdc++fs)

add_executable(csv_reading_example src/third_party_tools/csv/fast-cpp-csv-parser/csv_reading_example.cpp)
target_link_libraries(csv_reading_example)

add_executable(json_example src/third_party_tools/jason/nlohmann/json_example.cpp)
target_link_libraries(json_example)

add_executable(header_guard src/class/header_guard/main.cpp)
target_link_libraries(header_guard)

add_executable(optimizing_cpp src/optimizing_cpp/index.cpp)
target_link_libraries(optimizing_cpp)

add_executable(track_memoy_allocations src/track_memoy_allocations.cpp)
target_link_libraries(track_memoy_allocations)

add_executable(copy_initialization src/copy_initialization.cpp)
target_link_libraries(copy_initialization)

add_executable(buffer_overflow src/buffer_overflow.cpp)
target_link_libraries(buffer_overflow)

add_executable(nested_namespaces src/nested_namespaces.cpp)
target_link_libraries(nested_namespaces)

set(ENV{PKG_CONFIG_PATH} "$ENV{HOME}/usr/lib/pkgconfig:" $ENV{PKG_CONFIG_PATH})
MESSAGE("PKG_CONFIG_PATH:" $ENV{PKG_CONFIG_PATH})
find_package(PkgConfig)
pkg_check_modules(TINYXML2 tinyxml2)
if(${TINYXML2_FOUND})
    MESSAGE("TINYXML2_FOUND:" ${TINYXML2_FOUND})
    MESSAGE("TINYXML2_VERSION:" ${TINYXML2_VERSION})
    MESSAGE("TINYXML2_LIBRARIES:" ${TINYXML2_LIBRARIES})
    MESSAGE("TINYXML2_INCLUDE_DIRS:" ${TINYXML2_INCLUDE_DIRS})
    MESSAGE("TINYXML2_LIBRARY_DIRS:" ${TINYXML2_LIBRARY_DIRS})
    INCLUDE_DIRECTORIES(${TINYXML2_INCLUDE_DIRS})
    LINK_DIRECTORIES(${TINYXML2_LIBRARY_DIRS})
    add_executable(tinyxml2_demo src/third_party_tools/xml/tinyxml2/tinyxml2_demo.cpp)
    target_link_libraries(tinyxml2_demo ${TINYXML2_LIBRARIES})
endif()

set(yaml-cpp_DIR "$ENV{HOME}/usr/share/cmake/yaml-cpp")
FIND_PACKAGE(yaml-cpp)
IF(${yaml-cpp_FOUND})
    MESSAGE("yaml-cpp_FOUND:" ${yaml-cpp_FOUND})
    MESSAGE("yaml-cpp_VERSION:" ${yaml-cpp_VERSION})
    add_executable(yaml-cpp_example src/third_party_tools/yaml/yaml-cpp/yaml-cpp_example.cpp )
    target_link_libraries(yaml-cpp_example yaml-cpp)
ENDIF()


pkg_check_modules(BENCHMARK benchmark)
if(${BENCHMARK_FOUND})
    MESSAGE("BENCHMARK_FOUND:" ${BENCHMARK_FOUND})
    MESSAGE("BENCHMARK_VERSION:" ${BENCHMARK_VERSION})
    MESSAGE("BENCHMARK_LIBRARIES:" ${BENCHMARK_LIBRARIES})
    MESSAGE("BENCHMARK_INCLUDE_DIRS:" ${BENCHMARK_INCLUDE_DIRS})
    MESSAGE("BENCHMARK_LIBRARY_DIRS:" ${BENCHMARK_LIBRARY_DIRS})
    INCLUDE_DIRECTORIES(${TINYXML2_INCLUDE_DIRS})
    LINK_DIRECTORIES(${TINYXML2_LIBRARY_DIRS})
    add_executable(benchmark_demo src/third_party_tools/benchmark/benchmark_demo.cpp)
    target_link_libraries(benchmark_demo ${BENCHMARK_LIBRARIES} pthread)
endif()




OPTION(TESTING "Enable testing" OFF)
IF(testing_enabled)
	ADD_SUBDIRECTORY(tests)
ENDIF()

IF(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/sandbox")
	ADD_SUBDIRECTORY(src/sandbox)
ENDIF()



