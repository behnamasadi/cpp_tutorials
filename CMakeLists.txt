cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
set(CMAKE_BUILD_TYPE DEBUG)
#set(CMAKE_BUILD_TYPE RELEASE)

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
 
#SET(CPACK_GENERATOR "DEB")
#SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "David Doria") #required
 
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

ADD_EXECUTABLE(pointers  src/pointers.cpp )
TARGET_LINK_LIBRARIES( pointers)

ADD_EXECUTABLE(string  src/string.cpp )
TARGET_LINK_LIBRARIES( string)

ADD_EXECUTABLE(exception_handling  src/exception_handling.cpp)
TARGET_LINK_LIBRARIES( exception_handling)

ADD_EXECUTABLE(heap_vs_stack  src/heap_vs_stack.cpp)
TARGET_LINK_LIBRARIES(heap_vs_stack)

ADD_EXECUTABLE(assert  src/assert.cpp)
TARGET_LINK_LIBRARIES(assert)

ADD_EXECUTABLE(return_abort_exit  src/return_abort_exit.cpp)
TARGET_LINK_LIBRARIES(return_abort_exit)

ADD_EXECUTABLE(rvalue_lvalue  src/rvalue_lvalue.cpp)
TARGET_LINK_LIBRARIES(rvalue_lvalue)

ADD_EXECUTABLE(primitive_data_types_numerical_limits_accuracy  src/primitive_data_types_numerical_limits_accuracy.cpp)
TARGET_LINK_LIBRARIES(primitive_data_types_numerical_limits_accuracy)

ADD_EXECUTABLE(arrays  src/arrays.cpp)
TARGET_LINK_LIBRARIES(arrays)


ADD_EXECUTABLE(data_type_info  src/data_type_info.cpp)
TARGET_LINK_LIBRARIES(data_type_info)

ADD_EXECUTABLE(dynamic_memory_allocation src/dynamic_memory_allocation.cpp)
TARGET_LINK_LIBRARIES(dynamic_memory_allocation)

#ADD_LIBRARY(add MODULE src/add.cpp)
ADD_EXECUTABLE(loadeding_libraries src/loadeding_libraries.cpp )
TARGET_LINK_LIBRARIES(loadeding_libraries dl)

ADD_EXECUTABLE(template src/template.cpp )
TARGET_LINK_LIBRARIES(template)

ADD_DEFINITIONS(-DLOGING=0)
ADD_EXECUTABLE(macro src/macro.cpp)
TARGET_LINK_LIBRARIES(macro)

ADD_EXECUTABLE(inline_functions src/inline_functions.cpp)
TARGET_LINK_LIBRARIES(inline_functions)

ADD_EXECUTABLE(align src/align.cpp)
TARGET_LINK_LIBRARIES(align)

ADD_EXECUTABLE(non_modifing_algorithm src/non_modifing_algorithm.cpp)
TARGET_LINK_LIBRARIES(non_modifing_algorithm)

ADD_EXECUTABLE(vector src/vector.cpp)
TARGET_LINK_LIBRARIES(vector)

ADD_EXECUTABLE(variadic_templates src/variadic_templates.cpp)
TARGET_LINK_LIBRARIES(variadic_templates)

ADD_EXECUTABLE(enum src/enum.cpp)
TARGET_LINK_LIBRARIES(enum)

ADD_EXECUTABLE(diamond_problem_virtual_inheritance src/class/diamond_problem_virtual_inheritance.cpp)
TARGET_LINK_LIBRARIES(diamond_problem_virtual_inheritance)

ADD_EXECUTABLE(shadowing src/class/shadowing.cpp)
TARGET_LINK_LIBRARIES(shadowing)

ADD_EXECUTABLE(protected_friend_class_function src/class/protected_friend_class_function.cpp)
TARGET_LINK_LIBRARIES(protected_friend_class_function)

ADD_EXECUTABLE(virtual_function_abstract_class src/class/virtual_function_abstract_class.cpp)
TARGET_LINK_LIBRARIES(virtual_function_abstract_class)

ADD_EXECUTABLE(multiple_inheritance_polymorphism src/class/multiple_inheritance_polymorphism.cpp)
TARGET_LINK_LIBRARIES(multiple_inheritance_polymorphism)

ADD_EXECUTABLE(private_public_protected_inheritance src/class/private_public_protected_inheritance.cpp)
TARGET_LINK_LIBRARIES(private_public_protected_inheritance)

ADD_EXECUTABLE(cast_Base_to_Derived_to_Base src/class/cast_Base_to_Derived_to_Base.cpp)
TARGET_LINK_LIBRARIES(cast_Base_to_Derived_to_Base)

ADD_EXECUTABLE(operator_overloading  src/class/operator_overloading.cpp )
TARGET_LINK_LIBRARIES( operator_overloading)

ADD_EXECUTABLE(default_0_delete_meaning  src/class/default_0_delete_meaning.cpp)
TARGET_LINK_LIBRARIES(default_0_delete_meaning)

ADD_EXECUTABLE(circular_dependency
    src/class/Circular_Dependency/circular_dependency.cpp
    src/class/Circular_Dependency/a.cpp
    src/class/Circular_Dependency/b.cpp
    src/class/Circular_Dependency/c.cpp)
TARGET_LINK_LIBRARIES(circular_dependency)

ADD_EXECUTABLE(switch_case src/switch_case.cpp)
TARGET_LINK_LIBRARIES(switch_case)

ADD_EXECUTABLE(set_map_pair_tuple src/set_map_pair_tuple.cpp)
TARGET_LINK_LIBRARIES(set_map_pair_tuple)

ADD_EXECUTABLE(cast_RTTI src/cast_RTTI.cpp)
TARGET_LINK_LIBRARIES(cast_RTTI)

ADD_EXECUTABLE(signals src/signals.cpp)
TARGET_LINK_LIBRARIES(signals)

ADD_EXECUTABLE(error_handling src/error_handling.cpp)
TARGET_LINK_LIBRARIES(error_handling)

ADD_EXECUTABLE(bitwise_operations src/bitwise_operations.cpp)
TARGET_LINK_LIBRARIES(bitwise_operations)

ADD_EXECUTABLE(bitset src/bitset.cpp)
TARGET_LINK_LIBRARIES(bitset)


############################# Multithreading #############################

ADD_EXECUTABLE(creating_and_terminating_threads src/multithreading/creating_and_terminating_threads.cpp)
TARGET_LINK_LIBRARIES(creating_and_terminating_threads -pthread)

ADD_EXECUTABLE(differentiating_between_threads src/multithreading/differentiating_between_threads.cpp)
TARGET_LINK_LIBRARIES(differentiating_between_threads -pthread)

ADD_EXECUTABLE(sleeping_threads src/multithreading/sleeping_threads.cpp)
TARGET_LINK_LIBRARIES(sleeping_threads -pthread)

ADD_EXECUTABLE(join_detach_threads src/multithreading/join_detach_threads.cpp)
TARGET_LINK_LIBRARIES(join_detach_threads -pthread)

ADD_EXECUTABLE(mutex src/multithreading/mutex.cpp)
TARGET_LINK_LIBRARIES(mutex -pthread)


ADD_EXECUTABLE(condition_variable src/multithreading/condition_variable.cpp)
TARGET_LINK_LIBRARIES(condition_variable -pthread)


ADD_EXECUTABLE(async_future_promise src/multithreading/async_future_promise.cpp)
TARGET_LINK_LIBRARIES(async_future_promise -pthread)

ADD_EXECUTABLE(dead_lock src/multithreading/dead_lock.cpp)
TARGET_LINK_LIBRARIES(dead_lock -pthread)

ADD_EXECUTABLE(thread_safe src/multithreading/thread_safe.cpp)
TARGET_LINK_LIBRARIES(thread_safe -pthread)

ADD_EXECUTABLE(packaged_task src/multithreading/packaged_task.cpp)
TARGET_LINK_LIBRARIES(packaged_task -pthread)

ADD_EXECUTABLE(race_condition src/multithreading/race_condition.cpp)
TARGET_LINK_LIBRARIES(race_condition -pthread)


ADD_EXECUTABLE(thread_callable_objects src/multithreading/thread_callable_objects.cpp)
TARGET_LINK_LIBRARIES(thread_callable_objects -pthread)


ADD_EXECUTABLE(unique_lock src/multithreading/unique_lock.cpp)
TARGET_LINK_LIBRARIES(unique_lock -pthread)




ADD_EXECUTABLE(function_pointer src/function_pointer.cpp)
TARGET_LINK_LIBRARIES(function_pointer)

ADD_EXECUTABLE(bind src/bind.cpp)
TARGET_LINK_LIBRARIES(bind)

ADD_EXECUTABLE(function src/function.cpp)
TARGET_LINK_LIBRARIES(function)

ADD_EXECUTABLE(lambda src/lambda.cpp)
TARGET_LINK_LIBRARIES(lambda)

ADD_EXECUTABLE(unions src/unions.cpp)
TARGET_LINK_LIBRARIES(unions)

ADD_EXECUTABLE(date_time src/date_time.cpp)
TARGET_LINK_LIBRARIES(date_time)

ADD_EXECUTABLE(fork src/fork.cpp)
TARGET_LINK_LIBRARIES(fork)

ADD_EXECUTABLE(basic_IO_operation_streams src/basic_IO_operation_streams_reading_writing_files_formating_output_cin_cout_scanf_printf_gets_puts_getline.cpp)
TARGET_LINK_LIBRARIES(basic_IO_operation_streams)

ADD_EXECUTABLE(explicit  src/explicit.cpp)
TARGET_LINK_LIBRARIES(explicit)

ADD_EXECUTABLE(volatile  src/volatile.cpp)
TARGET_LINK_LIBRARIES(volatile)

ADD_EXECUTABLE(register  src/register.cpp)
TARGET_LINK_LIBRARIES(register)

ADD_EXECUTABLE(const_constexpr_mutable  src/const_constexpr_mutable.cpp)
TARGET_LINK_LIBRARIES(const_constexpr_mutable)

ADD_EXECUTABLE(literals  src/literals.cpp)
TARGET_LINK_LIBRARIES(literals)

ADD_EXECUTABLE(ternary src/ternary.cpp)
TARGET_LINK_LIBRARIES(ternary)

ADD_EXECUTABLE(lists src/lists.cpp)
TARGET_LINK_LIBRARIES(lists)

ADD_EXECUTABLE(type_traits src/type_traits.cpp)
TARGET_LINK_LIBRARIES(type_traits)

ADD_EXECUTABLE(pimpl src/pimpl/main.cpp src/pimpl/studentpimpl.cpp)
TARGET_LINK_LIBRARIES(pimpl)

ADD_EXECUTABLE(abstract_class_vs_interface src/class/abstract_class_vs_interface.cpp)
TARGET_LINK_LIBRARIES(abstract_class_vs_interface)

ADD_EXECUTABLE(typedef_type_alias_using_keyword src/typedef_type_alias_using_keyword.cpp)
TARGET_LINK_LIBRARIES(typedef_type_alias_using_keyword)

ADD_EXECUTABLE(most_vexing_parse src/most_vexing_parse.cpp)
TARGET_LINK_LIBRARIES(most_vexing_parse)

ADD_EXECUTABLE(VTABLE_and_VPTR src/VTABLE_and_VPTR.cpp)
TARGET_LINK_LIBRARIES(VTABLE_and_VPTR)


ADD_EXECUTABLE(aggregate_Initialization  src/{}_aggregate_Initialization.cpp )
TARGET_LINK_LIBRARIES( aggregate_Initialization)

ADD_EXECUTABLE(allocator src/allocator.cpp)
TARGET_LINK_LIBRARIES(allocator)

ADD_EXECUTABLE(noexcept_operator_specifier src/noexcept_operator_specifier.cpp)
TARGET_LINK_LIBRARIES(noexcept_operator_specifier)

ADD_EXECUTABLE(callbacks src/callbacks.cpp)
TARGET_LINK_LIBRARIES(callbacks)

ADD_EXECUTABLE(template_specialization_tag_dispatch_SFINAE src/template_specialization_tag_dispatch_SFINAE.cpp)
TARGET_LINK_LIBRARIES(template_specialization_tag_dispatch_SFINAE)

ADD_EXECUTABLE(copy_move src/class/copy_move.cpp)
TARGET_LINK_LIBRARIES(copy_move)

ADD_EXECUTABLE(forward src/forward.cpp)
TARGET_LINK_LIBRARIES(forward)

ADD_EXECUTABLE(RVO_NRVO_copy_elision src/RVO_NRVO_copy_elision.cpp)
TARGET_LINK_LIBRARIES(RVO_NRVO_copy_elision)

ADD_EXECUTABLE(virtual_copy_constructor_destructor src/class/virtual_copy_constructor_destructor.cpp)
TARGET_LINK_LIBRARIES(virtual_copy_constructor_destructor)


ADD_EXECUTABLE(range_for_loop src/range_for_loop.cpp)
TARGET_LINK_LIBRARIES(range_for_loop)

ADD_EXECUTABLE(cpp_17 src/cpp_17.cpp)
TARGET_LINK_LIBRARIES(cpp_17)

ADD_EXECUTABLE(csv_reading_example src/third_party_tools/csv/fast-cpp-csv-parser/csv_reading_example.cpp)
TARGET_LINK_LIBRARIES(csv_reading_example)

ADD_EXECUTABLE(json_example src/third_party_tools/jason/nlohmann/json_example.cpp)
TARGET_LINK_LIBRARIES(json_example)

ADD_EXECUTABLE(header_guard src/class/header_guard/main.cpp)
TARGET_LINK_LIBRARIES(header_guard)

ADD_EXECUTABLE(optimizing_cpp src/optimizing_cpp/index.cpp)
TARGET_LINK_LIBRARIES(optimizing_cpp)

ADD_EXECUTABLE(track_memoy_allocations src/track_memoy_allocations.cpp)
TARGET_LINK_LIBRARIES(track_memoy_allocations)

SET(ENV{PKG_CONFIG_PATH} "$ENV{HOME}/usr/lib/pkgconfig:" $ENV{PKG_CONFIG_PATH})
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
    ADD_EXECUTABLE(tinyxml2_demo src/third_party_tools/xml/tinyxml2/tinyxml2_demo.cpp)
    TARGET_LINK_LIBRARIES(tinyxml2_demo ${TINYXML2_LIBRARIES})
endif()

SET(yaml-cpp_DIR "$ENV{HOME}/usr/share/cmake/yaml-cpp")
FIND_PACKAGE(yaml-cpp)
IF(${yaml-cpp_FOUND})
    MESSAGE("yaml-cpp_FOUND:" ${yaml-cpp_FOUND})
    MESSAGE("yaml-cpp_VERSION:" ${yaml-cpp_VERSION})
    ADD_EXECUTABLE(yaml-cpp_example src/third_party_tools/yaml/yaml-cpp/yaml-cpp_example.cpp )
    TARGET_LINK_LIBRARIES(yaml-cpp_example yaml-cpp)
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
    ADD_EXECUTABLE(benchmark_demo src/third_party_tools/benchmark/benchmark_demo.cpp)
    TARGET_LINK_LIBRARIES(benchmark_demo ${BENCHMARK_LIBRARIES} pthread)
endif()




OPTION(TESTING "Enable testing" OFF)
IF(testing_enabled)
	ADD_SUBDIRECTORY(tests)
ENDIF()

IF(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/sandbox")
	ADD_SUBDIRECTORY(src/sandbox)
ENDIF()



