cmake_minimum_required(VERSION 3.16.3)

if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE PATH "toolchain file")
endif()

message("toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
project(microservices)

find_package(gRPC CONFIG REQUIRED)

message("gRPC_FOUND: "${gRPC_FOUND})
message("gRPC_VERSION: "${gRPC_VERSION})
