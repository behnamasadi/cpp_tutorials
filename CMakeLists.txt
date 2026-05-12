cmake_minimum_required(VERSION 3.20)
project(cpp_tutorials LANGUAGES CXX C)

# This project uses per-config output directories, so it requires a
# multi-config generator. Ninja Multi-Config is the recommended choice.
if(NOT "${CMAKE_GENERATOR}" STREQUAL "Ninja Multi-Config")
    message(FATAL_ERROR "Please use -G \"Ninja Multi-Config\" for this project.")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# C++20 module scanning isn't compatible with several fetched dependencies
# (yaml-cpp, spdlog). Disable globally; targets that need modules can opt in.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(STATUS "using Visual Studio C++")
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    endif()
endif()

message(STATUS "CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")

# Modern threading: instead of hardcoding -pthread, let CMake pick the
# right flag/library per platform. Use Threads::Threads in target_link_libraries.
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Pulled in once and reused by every fetched dependency below.
include(FetchContent)

# ---------------------------------------------------------------------------
# Build options
# ---------------------------------------------------------------------------
option(ENABLE_JSON          "Enable nlohmann/json example"                 ON)
option(ENABLE_CSV           "Enable fast-cpp-csv-parser example"           ON)
option(ENABLE_YAML          "Enable yaml-cpp example"                      ON)
option(ENABLE_XML           "Enable tinyxml2 example"                      ON)
option(ENABLE_SPDLOG        "Enable spdlog example"                        ON)
option(ENABLE_BENCHMARKING  "Enable Google Benchmark"                      OFF)
option(ENABLE_TRACY         "Enable Tracy profiler"                        OFF)
option(ENABLE_TESTING       "Enable ctest tests"                           OFF)
option(ENABLE_SANITIZE      "Build with -fsanitize=address"                OFF)
option(ENABLE_CURL          "Enable cURL (REST APIs / Webhooks)"           OFF)
option(ENABLE_GRPC          "Enable gRPC (RPC)"                            OFF)
option(ENABLE_WEBSOCKET     "Enable IXWebSocket"                           OFF)
option(ENABLE_WEBRTC        "Enable libdatachannel (WebRTC)"               OFF)
option(ENABLE_GRAPHQL       "Enable cppgraphqlgen (GraphQL)"               OFF)
option(ENABLE_SOAP          "Enable XML SOAP example (needs XML + CURL)"   OFF)
option(PRINT_INCLUDES       "Print list of all included headers"           OFF)

if(ENABLE_SANITIZE)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()

# ---------------------------------------------------------------------------
# Core language / library examples
# ---------------------------------------------------------------------------
add_executable(pointers src/pointers/pointers.cpp)
add_executable(string src/string.cpp)
add_executable(format src/format.cpp)
add_executable(exception_handling src/exception_handling.cpp)
add_executable(assert src/assert.cpp)
add_executable(return_abort_exit src/return_abort_exit.cpp)
add_executable(rvalue_lvalue src/rvalue_lvalue.cpp)
add_executable(primitive_data_types_numerical_limits_accuracy src/primitive_data_types_numerical_limits_accuracy.cpp)
add_executable(arrays src/arrays.cpp)
add_executable(date_time src/date_time.cpp)
add_executable(dynamic_memory_allocation src/dynamic_memory_allocation.cpp)
add_executable(template src/template.cpp)
add_executable(inline_functions src/inline_functions.cpp)
add_executable(algorithms_library src/algorithms_library.cpp)
add_executable(execution_policies src/execution_policies.cpp)
add_executable(vector src/vector.cpp)
add_executable(queue src/queue.cpp)
add_executable(variadic_templates src/variadic_templates.cpp)
add_executable(enum src/enum.cpp)
add_executable(switch_case src/switch_case.cpp)
add_executable(set_map_pair_tuple src/set_map_pair_tuple.cpp)
add_executable(error_handling src/error_handling.cpp)
add_executable(bitwise_operations src/bitwise_operations.cpp)
add_executable(big_endian_little_endian src/big_endian_little_endian.cpp)
add_executable(bitset_bit_field src/bitset_bit_field.cpp)
add_executable(hash src/hash.cpp)
add_executable(containers src/containers.cpp)
add_executable(iterator_loop src/iterator_loop.cpp)
add_executable(function_pointer src/function_pointer.cpp)
add_executable(bind src/bind.cpp)
add_executable(function src/function.cpp)
add_executable(functions src/functions.cpp)
add_executable(lambda src/lambda.cpp)
add_executable(unions src/unions.cpp)
add_executable(basic_IO_operation_streams src/basic_IO_operation_filesystem_streams_reading_writing_files_formating_output_cin_cout_scanf_printf_gets_puts_getline.cpp)
add_executable(filesystem src/filesystem.cpp)
add_executable(const_constexpr_mutable src/const_constexpr_mutable.cpp)
add_executable(immutable_objects src/immutable_objects.cpp)
add_executable(literals src/literals.cpp)
add_executable(ternary src/ternary.cpp)
add_executable(lists src/lists.cpp)
add_executable(type_traits src/type_traits.cpp)
add_executable(decay src/decay.cpp)
add_executable(typedef_type_alias_using_keyword src/typedef_type_alias_using_keyword.cpp)
add_executable(most_vexing_parse src/most_vexing_parse.cpp)
add_executable(VTABLE_and_VPTR src/VTABLE_and_VPTR.cpp)
add_executable(allocator src/allocator.cpp)
add_executable(callbacks src/callbacks.cpp)
add_executable(template_specialization_tag_dispatch src/template_specialization_tag_dispatch.cpp)
add_executable(copy_constructor_move_constructor src/copy_constructor_move_constructor.cpp)
add_executable(shallow_copy_deep_copy src/shallow_copy_deep_copy.cpp)
add_executable(forward src/forward.cpp)
add_executable(track_memory_allocations src/track_memory_allocations.cpp)
add_executable(nested_namespaces src/nested_namespaces.cpp)
add_executable(type_dispatch_integral_constant_true_false_type src/type_dispatch_integral_constant_true_false_type.cpp)
add_executable(set_argv_argc src/set_argv_argc.cpp)
add_executable(arguments_parser_example src/arguments_parser_example.cpp)
add_executable(initialization src/initialization.cpp)
add_executable(scope_resolution_operator src/scope_resolution_operator.cpp)
add_executable(argument_dependent_lookup src/argument_dependent_lookup.cpp)
add_executable(random_number_generation src/random_number_generation.cpp)
add_executable(heap_and_stack_memory_layout_of_C_programs src/heap_and_stack_memory_layout_of_C_programs.cpp)
add_executable(memory_checking src/memory_checking.cpp)
add_executable(tricky_questions src/tricky_questions.cpp)
add_executable(metaprogramming src/metaprogramming.cpp)
add_executable(regex_mathch_search src/regex_mathch_search.cpp)
add_executable(lexical_analyzer src/lexical_analyzer.cpp)
add_executable(optional src/optional.cpp)
add_executable(std_invoke src/std_invoke.cpp)
add_executable(structured_binding_declaration src/structured_binding_declaration.cpp)
add_executable(std_visit src/std_visit.cpp)
add_executable(error_code src/error_code.cpp)
add_executable(any src/any.cpp)
add_executable(buffer_overflow src/buffer_overflow.cpp)
add_executable(stack_overflow src/stack_overflow.cpp)
add_executable(stack_unwinding src/stack_unwinding.cpp)

# macro.cpp expects the LOGING preprocessor define. Scope it to that target
# only — defining it globally bleeds into every executable below.
add_executable(macro src/macro.cpp)
target_compile_definitions(macro PRIVATE LOGING=0)

# Sources under src/class/
add_executable(diamond_problem_virtual_inheritance src/class/diamond_problem_virtual_inheritance.cpp)
add_executable(shadowing src/class/shadowing.cpp)
add_executable(protected_friend_class_function src/class/protected_friend_class_function.cpp)
add_executable(virtual_function_abstract_class src/class/virtual_function_abstract_class.cpp)
add_executable(multiple_inheritance_polymorphism src/class/multiple_inheritance_polymorphism.cpp)
add_executable(private_public_protected_inheritance src/class/private_public_protected_inheritance.cpp)
add_executable(cast_Base_to_Derived_to_Base src/class/cast_Base_to_Derived_to_Base.cpp)
add_executable(operator_overloading src/class/operator_overloading.cpp)
add_executable(default_0_delete_meaning src/class/default_0_delete_meaning.cpp)
add_executable(special_member_functions src/class/special_member_functions.cpp)
add_executable(object_slicing src/class/object_slicing.cpp)
add_executable(static_member_function src/class/static_member_function.cpp)
add_executable(constructor_initialization_list src/class/constructor_initialization_list.cpp)
add_executable(inheritance src/class/inheritance.cpp)
add_executable(class_forward_declaration src/class/class_forward_declaration.cpp)
add_executable(explicit_constructor src/class/explicit_constructor.cpp)
add_executable(abstract_class_vs_interface src/class/abstract_class_vs_interface.cpp)
add_executable(virtual_destructor_virtual_constructor src/class/virtual_destructor_virtual_constructor.cpp)
add_executable(header_guard src/class/header_guard/main.cpp)
add_executable(circular_dependency
    src/class/circular_dependency/circular_dependency.cpp
    src/class/circular_dependency/classA.cpp
    src/class/circular_dependency/classB.cpp)

# extern: cross-TU global variable and extern "C" linkage to a C function.
add_executable(extern_variable
    src/extern/variable/main.cpp
    src/extern/variable/global.cpp)
add_executable(extern_function
    src/extern/function/main.cpp
    src/extern/function/foo.c)

# ---------------------------------------------------------------------------
# Advanced C++ concepts / idioms
# ---------------------------------------------------------------------------
add_executable(RTTI src/RTTI.cpp)
add_executable(CRTP src/CRTP.cpp)
add_executable(double_dispatch src/double_dispatch.cpp)
add_executable(RVO_NRVO_copy_elision src/RVO_NRVO_copy_elision.cpp)
add_executable(pimpl src/pimpl/main.cpp src/pimpl/studentpimpl.cpp)
add_executable(return_type_resolver src/return_type_resolver.cpp)
add_executable(SFINAE src/SFINAE.cpp)
add_executable(concepts src/concepts.cpp)
add_executable(coroutines src/coroutines.cpp)
add_executable(type_erasure src/type_erasure.cpp)
add_executable(optimizing_cpp src/optimizing_cpp/index.cpp)
add_executable(copy_and_swap src/copy_and_swap.cpp)
if(PRINT_INCLUDES)
    if(MSVC)
        target_compile_options(copy_and_swap PRIVATE "/showIncludes")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(copy_and_swap PRIVATE "-H")
    endif()
endif()

# RAII uses std::jthread.
add_executable(RAII src/RAII.cpp)
target_link_libraries(RAII PRIVATE Threads::Threads)

# conditional_compilation demonstrates CMake-driven preprocessor flags.
add_executable(conditional_compilation src/conditional_compilation.cpp)
target_compile_definitions(conditional_compilation PRIVATE
    MY_FEATURE=1
    $<$<CONFIG:Debug>:DEBUG_FEATURE=1>)

# ---------------------------------------------------------------------------
# Multithreading examples (all linked against Threads::Threads)
# ---------------------------------------------------------------------------
set(MT_TARGETS
    creating_and_terminating_threads
    differentiating_between_threads
    sleeping_threads
    join_detach_threads
    mutex
    condition_variable
    async_future_promise
    dead_lock
    thread_safe
    packaged_task
    race_condition
    unique_lock
    lock_guard
    scoped_lock
    semaphor
    thread_pool
    thread_memory_error_detector)

foreach(target IN LISTS MT_TARGETS)
    add_executable(${target} src/multithreading/${target}.cpp)
    target_link_libraries(${target} PRIVATE Threads::Threads)
endforeach()

# ---------------------------------------------------------------------------
# Platform-conditional targets
# ---------------------------------------------------------------------------
if(UNIX)
    # core_dump uses <unistd.h>.
    add_executable(core_dump src/core_dump.cpp)
endif()

if(NOT WIN32)
    add_executable(align src/align.cpp)
    add_executable(signals src/signals.cpp)
    add_executable(system_call src/system_call.cpp)
    add_executable(fork src/fork.cpp)

    # Plugin-loading demo: builds a shared library, then an executable that
    # dlopen()s it.
    add_library(add SHARED src/add.cpp)
    add_executable(loadeding_libraries src/loadeding_libraries.cpp)
    target_link_libraries(loadeding_libraries PRIVATE ${CMAKE_DL_LIBS})
endif()

# Local sandbox file (gitignored). Only build if it exists.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
    add_executable(main src/main.cpp)
endif()

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------
message("\n########################################## ctest ##########################################\n")
if(ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
else()
    message(STATUS "testing is not enabled (set ENABLE_TESTING=ON)")
endif()

# ---------------------------------------------------------------------------
# Third-party deps via FetchContent
# ---------------------------------------------------------------------------
message("\n########################################## tinyxml2 ##########################################\n")
if(ENABLE_XML)
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
    target_link_libraries(tinyxml2_demo PRIVATE tinyxml2)
else()
    message(STATUS "XML is not enabled")
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
    target_link_libraries(yaml-cpp_example PRIVATE yaml-cpp)
else()
    message(STATUS "yaml is not enabled")
endif()


message("\n########################################## Google benchmark ##########################################\n")
if(ENABLE_BENCHMARKING)
    set(BENCHMARK_VER "1.9.5")
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark
        GIT_TAG v${BENCHMARK_VER}
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(benchmark)
    message(STATUS "Google benchmark version: ${BENCHMARK_VER}")

    add_executable(benchmark_demo src/benchmark_demo.cpp)
    target_link_libraries(benchmark_demo PRIVATE benchmark::benchmark Threads::Threads)
else()
    message(STATUS "Benchmarking is not enabled")
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
    target_compile_definitions(json_example PRIVATE
        CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
    target_link_libraries(json_example PRIVATE nlohmann_json::nlohmann_json)
else()
    message(STATUS "json is not enabled")
endif()


message("\n########################################## tracy ##########################################\n")
if(ENABLE_TRACY)
    set(TRACY_VER "0.11.0")
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
    target_link_libraries(tracy_profile PRIVATE TracyClient)
else()
    message(STATUS "tracy is not enabled")
endif()


message("\n########################################## fast-cpp-csv-parser ##########################################\n")
if(ENABLE_CSV)
    # No versioned releases; pinned to a commit (2025-02-02).
    set(FAST_CPP_CSV_PARSER_COMMIT "574a9fe4d323ba63416877a4a5fe59088d37aa34")
    FetchContent_Declare(
        fast-cpp-csv-parser
        GIT_REPOSITORY https://github.com/ben-strasser/fast-cpp-csv-parser
        GIT_TAG ${FAST_CPP_CSV_PARSER_COMMIT}
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(fast-cpp-csv-parser)
    string(SUBSTRING ${FAST_CPP_CSV_PARSER_COMMIT} 0 7 FAST_CPP_CSV_PARSER_SHORT)
    message(STATUS "fast-cpp-csv-parser commit: ${FAST_CPP_CSV_PARSER_SHORT} (header-only)")

    add_executable(csv_reading_example src/csv_reading_example.cpp)
    target_compile_definitions(csv_reading_example PRIVATE
        CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
    target_include_directories(csv_reading_example PRIVATE ${fast-cpp-csv-parser_SOURCE_DIR})
else()
    message(STATUS "fast-cpp-csv-parser is not enabled")
endif()


message("\n########################################## spdlog ##########################################\n")
if(ENABLE_SPDLOG)
    set(SPDLOG_VER "1.14.1")
    # Use std::format (C++20) instead of bundled fmt to avoid a Clang/Windows
    # consteval bug.
    set(SPDLOG_USE_STD_FORMAT ON CACHE BOOL "Use std::format instead of bundled fmt")
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
    message(STATUS "spdlog is not enabled")
endif()


message("\n########################################## cURL (REST APIs / Webhooks) ##########################################\n")
if(ENABLE_CURL)
    set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(CURL_USE_OPENSSL ON CACHE BOOL "" FORCE)
    set(CURL_USE_LIBPSL OFF CACHE BOOL "" FORCE)
    set(CURL_USE_LIBIDN2 OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        curl
        GIT_REPOSITORY https://github.com/curl/curl.git
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(curl)
    message(STATUS "cURL version: ${CURL_VERSION}")
    # add_executable(curl_rest_example src/curl_rest_example.cpp)
    # target_link_libraries(curl_rest_example PRIVATE CURL::libcurl)
else()
    message(STATUS "cURL is not enabled")
endif()


message("\n########################################## gRPC ##########################################\n")
if(ENABLE_GRPC)
    set(gRPC_INSTALL OFF CACHE BOOL "" FORCE)
    set(gRPC_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    foreach(plugin NODE RUBY PHP PYTHON CSHARP OBJECTIVE_C)
        set(gRPC_BUILD_GRPC_${plugin}_PLUGIN OFF CACHE BOOL "" FORCE)
    endforeach()
    FetchContent_Declare(
        gRPC
        GIT_REPOSITORY https://github.com/grpc/grpc.git
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(gRPC)
    message(STATUS "gRPC version: ${gRPC_CPP_VERSION}")
    # add_executable(grpc_example src/grpc_example.cpp)
    # target_link_libraries(grpc_example PRIVATE grpc++ protobuf::libprotobuf)
else()
    message(STATUS "gRPC is not enabled")
endif()


message("\n########################################## IXWebSocket ##########################################\n")
if(ENABLE_WEBSOCKET)
    set(USE_TLS OFF CACHE BOOL "" FORCE)
    set(USE_ZLIB OFF CACHE BOOL "" FORCE)
    set(IXWEBSOCKET_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        ixwebsocket
        GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(ixwebsocket)
    message(STATUS "IXWebSocket version: ${ixwebsocket_VERSION}")
    # add_executable(websocket_example src/websocket_example.cpp)
    # target_link_libraries(websocket_example PRIVATE ixwebsocket::ixwebsocket Threads::Threads)
else()
    message(STATUS "IXWebSocket is not enabled")
endif()


message("\n########################################## libdatachannel (WebRTC) ##########################################\n")
if(ENABLE_WEBRTC)
    set(NO_MEDIA OFF CACHE BOOL "" FORCE)
    set(NO_WEBSOCKET OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        libdatachannel
        GIT_REPOSITORY https://github.com/paullouisageneau/libdatachannel.git
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(libdatachannel)
    message(STATUS "libdatachannel version: ${libdatachannel_VERSION}")
    # add_executable(webrtc_example src/webrtc_example.cpp)
    # target_link_libraries(webrtc_example PRIVATE LibDataChannel::LibDataChannel)
else()
    message(STATUS "libdatachannel (WebRTC) is not enabled")
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
    # add_executable(graphql_example src/graphql_example.cpp)
    # target_link_libraries(graphql_example PRIVATE cppgraphqlgen::graphqlservice)
else()
    message(STATUS "cppgraphqlgen (GraphQL) is not enabled")
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
    message(STATUS "XML SOAP is not enabled")
endif()


# ---------------------------------------------------------------------------
# Tool availability checks (informational; doesn't fail configure)
# ---------------------------------------------------------------------------
message("\n########################################## clang-format ##########################################\n")
find_program(CLANG_FORMAT_EXECUTABLE NAMES clang-format clang-format-18 clang-format-17 clang-format-16 clang-format-15)
if(CLANG_FORMAT_EXECUTABLE)
    execute_process(
        COMMAND ${CLANG_FORMAT_EXECUTABLE} --version
        OUTPUT_VARIABLE CLANG_FORMAT_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    message(STATUS "clang-format found: ${CLANG_FORMAT_EXECUTABLE}")
    message(STATUS "clang-format version: ${CLANG_FORMAT_VERSION}")
else()
    message(WARNING "clang-format is not installed. Install it to enable format-on-save in VS Code.\n"
                    "  Ubuntu/Debian: sudo apt install clang-format\n"
                    "  macOS:         brew install clang-format\n"
                    "  Windows:       choco install llvm")
endif()


message("\n########################################## clang-tidy ##########################################\n")
find_program(CLANG_TIDY_EXECUTABLE NAMES clang-tidy clang-tidy-18 clang-tidy-17 clang-tidy-16 clang-tidy-15)
if(CLANG_TIDY_EXECUTABLE)
    execute_process(
        COMMAND ${CLANG_TIDY_EXECUTABLE} --version
        OUTPUT_VARIABLE CLANG_TIDY_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    message(STATUS "clang-tidy found: ${CLANG_TIDY_EXECUTABLE}")
    message(STATUS "clang-tidy version:\n${CLANG_TIDY_VERSION}")
else()
    message(WARNING "clang-tidy is not installed. Install it for static analysis.\n"
                    "  Ubuntu/Debian: sudo apt install clang-tidy\n"
                    "  macOS:         brew install llvm\n"
                    "  Windows:       choco install llvm")
endif()


message("\n########################################## System Design  ##########################################\n")
add_executable(reusable_library src/reusable_library.cpp)
