# vcpkg
First add it as submodule to your peoject:

`git submodule add https://github.com/microsoft/vcpkg`


## Building Dependecies with Your CMakeLists.txt
Now add the following to your CMakeLists

```
cmake_minimum_required(VERSION 3.16.3)

set(CMAKE_BUILD_TYPE Debug)

if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE PATH "toolchain file")
endif()

message("toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
project(your-project-name)

find_package(vtk  9.2)
```

After that, create the folloiwng file  `vcpkg.json` next to your `CMakeLists.txt`: 

```
{
  "name": "your-project-name",
  "version-string": "1.1.0",
  "dependencies": [
    "vtk"
  ]
}
```
Now you can run:
```
cmake -B [build directory] -S . -DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake
```


## Building and Installing Dependecies 
Alternatively you can install everything globally/ locally

Then run the following script on windows:

`.\vcpkg\bootstrap-vcpkg.bat`

on the bash:

`./vcpkg/bootstrap-vcpkg.sh`

Now you can install dependencies:

`
.\vcpkg.exe install opencv4:x64-windows
.\vcpkg.exe install curl[openssl]:x64-windows
.\vcpkg.exe  upgrade curl --no-dry-run
.\vcpkg.exe  upgrade remove curl:x64-windows --recurse
`
 If you dont want to type the architecture i.e. `x64-windows`, create a new environmental variable:
```  
Default x64/x32:
Name:  VCPKG_DEFAULT_TRIPLET
Value: x64-windows
```

You can create a `vcpkg.json` next to your cmake file:


All availabe packages listed [here](https://vcpkg.io/en/packages.html):

and localy they are  stored at:

`vcpkg\ports\ffmpeg\portfile.cmake`

Refs: [1](https://vcpkg.io/en/getting-started.html)

