cmake_minimum_required(VERSION 3.14)
project(microservices CXX)

if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE PATH "toolchain file")
endif()
message("toolchain file: ${CMAKE_TOOLCHAIN_FILE}")

# Find Protobuf and gRPC packages
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

message("gRPC_FOUND: "${gRPC_FOUND})
message("gRPC_VERSION: "${gRPC_VERSION})


message("Protobuf_FOUND: "${Protobuf_FOUND})
message("Protobuf_VERSION: "${Protobuf_VERSION})


set(GENERATED_DIR     "${CMAKE_SOURCE_DIR}/generated" )
set(PROTO_SRCS     "${GENERATED_DIR}/calculator.pb.cc" )
set(GRPC_SRCS     "${GENERATED_DIR}/calculator.grpc.pb.cc")

include_directories(${GENERATED_DIR})


add_executable(server src/server.cpp ${PROTO_SRCS} ${GRPC_SRCS})
add_executable(client src/client.cpp ${PROTO_SRCS} ${GRPC_SRCS})

target_link_libraries(server PRIVATE gRPC::grpc++ protobuf::libprotobuf)
target_link_libraries(client PRIVATE gRPC::grpc++ protobuf::libprotobuf)
