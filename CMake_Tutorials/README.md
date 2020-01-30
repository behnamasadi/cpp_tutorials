# CMake Tutorials


##  Typical project structure
- project
  - .gitignore
  - README.md
  - LICENCE.md
  - CMakeLists.txt
  - cmake
    - FindSomeLib.cmake
    - something_else.cmake
  - include
    - project
      - lib.hpp
  - src
    - CMakeLists.txt
    - lib.cpp
  - apps
    - CMakeLists.txt
    - app.cpp
  - tests
    - CMakeLists.txt
    - testlib.cpp
  - docs
    - CMakeLists.txt
  - extern
    - googletest
  - scripts
    - helper.py


## Setting the compiler
### clang
```
export CC=/usr/bin/clang
export CXX=/usr/bin/clang++
```
### gcc
```
export CC=/usr/bin/gcc
export CXX=/usr/bin/g++
```

## Cmake command Line Parameters

-S `<path to source directory>`   
-B `<path to build directory>`  
-D `<cache variable>=<value>`  
-G `<generator-name>`   
### Generating solution for Visual Studio, Xcode :
```
cmake -G"Visual Studio 16" ../  
cmake -GXcode ../   
cmake -G "Unix Makefiles"  ../  
```
### Visualising dependency graph:

--graphviz=[dependency graph outfile]  
--trace-source=CMakeLists.txt  

```
cmake   --graphviz=viz.dot  --trace-source=CMakeLists.txt
dot -Tsvg viz.dot -o viz.svg
```
### Lisiting all variables with description:

-L[A][H]  

## Scripting in CMake

```
if() ... else()/elseif() ... endif()

option(TESTING "Enable testing" OFF)
if(testing_enabled)
	add_subdirectory(tests)
endif()


if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/temp")
	add_subdirectory(src/temp)
endif()

foreach() ... endforeach()
```


## Built in Functions in Cmake

Always use lowercase function names. Always user lower case. Upper case is for variables.
The languages are C, CXX, Fortran, ASM, CUDA (CMake 3.8+), CSharp (3.8+), and SWIFT.

```
cmake_minimum_required(VERSION 3.1)  
project(my-cmake-project VERSION 1.2.3.4 DESCRIPTION "An example project with CMake" LANGUAGES CXX)  
```

### add_subdirectory()
Add a subdirectory to the build. The binary_dir specifies the directory in which to place the output files. The CMakeLists.txt in the added subdirectory will be called and executed.
```
add_subdirectory(src)
add_subdirectory(src binary_dir)
```
### include()
Load and run CMake code from the file given.
```
include(someother.cmake)
```
### include_directories()
tell cmake where to look for *.h files
```
include_directories(${PROJECT_SOURCE_DIR}/include)
```
### add_library()
create library "libtools"
```
add_library(tools STATIC|SHARED|MODULE src/tools.cpp)
```
[How to add static/ shared library](Creating_Librares)

[How to link your static/ shared library](Linking_to_Libraries)

### add_executable()
Adds an executable target called <name> to be built from the source files listed. 

```
add_executable(main [WIN32]  src/tools_main.cpp src/tools_lib.cpp)
```
### target_link_libraries()
Tell the linker to bind these objects together.
```
target_link_libraries(main tools)
```

### mark_as_advanced()
Mark the named cached variables as advanced. An advanced variable will not be displayed in any of the cmake GUIs unless the show advanced option is on, for instance to keep **CACHE**  clean:
```
mark_as_advanced(
    BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
    gmock_build_tests gtest_build_samples gtest_build_tests
    gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
)
```

## Variables
### Reading/ Setting Variables

Setting variables:
```
SET(xxx value)
```
Reading variables:
```
${XXX}
```
Environment variables:
```
$ENV{HOME}  
```
Paths may contain a space at any time and should always be quoted when they are a variable (never write ${VAR_PATH}, always should be "${VAR_PATH}").
```
$ENV{PATH}  
```

### Options
Options provide an option for the user to select as ON or OFF.
```
option(PACKAGE_TESTS "Build the tests" ON)
```

### Important Builtin variable Variables

```
MESSAGE( STATUS "PROJECT_NAME: " ${PROJECT_NAME} )  
MESSAGE( STATUS "PROJECT_VERSION: " ${PROJECT_VERSION} )  
MESSAGE( STATUS "BUILD_SHARED_LIBS: " ${BUILD_SHARED_LIBS} )  
MESSAGE( STATUS "BUILD_TESTING: " ${BUILD_TESTING} )  
MESSAGE( STATUS "CMAKE_C_COMPILER: " ${CMAKE_C_COMPILER} )  
MESSAGE( STATUS "CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER} )  
MESSAGE( STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE} )
MESSAGE( STATUS "CMAKE_INSTALL_PREFIX: " ${CMAKE_INSTALL_PREFIX} )
MESSAGE( STATUS "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR} )
MESSAGE( STATUS "CMAKE_SOURCE_DIR: " ${CMAKE_SOURCE_DIR} )
MESSAGE( STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR} )
MESSAGE( STATUS "PROJECT_BINARY_DIR: " ${PROJECT_BINARY_DIR} )
MESSAGE( STATUS "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR} )
MESSAGE( STATUS "CMAKE_BINARY_DIR: " ${CMAKE_BINARY_DIR} )
MESSAGE( STATUS "EXECUTABLE_OUTPUT_PATH: " "${EXECUTABLE_OUTPUT_PATH}" )
MESSAGE( STATUS "LIBRARY_OUTPUT_PATH:     " "${LIBRARY_OUTPUT_PATH}" )
MESSAGE( STATUS "CMAKE_MODULE_PATH: " "${CMAKE_MODULE_PATH}" )
MESSAGE( STATUS "CMAKE_INCLUDE_PATH: " "${CMAKE_INCLUDE_PATH}" )
MESSAGE( STATUS "CMAKE_PREFIX_PATH: " "${CMAKE_PREFIX_PATH}" )
MESSAGE( STATUS "CMAKE_LIBRARY_PATH: " "${CMAKE_LIBRARY_PATH}" )
MESSAGE( STATUS "CMAKE_SYSTEM_LIBRARY_PATH: " "${CMAKE_SYSTEM_LIBRARY_PATH}" )
MESSAGE( STATUS "CMAKE_CTEST_COMMAND: " ${CMAKE_CTEST_COMMAND} )  
MESSAGE( STATUS "CMAKE_GENERATOR: " ${CMAKE_GENERATOR} )  
```


## Communicating with your code
### Reading from CMake into your files
configure_file command copies the content of the first parameter (Version.h.in) to second parameter (Version) and substitute all CMake variables it finds. If you want to avoid replacing existing ${} syntax in your input file, use the @ONLY keyword. 
There's also a COPY_ONLY keyword if you are just using this as a replacement for file(COPY. 
```
configure_file ( "${PROJECT_SOURCE_DIR}/include/project/Version.h.in"  "${PROJECT_BINARY_DIR}/include/project/Version.h")
```
Content of Version.h.in
```
#pragma once

#define MY_VERSION_MAJOR @PROJECT_VERSION_MAJOR@
#define MY_VERSION_MINOR @PROJECT_VERSION_MINOR@
#define MY_VERSION_PATCH @PROJECT_VERSION_PATCH@
#define MY_VERSION_TWEAK @PROJECT_VERSION_TWEAK@
#define MY_VERSION "@PROJECT_VERSION@"
```
The conntet of "Version.h" will be 

```
#pragma once

#define MY_VERSION_MAJOR 1
#define MY_VERSION_MINOR 2
#define MY_VERSION_PATCH 3
#define MY_VERSION_TWEAK 4
#define MY_VERSION "1.2.3.4"
```

### CMake reading from your files

```
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/include/project/Version.hpp" VERSION)

string(REGEX MATCH "VERSION_MAJOR ([0-9]*)" _ ${VERSION})
set(VERSION_MAJOR ${CMAKE_MATCH_1})

string(REGEX MATCH "VERSION_MINOR ([0-9]*)" _ ${VERSION})
set(VERSION_MINOR ${CMAKE_MATCH_1})

string(REGEX MATCH "VERSION_PATCH ([0-9]*)" _ ${VERSION})
set(VERSION_PATCH ${CMAKE_MATCH_1})

message(STATUS "VERSION: ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

```



## Testing
You can use **ctest** to test your **unittests**. In your main CMakelist.txt:

```
option(BUILD_TESTING "this will automatically enable testing" ON)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    include(CTest)
    add_subdirectory(tests)
endif()

```
in your **tests** directory:
```
add_executable(test1 test1.cpp)
target_link_libraries(test1)
add_test(mytester test1)
```
The content of test1.cpp
```
int main(int argc, char ** argv)
{
    return 0;
}
```

Then in the build directory after building call **ctest**.

## GoogleTest
If you want to use google test for performing your unit test, first let's add it as submodule into your extern/googletest directory in your project:

```
git submodule add  https://github.com/google/googletest.git extern/googletest
```
Then, in your main CMakeLists.txt:
```
option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests)
endif()
```
Now, in your tests directory:
```
add_subdirectory("${PROJECT_SOURCE_DIR}/extern/googletest" "extern/googletest")
```
The extra path here is needed to correct the build path because we are calling it from a subdirectory. Then, create the following macros:
```
macro(package_add_test TESTNAME)
    # create an exectuable in which the tests will be stored
    add_executable(${TESTNAME} ${ARGN})
    # link the Google test infrastructure, mocking library, and a default main fuction to
    # the test executable.  Remove g_test_main if writing your own main function.
    target_link_libraries(${TESTNAME} gtest gmock gtest_main)
    # gtest_discover_tests replaces gtest_add_tests,
    # see https://cmake.org/cmake/help/v3.10/module/GoogleTest.html for more options to pass to it
    gtest_discover_tests(${TESTNAME}
        # set a working directory so your project root so that you can find test data via paths relative to the project root
        WORKING_DIRECTORY ${PROJECT_DIR}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
    )
    set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
endmacro()
```
And call it like this:
```
package_add_test(test1 test1.cpp)
```
if you're testing libraries and need to link in different libraries for different tests, you might use this:
```
macro(package_add_test_with_libraries TESTNAME FILES LIBRARIES TEST_WORKING_DIRECTORY)
    add_executable(${TESTNAME} ${FILES})
    target_link_libraries(${TESTNAME} gtest gmock gtest_main ${LIBRARIES})
    gtest_discover_tests(${TESTNAME}
        WORKING_DIRECTORY ${TEST_WORKING_DIRECTORY}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${TEST_WORKING_DIRECTORY}"
    )
    set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
endmacro()

package_add_test_with_libraries(test1 test1.cpp lib_to_test "${PROJECT_DIR}/european-test-data/")
```


### Download method
```
cmake_minimum_required(VERSION 3.10)
project(MyProject CXX)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

enable_testing() # Must be in main file

include(AddGoogleTest) # Could be in /tests/CMakeLists.txt
add_executable(SimpleTest SimpleTest.cu)
add_gtest(SimpleTest)

target_link_libraries(SimpleTest gtest gmock gtest_main)
add_test(SimpleTest SimpleTest)
```
### FetchContent

```
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.8.0
)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()
```

## Exporting Your Project
Command `find_package` has two modes: `Module` mode and `Config` mode. Module mode will look for `Find<mypackage>.cmake` and config mode 
will look for `MypackageConfig.cmake`.
### Find<mypackage>.cmake
This should be used when a project has no CMake support. Usually you create **Find<mypackage>.cmake** for a library and put under **cmake** directory in your porject. Then you should set the set the **CMAKE_MODULE_PATH** pointing to that.
```
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
```
you can add the project by find_package(packagename [version] [EXACT] [QUIET][REQUIRED] [[COMPONENTS])
	
### MypackageConfig.cmake


### Adding Subproject
For small and header only libraries, you can just use add_subdirectory() and include the entire  porject.
You can use **CMAKE_CURRENT_SOURCE_DIR** instead of **PROJECT_SOURCE_DIR** and 
```
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
...
endif()
```


## How to find CMake from arbitrary installed locations

Here I gathered several examples:
## PCL point cloud
```
set(PCL_DIR "$ENV{HOME}/usr/share/pcl-1.8/")
```

## OpenCV
```
set(OpenCV_DIR "$ENV{HOME}/usr/share/OpenCV/")
```

## glog
```
cmake  -Dglog_DIR=~/usr/lib/cmake/glog/
```



                        




## C++17 support
```
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

## Position independent code

Globally:
```
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
```
Explicitly turn it ON (or OFF) for a target:

```
set_target_properties(lib1 PROPERTIES POSITION_INDEPENDENT_CODE ON)
```

## Setting warning into errors
```
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
```

## Finding Memory leaking, Stack and Heap overflow
```
set(CMAKE_CXX_FLAGS "-fsanitize=address ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "-fno-omit-frame-pointer ${CMAKE_CXX_FLAGS}")
```
References:[1](https://gist.github.com/mbinna/), [2](https://cliutils.gitlab.io/modern-cmake/),[3](https://stackoverflow.com/questions/20746936/what-use-is-find-package-if-you-need-to-specify-cmake-module-path-anyway)
