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

# Disable C++20 module scanning (CMake 3.28+)
# Dependencies like yaml-cpp, spdlog don't support C++20 modules yet
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message("using Visual Studio C++")

    # set warning level into errors
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    endif()
endif()

# Handle threading library for different platforms
if(WIN32)
    set(THREADING_LIB "")
else()
    set(THREADING_LIB "-pthread")
endif()

message("CMAKE_CXX_COMPILER_VERSION: " ${CMAKE_CXX_COMPILER_VERSION})

option(ENABLE_JSON "enable the json example with nlohmann library" ON)
option(ENABLE_CSV "enable the csv example with fast-cpp-csv-parser library" ON)
option(ENABLE_YAML "enable the yaml example with yaml-cpp" ON)
option(ENABLE_BENCHMARKING "enable the code benchmarking with Google benchmark v1.9.5" OFF)
option(ENABLE_TRACY "enable the code profiling with tracy" OFF)
option(ENABLE_XML "enable the xml example with tinyxml2" ON)
option(ENABLE_TESTING "Enable testing" OFF)
option(ENABLE_SANITIZE "Add sanitizer flags" OFF)
option(ENABLE_SPDLOG "Add spdlog for logging" ON)
option(ENABLE_CURL "Enable cURL for REST APIs and Webhooks (HTTP callbacks)" OFF)
option(ENABLE_GRPC "Enable gRPC for RPC communication" OFF)
option(ENABLE_WEBSOCKET "Enable IXWebSocket for WebSocket communication" OFF)
option(ENABLE_WEBRTC "Enable libdatachannel for WebRTC (Data Channels, Media, WebSocket)" OFF)
option(ENABLE_GRAPHQL "Enable cppgraphqlgen for GraphQL" OFF)
option(ENABLE_SOAP "Enable XML SOAP example (requires ENABLE_XML and ENABLE_CURL)" OFF)

add_executable(pointers src/pointers/pointers.cpp)

add_executable(string  src/string.cpp)

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

add_executable(queue src/queue.cpp)

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
target_link_libraries(RAII ${THREADING_LIB})

add_executable(SFINAE src/SFINAE.cpp)

add_executable(type_erasure src/type_erasure.cpp)

add_executable(buffer_overflow src/buffer_overflow.cpp)

add_executable(stack_overflow src/stack_overflow.cpp)

add_executable(stack_unwinding src/stack_unwinding.cpp)

# ############################ Multithreading #############################
add_executable(creating_and_terminating_threads src/multithreading/creating_and_terminating_threads.cpp)
target_link_libraries(creating_and_terminating_threads ${THREADING_LIB})

add_executable(differentiating_between_threads src/multithreading/differentiating_between_threads.cpp)
target_link_libraries(differentiating_between_threads ${THREADING_LIB})

add_executable(sleeping_threads src/multithreading/sleeping_threads.cpp)
target_link_libraries(sleeping_threads ${THREADING_LIB})

add_executable(join_detach_threads src/multithreading/join_detach_threads.cpp)
target_link_libraries(join_detach_threads ${THREADING_LIB})

add_executable(mutex src/multithreading/mutex.cpp)
target_link_libraries(mutex ${THREADING_LIB})

# add_executable(condition_variable src/multithreading/condition_variable.cpp)
# target_link_libraries(condition_variable -pthread)
add_executable(async_future_promise src/multithreading/async_future_promise.cpp)
target_link_libraries(async_future_promise ${THREADING_LIB})

add_executable(dead_lock src/multithreading/dead_lock.cpp)
target_link_libraries(dead_lock ${THREADING_LIB})

add_executable(thread_safe src/multithreading/thread_safe.cpp)
target_link_libraries(thread_safe ${THREADING_LIB})

add_executable(packaged_task src/multithreading/packaged_task.cpp)
target_link_libraries(packaged_task ${THREADING_LIB})

add_executable(race_condition src/multithreading/race_condition.cpp)
target_link_libraries(race_condition ${THREADING_LIB})

add_executable(unique_lock src/multithreading/unique_lock.cpp)
target_link_libraries(unique_lock ${THREADING_LIB})

add_executable(lock_guard src/multithreading/lock_guard.cpp)
target_link_libraries(lock_guard ${THREADING_LIB})

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

add_executable(callbacks src/callbacks.cpp)

add_executable(template_specialization_tag_dispatch src/template_specialization_tag_dispatch.cpp)

add_executable(copy_constructor_copy_assignment src/copy_constructor_copy_assignment.cpp)

add_executable(shallow_copy_deep_copy src/shallow_copy_deep_copy.cpp)

add_executable(forward src/forward.cpp)

add_executable(virtual_destructor_virtual_constructor src/class/virtual_destructor_virtual_constructor.cpp)

add_executable(header_guard src/class/header_guard/main.cpp)

add_executable(optimizing_cpp src/optimizing_cpp/index.cpp)

add_executable(track_memory_allocations src/track_memory_allocations.cpp)

add_executable(nested_namespaces src/nested_namespaces.cpp)

add_executable(type_dispatch_integral_constant_true_false_type src/type_dispatch_integral_constant_true_false_type.cpp)

add_executable(set_argv_argc src/set_argv_argc.cpp)

add_executable(arguments_parser_example src/arguments_parser_example.cpp)

add_executable(initialization src/initialization.cpp)

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
target_link_libraries(asynchronous_programming ${THREADING_LIB})

add_executable(circular_dependency src/class/circular_dependency/circular_dependency.cpp src/class/circular_dependency/classA.cpp src/class/circular_dependency/classB.cpp)

# core_dump uses unistd.h which is only available on Unix-like systems
if(UNIX)
    add_executable(core_dump src/core_dump.cpp)
endif()

# Only build these on non-Windows platforms
if(NOT WIN32)
    add_executable(align src/align.cpp)
    add_executable(signals src/signals.cpp)
    add_executable(system_call src/system_call.cpp)
    add_executable(fork src/fork.cpp)
endif()

#add_executable(packaged_task src/packaged_task.cpp)

add_executable(std_visit src/std_visit.cpp)

add_executable(error_code src/error_code.cpp)


# if(${CMAKE_GNU_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} GREATER_EQUAL 13)
#     add_executable(printing_with_format src/printing_with_format.cpp)
# endif()

# if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND ${CMAKE_CXX_COMPILER_VERSION} GREATER_EQUAL 14)
#     add_executable(printing_with_format src/printing_with_format.cpp)
# elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND ${CMAKE_CXX_COMPILER_VERSION} GREATER_EQUAL 13)
#     add_executable(printing_with_format src/printing_with_format.cpp)
# elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND ${CMAKE_CXX_COMPILER_VERSION} GREATER_EQUAL 1900)
#     add_executable(printing_with_format src/printing_with_format.cpp)
# endif()









# Only build these on non-Windows platforms (Unix/Linux)
if(NOT WIN32)
    add_library(add SHARED src/add.cpp)
    add_executable(loadeding_libraries src/loadeding_libraries.cpp)
    target_link_libraries(loadeding_libraries dl)
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

message("\n########################################## ctest ##########################################\n")

if(ENABLED_TESTING)
    add_subdirectory(tests)
else()
    message("testing is not enabled")
endif()

message("\n########################################## tinyxml2 ##########################################\n")
if(ENABLE_XML)
    
    include(FetchContent)

    set(TINYXML2_VER "11.0.0")
    
    FetchContent_Declare(
        tinyxml2
        GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
        GIT_TAG ${TINYXML2_VER}
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    
    FetchContent_MakeAvailable(tinyxml2)

    message(STATUS "tinyxml2 version: ${TINYXML2_VER}")

    add_executable(tinyxml2_demo src/tinyxml2_demo.cpp)
    target_link_libraries(tinyxml2_demo tinyxml2)

else()
    message("XML is not enabled")
endif()


message("\n########################################## yaml-cpp ##########################################\n")
if(ENABLE_YAML)
    set(YAML_CPP_VER "0.9.0")

    FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG yaml-cpp-${YAML_CPP_VER}
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )

    FetchContent_MakeAvailable(yaml-cpp)

    message(STATUS "yaml-cpp version: ${YAML_CPP_VER}")

    add_executable(yaml-cpp_example src/yaml-cpp_example.cpp)
    target_link_libraries(yaml-cpp_example yaml-cpp)
else()
    message("yaml is not enabled")
endif()


message("\n########################################## Google benchmark ##########################################\n")
if(ENABLE_BENCHMARKING)
    set(BENCHMARK_VER "1.9.5")

    FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark
        GIT_TAG v${BENCHMARK_VER}
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(benchmark)

    message(STATUS "Google benchmark version: ${BENCHMARK_VER}")

    add_executable(benchmark_demo src/benchmark_demo.cpp)
    target_link_libraries(benchmark_demo benchmark::benchmark pthread)
else()
    message("Benchmarking is not enabled")
endif()


message("\n########################################## json ##########################################\n")
if(ENABLE_JSON)
    set(NLOHMANN_JSON_VER "3.12.0")
    FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG v${NLOHMANN_JSON_VER}
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )

    FetchContent_MakeAvailable(json)

    message(STATUS "nlohmann/json version: ${NLOHMANN_JSON_VER}")

    add_executable(json_example src/json_example.cpp)

    # Define a macro CMAKE_CURRENT_SOURCE_DIR with the value of CMAKE_CURRENT_SOURCE_DIR
    target_compile_definitions(json_example PRIVATE CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

    # Include the nlohmann/json directory
    target_include_directories(json_example PRIVATE ${json_SOURCE_DIR})
    target_link_libraries(json_example PRIVATE nlohmann_json::nlohmann_json)


else()
    message("json is not enabled")
endif()





message("\n########################################## tracy ##########################################\n")
if(ENABLE_TRACY)
    set(TRACY_VER "0.11.0")

    include(FetchContent)
    FetchContent_Declare(
        tracy
        GIT_REPOSITORY https://github.com/wolfpld/tracy.git
        GIT_TAG v${TRACY_VER}
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )

    FetchContent_MakeAvailable(tracy)

    message(STATUS "tracy version: ${TRACY_VER}")

    add_executable(tracy_profile src/tracy_profile.cpp)
    target_link_libraries(tracy_profile PUBLIC TracyClient)

else()
    message("tracy is not enabled")
endif()


message("\n########################################## fast-cpp-csv-parser ##########################################\n")
if(ENABLE_CSV)

    # No versioned releases; pinned to latest commit (2025-02-02)
    set(FAST_CPP_CSV_PARSER_COMMIT "574a9fe4d323ba63416877a4a5fe59088d37aa34")

    include(FetchContent)
    FetchContent_Declare(
        fast-cpp-csv-parser
        GIT_REPOSITORY https://github.com/ben-strasser/fast-cpp-csv-parser
        GIT_TAG ${FAST_CPP_CSV_PARSER_COMMIT}
        GIT_PROGRESS TRUE
    )

    FetchContent_MakeAvailable(fast-cpp-csv-parser)
    FetchContent_GetProperties(fast-cpp-csv-parser)

    string(SUBSTRING ${FAST_CPP_CSV_PARSER_COMMIT} 0 7 FAST_CPP_CSV_PARSER_SHORT)
    message(STATUS "fast-cpp-csv-parser commit: ${FAST_CPP_CSV_PARSER_SHORT} (2025-02-02, header-only, no versioned releases)")

    add_executable(csv_reading_example src/csv_reading_example.cpp)
    target_compile_definitions(csv_reading_example PRIVATE CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
    target_include_directories(csv_reading_example PRIVATE ${fast-cpp-csv-parser_SOURCE_DIR})

else()
    message("fast-cpp-csv-parser is not enabled")
endif()


message("\n########################################## spdlog ##########################################\n")
if(ENABLE_SPDLOG)
    set(SPDLOG_VER "1.14.1")

    # Use std::format (C++20) instead of bundled fmt to avoid Clang/Windows consteval bug
    set(SPDLOG_USE_STD_FORMAT ON CACHE BOOL "Use std::format instead of bundled fmt")

    # Fetch spdlog
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v${SPDLOG_VER}
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )

    FetchContent_MakeAvailable(spdlog)

    message(STATUS "spdlog version: ${SPDLOG_VER}")

    add_executable(spdlog_example src/spdlog_example.cpp)
    target_link_libraries(spdlog_example PRIVATE spdlog::spdlog)

else()
    message("spdlog is not enabled")
endif()



message("\n########################################## cURL (REST APIs / Webhooks) ##########################################\n")
if(ENABLE_CURL)
    FetchContent_Declare(
        curl
        GIT_REPOSITORY https://github.com/curl/curl.git
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )

    set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(CURL_USE_OPENSSL ON CACHE BOOL "" FORCE)
    set(CURL_USE_LIBPSL OFF CACHE BOOL "" FORCE)
    set(CURL_USE_LIBIDN2 OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(curl)

    message(STATUS "cURL version: ${CURL_VERSION}")

    # cURL is a solid C++ client for REST APIs.
    # Webhooks are typically just HTTP callbacks, so cURL works for those too.
    # add_executable(curl_rest_example src/curl_rest_example.cpp)
    # target_link_libraries(curl_rest_example PRIVATE CURL::libcurl)

else()
    message("cURL is not enabled")
endif()


message("\n########################################## gRPC ##########################################\n")
if(ENABLE_GRPC)
    FetchContent_Declare(
        gRPC
        GIT_REPOSITORY https://github.com/grpc/grpc.git
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )

    set(gRPC_INSTALL OFF CACHE BOOL "" FORCE)
    set(gRPC_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(gRPC_BUILD_GRPC_NODE_PLUGIN OFF CACHE BOOL "" FORCE)
    set(gRPC_BUILD_GRPC_RUBY_PLUGIN OFF CACHE BOOL "" FORCE)
    set(gRPC_BUILD_GRPC_PHP_PLUGIN OFF CACHE BOOL "" FORCE)
    set(gRPC_BUILD_GRPC_PYTHON_PLUGIN OFF CACHE BOOL "" FORCE)
    set(gRPC_BUILD_GRPC_CSHARP_PLUGIN OFF CACHE BOOL "" FORCE)
    set(gRPC_BUILD_GRPC_OBJECTIVE_C_PLUGIN OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(gRPC)

    message(STATUS "gRPC version: ${gRPC_CPP_VERSION}")

    # Official gRPC C++ support for RPC-style communication.
    # gRPC fetches its own dependencies (abseil, protobuf, re2, etc.) automatically.
    # add_executable(grpc_example src/grpc_example.cpp)
    # target_link_libraries(grpc_example PRIVATE grpc++ protobuf::libprotobuf)

else()
    message("gRPC is not enabled")
endif()


message("\n########################################## IXWebSocket ##########################################\n")
if(ENABLE_WEBSOCKET)
    FetchContent_Declare(
        ixwebsocket
        GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )

    set(USE_TLS OFF CACHE BOOL "" FORCE)
    set(USE_ZLIB OFF CACHE BOOL "" FORCE)
    set(IXWEBSOCKET_INSTALL OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(ixwebsocket)

    message(STATUS "IXWebSocket version: ${ixwebsocket_VERSION}")

    # IXWebSocket: actively maintained C++11 WebSocket client/server.
    # No Boost dependency. Optional TLS via mbedtls (set USE_TLS=ON).
    # WebSockets are often used for WebRTC signaling or standalone real-time communication.
    # add_executable(websocket_example src/websocket_example.cpp)
    # target_link_libraries(websocket_example PRIVATE ixwebsocket::ixwebsocket ${THREADING_LIB})

else()
    message("IXWebSocket is not enabled")
endif()


message("\n########################################## libdatachannel (WebRTC) ##########################################\n")
if(ENABLE_WEBRTC)
    FetchContent_Declare(
        libdatachannel
        GIT_REPOSITORY https://github.com/paullouisageneau/libdatachannel.git
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )

    set(NO_MEDIA OFF CACHE BOOL "" FORCE)
    set(NO_WEBSOCKET OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(libdatachannel)

    message(STATUS "libdatachannel version: ${libdatachannel_VERSION}")

    # libdatachannel is a C++17 standalone implementation of WebRTC Data Channels,
    # Media Transport, and WebSockets. The native WebRTC library (libwebrtc) is
    # primarily C++ but uses the GN build system; libdatachannel is CMake-friendly.
    # add_executable(webrtc_example src/webrtc_example.cpp)
    # target_link_libraries(webrtc_example PRIVATE LibDataChannel::LibDataChannel)

else()
    message("libdatachannel (WebRTC) is not enabled")
endif()


message("\n########################################## cppgraphqlgen (GraphQL) ##########################################\n")
if(ENABLE_GRAPHQL)
    FetchContent_Declare(
        cppgraphqlgen
        GIT_REPOSITORY https://github.com/microsoft/cppgraphqlgen.git
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )

    FetchContent_MakeAvailable(cppgraphqlgen)

    message(STATUS "cppgraphqlgen version: ${cppgraphqlgen_VERSION}")

    # cppgraphqlgen (Microsoft) provides a C++ GraphQL service and client implementation.
    # add_executable(graphql_example src/graphql_example.cpp)
    # target_link_libraries(graphql_example PRIVATE cppgraphqlgen::graphqlservice)

else()
    message("cppgraphqlgen (GraphQL) is not enabled")
endif()


message("\n########################################## XML SOAP (tinyxml2 + cURL) ##########################################\n")
if(ENABLE_SOAP)
    if(NOT ENABLE_XML)
        message(FATAL_ERROR "ENABLE_SOAP requires ENABLE_XML=ON (tinyxml2)")
    endif()
    if(NOT ENABLE_CURL)
        message(FATAL_ERROR "ENABLE_SOAP requires ENABLE_CURL=ON (libcurl)")
    endif()

    message(STATUS "XML SOAP example enabled (using tinyxml2 + cURL)")

    add_executable(xml_soap_example src/xml_soap_example.cpp)
    target_link_libraries(xml_soap_example PRIVATE tinyxml2 CURL::libcurl)

else()
    message("XML SOAP is not enabled")
endif()


if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
    add_executable(main src/main.cpp)
endif()

message("\n########################################## System Design  ##########################################\n")

add_executable(reusable_library src/reusable_library.cpp)
