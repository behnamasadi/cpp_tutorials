# Design a Reusable C++ Library

A reusable library is one that other people pick up, integrate, and don't regret. Most "reusability" problems aren't about features — they're about packaging, headers, ABI, and dependencies. This page is the checklist.

- [1. Decide What the Library *Is*](#1-decide-what-the-library-is)
- [2. Public Headers and the Include Surface](#2-public-headers-and-the-include-surface)
- [3. Namespacing and Symbol Hygiene](#3-namespacing-and-symbol-hygiene)
- [4. Header-Only vs Compiled](#4-header-only-vs-compiled)
- [5. CMake Packaging](#5-cmake-packaging)
- [6. Versioning and ABI](#6-versioning-and-abi)
- [7. Dependencies — As Few As Possible](#7-dependencies--as-few-as-possible)
- [8. Error Reporting](#8-error-reporting)
- [9. Documentation and Examples](#9-documentation-and-examples)
- [10. Reusability Checklist](#10-reusability-checklist)

---

# 1. Decide What the Library *Is*

Before any code, answer:

- **Scope.** One thing, well. "JSON parser." Not "JSON parser plus HTTP client plus logging."
- **Audience.** Application developers? Other library authors? Embedded? Each implies different defaults (allocator support, exception use, RTTI).
- **Consumption model.** Header-only? Static? Shared? `find_package`-able? `add_subdirectory`-able? Conan/vcpkg?
- **C++ standard.** Pick the lowest version that does the job. Bumping later is easy; lowering is impossible.
- **Stability promise.** SemVer with a real ABI commitment, or "headers only, recompile on every release"?

Write these down in a `DESIGN.md`. They will drive every later decision.

# 2. Public Headers and the Include Surface

Public headers are your contract. Treat them surgically:

- Put public headers under `include/<libname>/...` and never include from `src/...` or relative paths.
- Forward-declare aggressively in public headers. Pull in heavy STL headers only when the type is actually needed by value.
- Avoid `using namespace` and macros at file scope. Both leak into every consumer.
- Don't expose implementation types — use [PIMPL](../PIMPL.md) or opaque handles for class members that change often.
- Make sure each header compiles standalone (`#include`-it-first test).

```
mylib/
├── include/mylib/                  # public, installable
│   ├── core.hpp
│   └── detail/                     # public-but-private; don't promise stability
├── src/                            # implementation, never installed
└── test/
```

The `detail/` convention signals "you can see this, but if you depend on it your code may break."

# 3. Namespacing and Symbol Hygiene

- Top-level namespace = library name. No exceptions.
- Use an inner `detail` namespace for private helpers visible from headers.
- Use [inline namespaces](https://en.cppreference.com/w/cpp/language/namespace) for ABI versioning (`inline namespace v1 { ... }`).
- For shared libraries, mark only the public symbols visible (`-fvisibility=hidden` + an export macro).

```cpp
#if defined(_WIN32)
  #ifdef MYLIB_BUILDING
    #define MYLIB_API __declspec(dllexport)
  #else
    #define MYLIB_API __declspec(dllimport)
  #endif
#else
  #define MYLIB_API __attribute__((visibility("default")))
#endif
```

# 4. Header-Only vs Compiled

| | Header-only | Static lib | Shared lib |
|---|---|---|---|
| Build complexity for consumer | None | Low | Medium |
| Compile time hit | High (rebuilt everywhere) | Once | Once |
| ABI stability matters? | No (consumer recompiles) | No | **Yes** |
| Templates everywhere | Easy | Possible (extern template) | Painful |
| Plugin-style hot-swap | No | No | Yes |

Default: **static** unless you have a reason. Header-only for small utility libraries (`expected`, `span`-like). Shared for plugin systems and where binary distribution matters.

# 5. CMake Packaging

A modern, reusable library exposes one (or a few) IMPORTED targets. Consumers should write:

```cmake
find_package(mylib 2.0 REQUIRED)
target_link_libraries(myapp PRIVATE mylib::mylib)
```

Skeleton:

```cmake
cmake_minimum_required(VERSION 3.20)
project(mylib VERSION 1.2.0 LANGUAGES CXX)

add_library(mylib)
add_library(mylib::mylib ALIAS mylib)

target_sources(mylib PRIVATE src/foo.cpp)
target_include_directories(mylib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_compile_features(mylib PUBLIC cxx_std_20)

include(GNUInstallDirs)
install(TARGETS mylib EXPORT mylibTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT mylibTargets
        FILE mylibTargets.cmake
        NAMESPACE mylib::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mylib)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/mylibConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)
```

Things to get right:
- Use `BUILD_INTERFACE` / `INSTALL_INTERFACE` so headers work both in-tree and after install.
- Provide a `mylib::mylib` alias so consumers can write the same code in both modes.
- Generate and ship a `mylibConfigVersion.cmake` so `find_package(mylib 2.0)` works.

# 6. Versioning and ABI

Use [semantic versioning](../semantic_versioning.md): MAJOR.MINOR.PATCH.

| Change | Bump |
|---|---|
| Bug fix, no API change | PATCH |
| New API, old code still compiles | MINOR |
| Removed/renamed/changed signature | MAJOR |
| Changed layout of public class, vtable, exception type | MAJOR (ABI break) |

For shared libraries you also need an SONAME tied to the major version. See [API and ABI Design](api_abi_design.md) for the full rulebook.

# 7. Dependencies — As Few As Possible

Every dependency is a constraint you push onto your consumers (compiler version, standard, build system, license). Rules of thumb:

- **The standard library is free**, except `<filesystem>` and `<regex>` which historically had link-time gotchas.
- **Boost** is fine for app code, painful as a library dep — splits across versions, large download, header-time cost.
- **Other libraries** should be considered carefully; if you only need one function, copy it (with attribution) instead of dragging a transitive dep.
- For deps you really need, expose them only privately (`target_link_libraries(mylib PRIVATE foo)`) so consumers don't inherit the include path.

# 8. Error Reporting

A library cannot dictate how its callers handle errors. Pick a *single* mechanism, document it, and stick with it:

- **Exceptions** — natural for C++, costs nothing on the success path, painful in `-fno-exceptions` consumers.
- **Error codes** — small, predictable; `std::error_code` is the standard idiom.
- **`std::expected<T,E>`** (C++23) — the modern compromise; explicit, no exceptions, value-semantic.

Document which functions can fail and how. Never `std::terminate` from library code unless the precondition was clearly violated.

# 9. Documentation and Examples

A library nobody can figure out is not reusable.

- A `README.md` with: 30-second pitch, install snippet, smallest meaningful example, link to full docs.
- Doxygen on every public symbol — at minimum a one-line description and the failure mode.
- An `examples/` folder that builds with the library. Treat it as a test of your API ergonomics.
- Changelog (`CHANGELOG.md` in [Keep a Changelog](https://keepachangelog.com) format).

# 10. Reusability Checklist

- [ ] Public headers under `include/<libname>/`, each compiles standalone.
- [ ] All public symbols inside `<libname>::` namespace; private helpers in `<libname>::detail`.
- [ ] `mylib::mylib` CMake target works for both `find_package` and `add_subdirectory`.
- [ ] No `using namespace` in public headers.
- [ ] No transitive dependencies leaked through public headers unless intended.
- [ ] Versioned via SemVer; ABI commitment documented.
- [ ] Single, documented error-reporting mechanism.
- [ ] At least one buildable example.
- [ ] CI builds on the matrix of compilers/standards you claim to support.
- [ ] License file at the repo root.

# References

- [API and ABI Design](api_abi_design.md)
- [SOLID Principles](solid_principles.md)
- [Modern CMake Examples](https://cliutils.gitlab.io/modern-cmake/)
- *Large-Scale C++ Software Design*, John Lakos.
