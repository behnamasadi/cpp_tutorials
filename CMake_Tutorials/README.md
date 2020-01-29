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
cmake -G"Visual Studio 12" ../  
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


## Built in Functions in Cmake

Always use lowercase function names. Always user lower case. Upper case is for variables.
The languages are C, CXX, Fortran, ASM, CUDA (CMake 3.8+), CSharp (3.8+), and SWIFT.

```
cmake_minimum_required(VERSION 3.1)  
project(my-cmake-project VERSION 1.2.3.4 DESCRIPTION "An example project with CMake" LANGUAGES CXX)  
```

```
add_subdirectory(src)
```

```
include(someother.cmake)
```

tell cmake where to look for *.h files
```
include_directories(${PROJECT_SOURCE_DIR}/src)
```
create library "libtools"
```
add_library(tools src/tools.cpp)
```
add executable main
```
add_executable(main src/ tools_main .cpp)
```
tell the linker to bind these objects together
```
target_link_libraries(main tools)
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

## Including Projects
Let say you have used google test in your project, to enable testing, first we add google test as submodule:

```
git submodule add  https://github.com/google/googletest.git extern/googletest
```
Then in the main CMakeList
```
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
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

## [How to add static/ shared library](Creating_Librares)

## [How to link your static/ shared library](Linking_to_Libraries)

                        
## Conditional Constructs
```
IF() ... ELSE()/ELSEIF() ... ENDIF()

OPTION(TESTING "Enable testing" OFF)
IF(testing_enabled)
	ADD_SUBDIRECTORY(tests)
ENDIF()


IF(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/temp")
	ADD_SUBDIRECTORY(src/temp)
ENDIF()

FOREACH() ... ENDFOREACH()
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
References:[1](https://gist.github.com/mbinna/), [2](https://cliutils.gitlab.io/modern-cmake/), 



