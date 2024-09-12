To properly manage versioning in C++ projects using CMake and Git, leveraging Git tags for Semantic Versioning (SemVer), you can follow the approach below. This method ensures that:

1. **On tagged commits:** The exact tag is used as the version, e.g., `2.3.1`.
2. **On commits between tags:** The version is based on the most recent tag with additional commit information, e.g., `2.3.5-<commit-hash>`.

### Steps to Implement

#### 1. **Create a script to extract the version from Git**

Create a script (e.g., `get_version.sh`) to extract the version information from Git tags:

```bash
#!/bin/bash

# Get the most recent tag
TAG=$(git describe --tags --abbrev=0)

# Get the number of commits since the last tag
COMMITS_SINCE_TAG=$(git rev-list ${TAG}..HEAD --count)

# Get the current commit hash (shortened)
COMMIT_HASH=$(git rev-parse --short HEAD)

# Determine the version string
if [ "$COMMITS_SINCE_TAG" -eq "0" ]; then
    # If this is exactly the tagged commit, just return the tag
    echo "${TAG}"
else
    # Otherwise, return the tag + number of commits since the tag + the commit hash
    echo "${TAG}-${COMMITS_SINCE_TAG}.g${COMMIT_HASH}"
fi
```

Make the script executable:

```bash
chmod +x get_version.sh
```

#### 2. **Modify CMakeLists.txt to use the version**

In your `CMakeLists.txt`, modify it to use the version extracted by the script:

```cmake
cmake_minimum_required(VERSION 3.12)
project(MyProject VERSION 0.0.0) # Default version

# Define a custom command to extract the version from Git
find_package(Git REQUIRED)
execute_process(
    COMMAND ${CMAKE_SOURCE_DIR}/get_version.sh
    OUTPUT_VARIABLE GIT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Parse the version string
string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)" SEMVER_MATCHES "${GIT_VERSION}")
string(REGEX MATCH "^[0-9]+\\.[0-9]+\\.[0-9]+" SEMVER "${GIT_VERSION}")

if(SEMVER_MATCHES)
    message(STATUS "Setting version based on Git: ${SEMVER}")
    project(MyProject VERSION ${SEMVER})
endif()

# Use the full version (including commit hash) as the project's version
set(PROJECT_VERSION_FULL "${GIT_VERSION}")
message(STATUS "Full project version: ${PROJECT_VERSION_FULL}")

# Configure version.h
configure_file(
    ${CMAKE_SOURCE_DIR}/version.h.in
    ${CMAKE_BINARY_DIR}/version.h
    @ONLY
)

# Rest of your CMake configuration...
```

#### 3. **Create a `version.h.in` template**

Create a `version.h.in` file in your project's root directory:

```cpp
#pragma once

#define PROJECT_VERSION_MAJOR @MyProject_VERSION_MAJOR@
#define PROJECT_VERSION_MINOR @MyProject_VERSION_MINOR@
#define PROJECT_VERSION_PATCH @MyProject_VERSION_PATCH@
#define PROJECT_VERSION_FULL "@PROJECT_VERSION_FULL@"
```

This header will be configured by CMake to provide version information to your C++ code.

#### 4. **Include the `version.h` in your code**

In your C++ code, include the generated `version.h` to access the version information:

```cpp
#include "version.h"
#include <iostream>

int main() {
    std::cout << "Version: " << PROJECT_VERSION_MAJOR << "."
              << PROJECT_VERSION_MINOR << "."
              << PROJECT_VERSION_PATCH << std::endl;
    std::cout << "Full Version: " << PROJECT_VERSION_FULL << std::endl;
    return 0;
}
```

### Alternative Approach: Using `git describe`

If you prefer a more Git-centric approach without the script, you can use `git describe` directly in CMake:

```cmake
execute_process(
    COMMAND git describe --tags --long --always
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_DESCRIBE_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Extract the version components
string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)" SEMVER_MATCHES "${GIT_DESCRIBE_VERSION}")

if(SEMVER_MATCHES)
    message(STATUS "Setting version based on Git: ${GIT_DESCRIBE_VERSION}")
    set(PROJECT_VERSION_FULL "${GIT_DESCRIBE_VERSION}")
endif()

configure_file(
    ${CMAKE_SOURCE_DIR}/version.h.in
    ${CMAKE_BINARY_DIR}/version.h
    @ONLY
)
```

This approach gives you a flexible and automated way to handle semantic versioning in your C++ projects using CMake and Git. The key is to ensure that your build process consistently reflects the state of your repository, making it easy to track which version corresponds to which commit.


Yes, you can avoid using an external script like `get_version.sh` by directly integrating the logic into a CMake file. Here's how you can achieve it:

### 1. **Create a separate CMake file for versioning**

Let's create a separate `version.cmake` file with the logic to get the version from Git and include it in your main `CMakeLists.txt`.

#### `version.cmake`

```cmake
# This file will fetch the version from git tags and use it in CMake.

# Check if we have Git available
find_package(Git REQUIRED)

# Get the latest tag
execute_process(
    COMMAND git describe --tags --abbrev=0
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_LATEST_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the number of commits since the last tag
execute_process(
    COMMAND git rev-list ${GIT_LATEST_TAG}..HEAD --count
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMITS_SINCE_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the current commit hash
execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Determine the version string
if("${GIT_COMMITS_SINCE_TAG}" STREQUAL "0")
    # If this is exactly the tagged commit, just return the tag
    set(GIT_VERSION "${GIT_LATEST_TAG}")
else()
    # Otherwise, append the commit count and hash
    set(GIT_VERSION "${GIT_LATEST_TAG}-${GIT_COMMITS_SINCE_TAG}.g${GIT_COMMIT_HASH}")
endif()

# Parse the version components (major, minor, patch)
string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)" SEMVER_MATCHES "${GIT_LATEST_TAG}")
if(SEMVER_MATCHES)
    string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" PROJECT_VERSION_MAJOR "${GIT_LATEST_TAG}")
    string(REGEX REPLACE "^[0-9]+\\.([0-9]+)\\..*" "\\1" PROJECT_VERSION_MINOR "${GIT_LATEST_TAG}")
    string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" PROJECT_VERSION_PATCH "${GIT_LATEST_TAG}")
endif()

# Set the full version
set(PROJECT_VERSION_FULL "${GIT_VERSION}")
message(STATUS "Project version: ${PROJECT_VERSION_FULL}")

# Configure the version.h header file
configure_file(
    ${CMAKE_SOURCE_DIR}/version.h.in
    ${CMAKE_BINARY_DIR}/version.h
    @ONLY
)
```

### 2. **Include the versioning CMake in your main `CMakeLists.txt`**

In your main `CMakeLists.txt`, include the newly created `version.cmake` file:

#### `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.12)
project(MyProject VERSION 0.0.0) # Default version in case Git fails

# Include the versioning CMake script
include(${CMAKE_SOURCE_DIR}/version.cmake)

# Rest of your CMake configuration...
```

### 3. **`version.h.in` file**

Make sure you also have the `version.h.in` file to configure the version in your code:

#### `version.h.in`

```cpp
#pragma once

#define PROJECT_VERSION_MAJOR @PROJECT_VERSION_MAJOR@
#define PROJECT_VERSION_MINOR @PROJECT_VERSION_MINOR@
#define PROJECT_VERSION_PATCH @PROJECT_VERSION_PATCH@
#define PROJECT_VERSION_FULL "@PROJECT_VERSION_FULL@"
```

### 4. **How it works:**

- **`version.cmake`** fetches the latest Git tag, counts commits since that tag, and retrieves the current commit hash. It then sets the `PROJECT_VERSION_FULL` and other version variables.
- The **`version.h.in`** file gets configured by CMake to produce a `version.h` file containing version information.
- In your **`CMakeLists.txt`**, the `version.cmake` is included to handle all the versioning logic.

### 5. **Using the version in your C++ code**

In your C++ code, you can now include the generated `version.h` and use the version information:

```cpp
#include "version.h"
#include <iostream>

int main() {
    std::cout << "Version: " << PROJECT_VERSION_MAJOR << "."
              << PROJECT_VERSION_MINOR << "."
              << PROJECT_VERSION_PATCH << std::endl;
    std::cout << "Full Version: " << PROJECT_VERSION_FULL << std::endl;
    return 0;
}
```


Now, your versioning logic is completely integrated into CMake without relying on an external script, and you can include the `version.cmake` file in your main CMake setup. This makes it easier to manage and ensures that your project uses Git for versioning automatically.
