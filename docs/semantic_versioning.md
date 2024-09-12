### Semantic Versioning Overview

Semantic Versioning (SemVer) is a versioning scheme that uses a three-part number format to indicate the state of a software project. It is formatted as:

```
MAJOR.MINOR.PATCH
```

- **MAJOR**: Incremented when there are incompatible API changes.
- **MINOR**: Incremented when functionality is added in a backward-compatible manner.
- **PATCH**: Incremented for backward-compatible bug fixes.

For example:
- `1.0.0` is a new stable release.
- `1.1.0` adds new features but is backward-compatible.
- `1.1.1` fixes bugs but doesn’t add any new features or change the API.

### C++ Example of Semantic Versioning

Let’s assume you have a C++ library. We will use semantic versioning to manage the version of the library.

#### Step 1: Define Versioning Macros in C++
A common approach in C++ projects is to define version numbers as macros in a header file (e.g., `version.hpp`).

```cpp
// version.hpp
#ifndef MYLIB_VERSION_HPP
#define MYLIB_VERSION_HPP

#define MYLIB_VERSION_MAJOR 1
#define MYLIB_VERSION_MINOR 2
#define MYLIB_VERSION_PATCH 3

#define MYLIB_VERSION "1.2.3"

#endif // MYLIB_VERSION_HPP
```

You can use these version numbers throughout your project to check compatibility.

#### Step 2: Usage in Your Code

```cpp
#include "version.hpp"
#include <iostream>

void checkVersion() {
    std::cout << "Library Version: " << MYLIB_VERSION << std::endl;
}

int main() {
    checkVersion();

    // Example: Conditional code based on version
    #if MYLIB_VERSION_MAJOR == 1
        std::cout << "Version 1.x.x detected!" << std::endl;
    #endif
}
```

#### Step 3: Common Approaches to Set the Version

There are several ways to manage and update versions:

1. **Manual Versioning**: You can manually increment version numbers when you release new updates. This is simple but prone to human error if not managed carefully.
   
   Example of a version update:
   - Initial Release: `1.0.0`
   - Adding a new feature: `1.1.0`
   - Bug fix release: `1.1.1`
   - Breaking change: `2.0.0`

2. **Automated Versioning**: Tools like `CMake` or Git-based scripts can help automate versioning based on commits or tags in version control.

   - Example with CMake:
   
   ```cmake
   project(MyProject VERSION 1.2.3)
   
   # Use version numbers in CMakeLists.txt
   message(STATUS "Project version: ${PROJECT_VERSION}")
   ```

   In CMake, you can manage your project version and update it during the build process.

3. **Git Tags**: Many projects use Git tags to manage versioning. When you release a new version, you create a tag like `v1.2.3`. This can be automated using a CI/CD pipeline to increment version numbers based on commits (e.g., `git tag -a v1.2.3 -m "Release 1.2.3"`).

### Common Versioning Scenarios

1. **Initial Release**: Start with version `1.0.0`. This indicates the first public, stable release.

2. **Backward-Compatible Changes**: If you add new features that don’t break compatibility, increment the MINOR version. For example, if you add a new function to your library without breaking existing code, go from `1.0.0` to `1.1.0`.

3. **Bug Fixes**: If you fix a bug but don’t add any new features or break the API, increment the PATCH version. For example, after fixing a bug in `1.1.0`, go to `1.1.1`.

4. **Breaking Changes**: If you make any changes that are not backward-compatible (e.g., you change a function signature), increment the MAJOR version. For example, after introducing breaking changes in version `1.1.0`, you should go to `2.0.0`.

### Example of Version Increments:

- Initial release: `1.0.0`
- Added a new feature (backward-compatible): `1.1.0`
- Fixed a bug: `1.1.1`
- Made breaking changes to the API: `2.0.0`
- New feature added: `2.1.0`
- Another bug fix: `2.1.1`

### Conclusion

Semantic versioning in C++ projects provides clarity in understanding what changes have occurred and how those changes affect the software's backward compatibility. By following SemVer rules, you ensure a consistent, predictable versioning system that helps developers and users understand the impact of each release.
