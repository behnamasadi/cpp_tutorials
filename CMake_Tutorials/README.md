# CMake Tutorials

##  Typical project structure

```
project  
├──.gitignore  
├──README.md  
├──LICENCE.md  
├──CMakeLists.txt  
├──cmake  
│    └──FindSomeLib.cmake  
├──include  
│    └──poject  
│        └── lib.hpp  
├──src  
│    ├──CMakeLists.txt  
│    ├──lib.cpp  
│    └──include  
│        └──private_header.hpp  
├──apps  
│    ├──CMakeLists.txt  
│    └──app.cpp  
├──tests  
│    ├──CMakeLists.txt  
│    └──testlib.cpp  
├──docs  
│    └── CMakeLists.txt  
├──extern  
|    └──googletest  
└──scripts  
     └──helper.py  
```
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
## CMake Variables, Cache Variables and Options

### Variables
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

You can set a variable in the scope immediately above your current one with PARENT_SCOPE at the end. Let say you have the followings:
```
root
├── src
│   └── CMakeLists.txt
└── CMakeLists.txt
```
in your root CMake 
```
set( BAR "Bar from root." )
```
In your src CMake
```
set( BAR "Bar from src." ) #<-- set in this scope
set( BAR ${BAR} PARENT_SCOPE ) #<-- set in the parent scope too
```
Then in your root CMake, put this and observe the changes in the value of  `BAR`:
```
MESSAGE( STATUS "root: " ${BAR} )
add_subdirectory("${PROJECT_SOURCE_DIR}/src")
MESSAGE( STATUS "root: " ${BAR} )  
```


### Cache Variables
If you want to set a variable from the command line, CMake offers a variable cache. For example yu can define `MY_LIBRARY_VERSION`. The syntax for declaring a variable and setting it if it is not already set is:
```
set(MY_CACHE_VARIABLE "VALUE" CACHE STRING "Description")
```
Then you can sent the value via command line:
```
cmake -DMY_CACHE_VARIABLE=2.3.6.9
```
If you use keyword force, user can not override your value:
```
set(MY_LIBRARY_VERSION_MAJOR 1 CACHE STRING "major version" FORCE)
```
These are common CMake options to most packages:

`-DCMAKE_BUILD_TYPE` Pick from Release, RelWithDebInfo, Debug, or sometimes more.  
`-DCMAKE_INSTALL_PREFIX` The location to install to. System install on UNIX would often be /usr/local (the default), user directories are often ~/.local, or you can pick a folder.  
`-DBUILD_SHARED_LIBS` You can set this ON or OFF to control the default for shared libraries (the author can pick one vs. the other explicitly instead of using the default, though)  
`-DBUILD_TESTING` This is a common name for enabling tests, not all packages use it, though, sometimes with good reason.  


### Options
Options provide an option for the user to select as ON or OFF.
```
option(PACKAGE_TESTS "Build the tests" ON)
```

### Important Variables

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

## Properties
The other way CMake stores information is in properties. This is like a variable, but it is attached to some other item, like a directory or a target. There are two ways to set properties:
The first form is more general, 
```
set_property(TARGET TargetName PROPERTY CXX_STANDARD 11)
```

The second is a shortcut for setting several properties on one target
`set_target_properties(TargetName PROPERTIES CXX_STANDARD 11)`


### Generating solution for Visual Studio, Xcode :
To use an IDE, either pass -G"name of IDE" if CMake can produce that IDE's files (like Xcode, Visual Studio), 
or open the CMakeLists.txt file from your IDE if that IDE has built in support for CMake (CLion, QtCreator, many others).

```
cmake -GXcode ../   
cmake -G "Unix Makefiles"  ../  
```
### Visual Studio
Visual Studio 2019:
```
cmake -G"Visual Studio 16" ..
```

Visual Studio 2017
```
cmake -G"Visual Studio 15" ..
```

Visual Studio 2015 
```
cmake -G"Visual Studio 14" ..
```                   
Visual Studio 2013
``
cmake -G"Visual Studio 12" ..
``

and to build (you can use -v for verbose builds and -j N for parallel builds on N cores) :

```
cmake --build . -v -j 8
```
buid test
```
cmake --build . --target test
```
buid docs
```
cmake --build . --target docs
```
buid and install (the installation path would be `CMAKE_INSTALL_PREFIX`)
```
cmake --build . --target install --config Release
```
### Visualising dependency graph:

--graphviz=[dependency graph outfile]  
--trace-source=CMakeLists.txt  

```
cmake   --graphviz=viz.dot  --trace-source=CMakeLists.txt
dot -Tsvg viz.dot -o viz.svg
```
### Lisiting all variables with description:
```
-L[A][H]  
```
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


## Built-in Commands and Functions in CMake

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

### target_include_directories()

`include_directories(given_include_dir_path)` affects all the targets in its CMakeLists, as well as those in all sub directories added after the point of its call. They would have access to "given_include_dir_path" for including headers.

`target_include_directories(target <INTERFACE|PUBLIC|PRIVATE> target_include_directory_path)` would add an include directory for a specific target. 
The target must have been created by a command such as `add_executable()` or `add_library()`. The reason that we might use both of them is the following:
You should declare your public API of your library for the third-party applications, so you place them under `<project_root/include/project_name>`. You might have some headers that are being used only by your application and you don't need (or want) to give them to the public, so you place them under your source directory and use target_include_directories() to make them accessible by your target. Notice that, private headers should not be installed.

#### PUBLIC, PRIVATE, and INTERFACE
Other target couldbe compiled against your targets and they might need to access the headers that you have 
used in your target, by declaring them as `PUBLIC` other targets can have access to those include directories that you added
to your target and by using `PRIVATE` those include directories are nly available for your target. For example:

`target_include_directories(A PRIVATE ${Boost_INCLUDE_DIRS})` if you only use those Boost headers inside your source files (.cpp) or private header files (.h).

`target_include_directories(A PUBLIC ${Boost_INCLUDE_DIRS})` if you use those Boost headers in your public header files, which are included BOTH in some of A's source files and might also be included in any other client of your A library.

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

### install()

When you call the `make install`  or `cmake --build . --target install` this command will be executed.
In the following example, we export the properties of our TARGETS into an EXPORT callled FooTargets, so FooTargets know where we 
put the lib, dll, share dobject etc (On windows the dll will be in bin directory and .lib in the lib directory, On linux .a and .so will be in lib directory).

```
install( TARGETS Foo EXPORT FooTargets
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
RUNTIME DESTINATION bin
INCLUDES DESTINATION include)
```
But this only stores this information in an object named "FooTargets". The following will write the data that our EXPORT (FooTargets) contains into disk:
```
install(EXPORT FooTargets
FILE FooTargets.cmake
NAMESPACE FOO::
DESTINATION lib/cmake/Foo
)
```

## Communicating with your code
### Reading from CMake into your files
configure_file command copies the content of the first parameter (Version.h.in) to second parameter (Version) and substitute all CMake variables it finds. If you want to avoid replacing existing ${} syntax in your input file, use the @ONLY keyword. Passing @ONLY option to configure_file forces CMake to not touch ${...} expressions but substitute only @VAR@ ones.

```
configure_file(<input> <output> [@ONLY])
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

you can store your test configuration in file like "build.cmake" 
```
set(CTEST_SOURCE_DIRECTORY "/source")
set(CTEST_BINARY_DIRECTORY "/binary")
set(ENV{CXXFLAGS "--coverage"})
set(CTEST_CMAKE_GENERATOR "Ninja")
set(CTEST_USE_LAUNCHERS 1)
set(CTEST_COVERAGE_COMMAND "gcov")
set(CTEST_MEMORYCHECK_COMMAND "valgrind")

ctest_start("Continuous")
ctest_configure()
ctest_build()
ctest_test()
ctest_coverage()
ctest_memcheck()
ctest_submit()
```
Execute it by:
```
ctest -S build.cmake
```
And then send the results to a dashboard server.

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
Now, in the CMakeLists of your tests directory:
```
add_subdirectory("${PROJECT_SOURCE_DIR}/extern/googletest" "extern/googletest")
set(INSTALL_GTEST OFF)
add_executable(footest foo.cpp)
target_link_libraries(footest gtest_main)
gtest_discover_tests(footest)
```
The extra path here is needed to correct the build path because we are calling it from a subdirectory. 
Then copy this under `foo.cpp`:
```
#include "gtest/gtest.h"
TEST(Foo, Sum)
{
  EXPECT_EQ(2, 1 + 1);
}
```
And run it with:
```
ctest
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

There are 3 ways to access your project from another project:  
### 1) Adding Subproject
Adding your project with add_subdirectory().  For small and header only libraries, you can just use add_subdirectory() and include the entire  porject.
You can use **CMAKE_CURRENT_SOURCE_DIR** instead of **PROJECT_SOURCE_DIR** and 
```
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
...
endif()
```

### 2) Exporting build directory of your project.  
To use the build directory of one project in another project, you will need to export targets.
```
export(TARGETS taget1 target2  FILE MyLibTargets.cmake)
```

This puts the targets you have listed into a file in the build directory. Now, to allow CMake to find this package, export the package into the` $HOME/.cmake/packages` folder:
```
set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)
export(PACKAGE MyLib)
```
Now, if you find_package(MyLib), CMake can find the build folder.


### 3) Installing your project and calling find_package()
Let say you have the following project:
```
root
├── Lib1
│   └── CMakeLists.txt
|   └── src.cpp
└── CMakeLists.txt
└── Config.cmake.in

```
The content of Lib1/CMakeLists.txt:
```
add_library(lib1 src.cpp)
add_library(${CMAKE_PROJECT_NAME}::lib1 ALIAS lib1)
include(GNUInstallDirs)
install( 
  TARGETS lib1
  EXPORT ${CMAKE_PROJECT_NAME}Targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )
```
The content of Config.cmake.in:
```
@PACKAGE_INIT@
include( "${CMAKE_CURRENT_LIST_DIR}/MyPackTargets.cmake" )
```

The content of CMakeLists.txt:
```
cmake_minimum_required(VERSION 3.1)

set(CMAKE_PROJECT_NAME "MyPack")
project(${CMAKE_PROJECT_NAME})

set(CMAKE_INSTALL_PREFIX "X:/install")
set(CMAKE_BUILD_TYPE RELEASE)

set(MAJOR_VERSION 2)
set(MINOR_VERSION 1)
set(PATCH_VERSION 6)
set(TWEAK_VERSION 4)
set(VERSION    "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}.${TWEAK_VERSION}"    )


message("VERSION: " ${VERSION})

add_subdirectory(Lib1) 
#add_subdirectory(Lib2) 


install(
  EXPORT ${CMAKE_PROJECT_NAME}Targets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}
  NAMESPACE ${CMAKE_PROJECT_NAME}::
  FILE ${CMAKE_PROJECT_NAME}Targets.cmake 
  )

include(CMakePackageConfigHelpers)
configure_package_config_file( 
  "Config.cmake.in" 
  "${CMAKE_PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}
  PATH_VARS
    CMAKE_INSTALL_LIBDIR
  )

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake
  VERSION ${VERSION}
  COMPATIBILITY SameMajorVersion
  )

### Install Config and ConfigVersion files
install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}"
  )
```

Once you exported and installed and your project, you can call `find_package`. find_package() has the following parameter:  
`find_package(<package> [version] [EXACT] [QUIET] [MODULE] [REQUIRED] [[COMPONENTS] )` finds and loads settings from an external project.

The `QUIET` option disables messages if the package cannot be found.  
The `REQUIRED` option stops processing with an error message if the package cannot be found.  
`<package>_FOUND`  will be set to indicate whether the package was found.  
The `version` argument requests a version with which the package found should be compatible (format is major[.minor[.patch[.tweak]]]). 	


Command `find_package` has two modes: `Module` mode and `Config` mode. 

### Find\<package\>.cmake
Module mode will look for `Find<package>.cmake`in `CMAKE_MODULE_PATH`. This should be used when a project has no CMake support. Usually you create **Find<package>.cmake** for a library and put under **cmake** directory in your porject. Then you should set the set the **CMAKE_MODULE_PATH** pointing to that.
```
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
```

### \<package\>Config.cmake
In config mode it will look for `<name>Config.cmake` or `<lower-case-name>-config.cmake` in `<package>_DIR`. First set the `<package>_DIR`, for example:

```
cmake_minimum_required(VERSION 3.1)
set(MyPack_DIR "$ENV{HOME}/usr/lib/cmake/MyPack")
project(MyPack_user)
set(MyPack_DIR "X:/install/lib/cmake/MyPack")
find_package(MyPack CONFIG REQUIRED )
message("MyPack_FOUND: " ${MyPack_FOUND} )
message("MyPack_VERSION: " ${MyPack_VERSION} )
```

## How to find CMake from arbitrary installed locations

Here I gathered several examples:
### PCL point cloud
```
set(PCL_DIR "$ENV{HOME}/usr/share/pcl-1.8/")
```

### OpenCV
```
set(OpenCV_DIR "$ENV{HOME}/usr/share/OpenCV/")
```

### glog
```
cmake  -Dglog_DIR=~/usr/lib/cmake/glog/
```

## Using pkgconfig (.pc files)
I ususally install my program in my home directorym therefore everything goes into
```
<home>/usr/lib/
<home>/usr/include
<home>/usr/bin
```
So first let's check the pkg-config in terminal:

```
PKG_CONFIG_PATH=~/usr/lib/pkgconfig:${PKG_CONFIG_PATH}
export PKG_CONFIG_PATH
printenv PKG_CONFIG_PATH

pkg-config --cflags flann
pkg-config --libs flann
pkg-config --modversion flann
```

### Flann
```
find_package(PkgConfig)
pkg_check_modules(PC_FLANN flann)
```

### TinyXML2
```
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
```
### yaml-cpp
SET(yaml-cpp_DIR "$ENV{HOME}/usr/share/cmake/yaml-cpp")  
FIND_PACKAGE(yaml-cpp)  
IF(${yaml-cpp_FOUND})  
    MESSAGE("yaml-cpp_FOUND:" ${yaml-cpp_FOUND})  
    MESSAGE("yaml-cpp_VERSION:" ${yaml-cpp_VERSION})  
    ADD_EXECUTABLE(yaml-cpp_example src/third_party_tools/yaml/yaml-cpp/yaml-cpp_example.cpp )  
    TARGET_LINK_LIBRARIES(yaml-cpp_example yaml-cpp)  
ENDIF()

### Google Benchmark
```
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
```
## Setting Important Variables
### C++17 support
```
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

### Position independent code

Globally:
```
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
```
Explicitly turn it ON (or OFF) for a target:

```
set_target_properties(lib1 PROPERTIES POSITION_INDEPENDENT_CODE ON)
```

### Turning warning into errors
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

### Finding Memory leaking, Stack and Heap overflow
```
set(CMAKE_CXX_FLAGS "-fsanitize=address ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "-fno-omit-frame-pointer ${CMAKE_CXX_FLAGS}")
```

### Turning off the warning from unused variables
```
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-private-field")
```

### Setting build type
```
set(CMAKE_BUILD_TYPE DEBUG|RELEASE)
```

## Running a Command in CMake

### At Configure Time
```
find_package(Git QUIET)

 you can use ${CMAKE_COMMAND}, find_package(Git), or find_program to get access to a command to run. 
 Use RESULT_VARIABLE to check the return code and OUTPUT_VARIABLE to get the output.

if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    RESULT_VARIABLE GIT_SUBMOD_RESULT)
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    endif()
endif()
```

### At Build Time
```
find_package(PythonInterp REQUIRED)
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/include/Generated.hpp"
    COMMAND "${PYTHON_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/scripts/GenerateHeader.py" --argument
    DEPENDS some_target)

add_custom_target(generate_header ALL
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/include/Generated.hpp")

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/Generated.hpp DESTINATION include)
```

References:[1](https://gist.github.com/mbinna/), 
[2](https://cliutils.gitlab.io/modern-cmake/),
[3](https://stackoverflow.com/questions/20746936/what-use-is-find-package-if-you-need-to-specify-cmake-module-path-anyway),
[4](https://gitlab.kitware.com/cmake/community/-/wikis/doc/tutorials/How-To-Find-Libraries),
[5](https://github.com/forexample/package-example),
[6](https://gitlab.kitware.com/cmake/community/-/wikis/doc/tutorials/How-to-create-a-ProjectConfig.cmake-file),
[7](https://gitlab.kitware.com/cmake/community/-/wikis/doc/ctest/Scripting-Of-CTest),
[8](https://cmake.org/cmake/help/latest/command/find_package.html),
[9](https://foonathan.net/2016/03/cmake-install/),
[10](https://foonathan.net/2016/07/cmake-dependency-handling/),
[11](https://stackoverflow.com/questions/17511496/how-to-create-a-shared-library-with-cmake),
[12](https://stackoverflow.com/questions/31969547/what-is-the-difference-between-include-directories-and-target-include-directorie),
[13](https://foonathan.net/2016/03/cmake-install/), 
[14](https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/),
[15](https://www.youtube.com/watch?v=rLopVhns4Zs)
