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
```
CC=clang CXX=clang++ cmake ../  
CC=gcc CXX=g++ cmake ../  
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

cmake_minimum_required(VERSION 3.1)  

project(my-cmake-project VERSION 1.2.0.3 DESCRIPTION "An example project with CMake" LANGUAGES CXX)  


#The languages are C, CXX, Fortran, ASM, CUDA (CMake 3.8+), CSharp (3.8+), and SWIFT




#tell cmake to output binaries here:
set( EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set( LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
# tell cmake where to look for *.h files
include_directories(${PROJECT_SOURCE_DIR}/src)
# create library "libtools"
add_library(tools src/tools.cpp)
# add executable main
add_executable(main src/ tools_main .cpp)
# tell the linker to bind these objects together
target_link_libraries(main tools)


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


### How to print all cmake variable
1)First way:
```	
get_cmake_property(_variableNames VARIABLES)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()
```

2)Second way:
```
cmake -LAH ../
```


### Important Builtin variable Variables

```
MESSAGE( STATUS "PROJECT_NAME: " ${PROJECT_NAME} )

MESSAGE( STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE} )

MESSAGE( STATUS "CMAKE_INSTALL_PREFIX: " ${CMAKE_INSTALL_PREFIX} )

MESSAGE( STATUS "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR} )

MESSAGE( STATUS "CMAKE_SOURCE_DIR:         " ${CMAKE_SOURCE_DIR} )

MESSAGE( STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR} )

MESSAGE( STATUS "PROJECT_BINARY_DIR: " ${PROJECT_BINARY_DIR} )

MESSAGE( STATUS "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR} )

MESSAGE( STATUS "CMAKE_BINARY_DIR:         " ${CMAKE_BINARY_DIR} )

MESSAGE( STATUS "EXECUTABLE_OUTPUT_PATH: " ${EXECUTABLE_OUTPUT_PATH} )

MESSAGE( STATUS "LIBRARY_OUTPUT_PATH:     " ${LIBRARY_OUTPUT_PATH} )

MESSAGE( STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH} )

MESSAGE( STATUS "CMAKE_INCLUDE_PATH: " ${CMAKE_INCLUDE_PATH} )

MESSAGE( STATUS "CMAKE_PREFIX_PATH: " ${CMAKE_PREFIX_PATH} )

MESSAGE( STATUS "CMAKE_LIBRARY_PATH: " ${CMAKE_LIBRARY_PATH} )

MESSAGE( STATUS "CMAKE_SYSTEM_LIBRARY_PATH: " ${CMAKE_SYSTEM_LIBRARY_PATH} )
```

### How to set CMake variables via CMake parameters
```
cmake -D<VAR_NAME>=<VALUE>
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

## C++11 support

```
INCLUDE(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
IF(COMPILER_SUPPORTS_CXX11)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
ELSEIF(COMPILER_SUPPORTS_CXX0X)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -pthread")
ELSE()
  MESSAGE(ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
ENDIF()
```


## C++14 support
```
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
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


