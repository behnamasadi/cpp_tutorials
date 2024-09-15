cmake_minimum_required(VERSION 3.14)
project(microservices)


if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE PATH "toolchain file")
endif()
message("toolchain file: ${CMAKE_TOOLCHAIN_FILE}")


# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Find crow package
find_package(Crow)

add_executable(user_service src/user_service.cpp)
target_link_libraries(user_service PRIVATE Crow::Crow)


add_executable(product_service src/product_service.cpp)
target_link_libraries(product_service PRIVATE Crow::Crow)


add_executable(order_service src/order_service.cpp)
target_link_libraries(order_service PRIVATE Crow::Crow)

add_executable(payment_service src/payment_service.cpp)
target_link_libraries(payment_service PRIVATE Crow::Crow)

#add_executable(main src/main.cpp)
#target_link_libraries(main PRIVATE Crow::Crow)
