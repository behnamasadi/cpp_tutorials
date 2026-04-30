# API and ABI Design for C++ Libraries

API is what consumers *write* against (function signatures, classes, contracts). ABI is what consumers *link and load* against (mangled names, struct layouts, vtable shapes, calling conventions). They are different stability promises with different consequences.

- [1. API vs ABI — What Actually Breaks](#1-api-vs-abi--what-actually-breaks)
- [2. Symbol Visibility](#2-symbol-visibility)
- [3. PIMPL for ABI Stability](#3-pimpl-for-abi-stability)
- [4. Inline Namespaces for Versioning](#4-inline-namespaces-for-versioning)
- [5. The C ABI as a Stable Boundary](#5-the-c-abi-as-a-stable-boundary)
- [6. STL Types in Public Interfaces](#6-stl-types-in-public-interfaces)
- [7. Deprecation and Removal](#7-deprecation-and-removal)
- [8. Detecting ABI Breaks](#8-detecting-abi-breaks)
- [9. Quick Reference](#9-quick-reference)

---

# 1. API vs ABI — What Actually Breaks

| Change | API break? | ABI break? |
|---|---|---|
| Add a new function | No | No |
| Add a non-virtual member function | No | No |
| Add a virtual function (anywhere in the hierarchy) | No | **Yes** |
| Add a non-static data member | No | **Yes** |
| Reorder data members | No | **Yes** |
| Reorder virtual functions | No | **Yes** |
| Change a function signature | **Yes** | **Yes** |
| Change return type even subtly (`int`→`long`) | Maybe | **Yes** |
| Change default arg value | Maybe (silently) | No |
| Change inline function body in header | No | **Yes** if mixed-version inlining |
| Change `enum` underlying type | **Yes** | **Yes** |
| Add value to an `enum class` | No | No |
| Change template's instantiations exposed | No | **Yes** if extern template |

Rule of thumb: **anything that changes class size, layout, or vtable layout is an ABI break.** ABI-broken libraries link successfully and crash at runtime — that's the point of caring.

# 2. Symbol Visibility

By default on Unix, every non-static symbol in a shared library is exported. That:
- Bloats the dynamic symbol table (slow load).
- Makes accidental ABI dependencies trivial.
- Lets two libraries with name collisions clobber each other.

Set the default to hidden and opt symbols in:

```cmake
set_target_properties(mylib PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)
```

Then mark public ones:

```cpp
// mylib_export.hpp
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

CMake's `GenerateExportHeader` automates this.

# 3. PIMPL for ABI Stability

The cleanest way to keep a class's ABI stable while letting its implementation evolve:

```cpp
// public header — never changes when impl grows
#include <memory>
#include <string>

class Connection {
public:
    Connection();
    ~Connection();

    void send(const std::string& data);
private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
```

The size and layout of `Connection` are now exactly `sizeof(unique_ptr)` regardless of how `Impl` evolves. Cost: an extra heap allocation, indirection on every call. See [PIMPL](../PIMPL.md) for full details.

# 4. Inline Namespaces for Versioning

Inline namespaces let you ship multiple ABI versions side-by-side and pick one as the default:

```cpp
namespace mylib {
    inline namespace v2 {
        class Connection { int new_layout; };
    }
    namespace v1 {
        class Connection { int old_layout; };
    }
}

int main() {
    mylib::Connection c2;       // resolves to v2::Connection
    mylib::v1::Connection c1;   // old type still usable
}
```

Now `mylib::Connection` resolves to `mylib::v2::Connection`, but `mylib::v1::Connection` is still a valid type for old code. The mangled names differ, so the linker keeps them distinct. libstdc++'s dual ABI for `std::string` works this way (`__cxx11`).

# 5. The C ABI as a Stable Boundary

The C ABI is the most stable thing in computing. If your library must support consumers built with different compilers, different standard library versions, or other languages — give them a C interface:

```cpp
#include <stddef.h>

extern "C" {
    typedef struct mylib_conn mylib_conn;        // opaque handle
    mylib_conn* mylib_conn_new(void);
    void        mylib_conn_free(mylib_conn*);
    int         mylib_conn_send(mylib_conn*, const char* data, size_t len);
}
```

You then ship a thin C++ header on top for ergonomic use. Almost every cross-language library (`sqlite`, `libcurl`, `zlib`) does this.

# 6. STL Types in Public Interfaces

This is where most ABI breaks actually originate.

- **`std::string`, `std::vector` across DLL boundaries on Windows.** Different MSVC versions or `_ITERATOR_DEBUG_LEVEL` settings give incompatible layouts. Don't.
- **`std::shared_ptr` across libraries.** The control block must be allocated/freed on the same heap; incompatible if both sides use different runtimes.
- **`std::function` and lambdas in headers.** Inlined captures means changes in one TU silently disagree.
- **Standard library upgrades** can change layouts (the libstdc++ `std::list::size()` saga).

For library boundaries, prefer:
- POD-like value types you define yourself.
- C-friendly views: `const char*` + length, or `std::span`-like custom struct.
- Opaque handles for non-trivial state.

If you do expose STL types, document the *exact* compiler+stdlib combination supported.

# 7. Deprecation and Removal

Never delete in one release. The polite cycle:

```cpp
[[deprecated("use new_send() instead, removal in 3.0")]]
void send(const char* data);

void new_send(const char* data);

int main() {
    send("hello");      // compiles, but emits a deprecation warning
    new_send("hello");  // preferred replacement
}
```

- **Minor release N**: add `new_send`, mark `send` deprecated.
- **Minor release N+k**: keep both, document migration.
- **Major release N+1**: remove `send`.

Provide migration scripts or `clang-tidy` checks if the API change is mechanical.

# 8. Detecting ABI Breaks

Don't rely on humans. Automate:

- **`abi-compliance-checker`** / **`abidiff`** (libabigail) — diff two `.so` files, report incompatible changes.
- **API fuzz tests** — a small program built against the *old* headers, run against the *new* `.so`. If it crashes or produces wrong results, you broke ABI.
- **CI gate**: on every PR, build current, build at last released tag, diff. Fail the build if ABI changed without a major-version bump.

For header-only libraries, you don't have ABI but you do have *API* breaks — track them with [API-checker tools](https://github.com/include-what-you-use/include-what-you-use) or a typed compatibility test suite.

# 9. Quick Reference

**To preserve ABI:**
- Don't add/remove/reorder data members.
- Don't add/remove/reorder virtual functions.
- Don't change function signatures (add new overloads instead).
- Don't change inline functions exposed in headers.
- Don't change template signatures of instantiated templates.

**To make ABI evolution easy:**
- PIMPL on classes that will grow.
- Inline namespaces for opt-in version bumps.
- Hidden visibility by default.
- A C facade for the truly stable boundary.

**To ship safely:**
- SemVer with major bumps for ABI breaks.
- ABI diff in CI.
- Deprecation period of at least one minor cycle.

# References

- [Design a Reusable Library](design_a_reusable_library.md)
- [PIMPL](../PIMPL.md)
- [libabigail](https://sourceware.org/libabigail/)
- *KDE/Qt ABI Compatibility Guidelines*: https://community.kde.org/Policies/Binary_Compatibility_Issues_With_C++
- [Itanium C++ ABI](https://itanium-cxx-abi.github.io/cxx-abi/abi.html)
