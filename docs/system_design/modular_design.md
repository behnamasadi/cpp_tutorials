# Modular Design with C++20 Modules and Build-System Architecture

C++ has historically not had a real module system. The textual `#include` model — paste headers everywhere, parse them again per TU — is the root cause of long build times and most ODR-related bugs. C++20 modules finally fix this; this page covers both the modern story and the pragmatic mixed reality.

- [1. Why Modules Matter](#1-why-modules-matter)
- [2. Module Basics](#2-module-basics)
- [3. Module Partitions](#3-module-partitions)
- [4. Header Units — Migration Bridge](#4-header-units--migration-bridge)
- [5. Build-System Architecture: Targets, Layers, Boundaries](#5-build-system-architecture-targets-layers-boundaries)
- [6. CMake and Modules](#6-cmake-and-modules)
- [7. Toolchain Reality (2024+)](#7-toolchain-reality-2024)
- [8. Compile-Time Strategy Without Modules](#8-compile-time-strategy-without-modules)
- [9. Quick Reference](#9-quick-reference)

---

# 1. Why Modules Matter

The pre-modules world:
- Every TU re-parses every transitively-included header.
- Macros leak across includes, polluting consumers.
- Order of `#include` can change semantics.
- Templates in headers are recompiled in every consumer.
- Build times scale superlinearly with dependency depth.

What modules give you:
- **Each module is parsed once.** Imports replay a precompiled binary representation.
- **No macro leakage.** A module's macros do not escape unless explicitly exported.
- **Order of imports doesn't matter.** Imports are not transitive by default.
- **Hidden symbols stay hidden.** The compiler enforces `export`/non-export.
- **Faster builds** — usually 2–5× on header-heavy code.

# 2. Module Basics

```cpp
// math.cppm — module interface unit
export module math;

export int add(int a, int b) { return a + b; }
export int sub(int a, int b) { return a - b; }

// not exported — visible only inside the module
int internal_helper() { return 42; }
```

```cpp
// main.cpp
import math;

int main() { return add(1, 2); }
```

Compile with (Clang):
```bash
clang++ -std=c++20 --precompile math.cppm -o math.pcm
clang++ -std=c++20 -fprebuilt-module-path=. main.cpp math.pcm -o app
```

GCC and MSVC have analogous flags. CMake 3.28+ has direct support; see §6.

Two TU kinds:
- **Module interface unit** (`export module foo;`) — declares the module and its exports. Exactly one per module.
- **Module implementation unit** (`module foo;`) — implements the interface. May be many.

# 3. Module Partitions

Big modules can be split into partitions:

```cpp
// math-arith.cppm
export module math:arith;
export int add(int, int);
export int sub(int, int);

// math-trig.cppm
export module math:trig;
export double sin(double);

// math.cppm
export module math;
export import :arith;     // re-export the partition
export import :trig;
```

Partitions look to the outside like one module (`import math;`). Internally they're separately compiled units. Use partitions to break up large modules without exposing the split to consumers.

# 4. Header Units — Migration Bridge

You can `import` a legacy header as a module, getting most of the perf win without rewriting:

```cpp
import <vector>;       // standard header as module — fast
import "legacy.h";     // your own header
```

The compiler precompiles the header to a binary module interface. Macros from header units do leak (they're still header semantics) — but parsing happens once. This is the practical migration path: switch consumers to `import`, leave the headers themselves alone, then move headers one at a time to real modules.

# 5. Build-System Architecture: Targets, Layers, Boundaries

Modules don't fix bad architecture. The principles are the same regardless:

**Layered architecture.** Each layer depends only on layers below it. Drawn as a DAG; no cycles.

```
[ ui ] ──▶ [ application ] ──▶ [ domain ] ──▶ [ infrastructure ]
```

**One build target per logical component.** Each gets its own include path, its own dependencies. CMake `add_library(domain STATIC ...)`. Tests at component level.

**Public vs private dependencies.** In CMake:
- `target_link_libraries(mylib PUBLIC foo)` — `foo` is part of mylib's interface; consumers see it.
- `... PRIVATE foo` — used internally only.
- `... INTERFACE foo` — header-only / pure transitive.

Misuse causes leaky abstractions. A core domain library should not have `PUBLIC` dependencies on infrastructure libraries.

**Convention for big projects:**
```
project/
├── components/
│   ├── domain/
│   │   ├── include/proj/domain/    # public
│   │   ├── src/                    # private
│   │   ├── test/
│   │   └── CMakeLists.txt
│   ├── infra/...
│   └── ui/...
├── apps/
│   └── server/CMakeLists.txt
└── CMakeLists.txt                  # ties it all together
```

# 6. CMake and Modules

CMake 3.28 added first-class C++20 modules support:

```cmake
cmake_minimum_required(VERSION 3.28)
project(modules_demo CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

add_library(math)
target_sources(math
    PUBLIC FILE_SET CXX_MODULES FILES
        math.cppm
        math-arith.cppm
        math-trig.cppm
)

add_executable(app main.cpp)
target_link_libraries(app PRIVATE math)
```

`FILE_SET CXX_MODULES` is the new construct. CMake handles dependency scanning, ordering, and BMI (binary module interface) caching across the build.

You need a recent toolchain — `clang-16+`, `gcc-14+`, MSVC 19.40+, and Ninja 1.11+ (for `dyndep`).

# 7. Toolchain Reality (2024+)

Status as of mid-2024:

- **Clang/libc++**: best-supported. `import std;` works. Module-aware `clangd` for IDE.
- **GCC/libstdc++**: full module support landed in GCC 14. Some standard headers as modules still incomplete.
- **MSVC/STL**: well-supported, including `import std;`. Visual Studio integrates cleanly.
- **CMake**: 3.28+ for direct support; 3.30 fixed many corners.
- **Ninja**: required for module dependency scanning (Make-based builds don't work cleanly).

Mixed projects:
- Modules and headers coexist. A module can `#include` legacy headers; a legacy TU can `import` modules.
- Don't define macros in module-purview that you want consumers to see — use a `module : private;` section or convert to `constexpr` constants.
- Be careful with `export import std;` if other consumers also have their own STL bindings.

# 8. Compile-Time Strategy Without Modules

If you can't use modules yet, the long-standing tools:

- **Forward declarations** wherever possible.
- **PIMPL** for big classes — header sees only `unique_ptr<Impl>`.
- **Precompiled headers** (`#pragma once` + `target_precompile_headers`) — biggest non-module speedup.
- **Unity builds** (CMake `CMAKE_UNITY_BUILD`) — concatenate TUs to amortize header parsing. Trades memory for time.
- **`include-what-you-use`** — IWYU clean up reduces transitive includes.
- **`ccache` / `sccache`** — cache compiler output across builds.

Combined, these cut typical C++ build times in half before you change a line of architecture.

# 9. Quick Reference

| Want | Do |
|---|---|
| Fastest builds, fresh codebase | Modules from day one, `import std;`, CMake 3.28 |
| Faster builds, mature codebase | PCH first, then header units, then modules incrementally |
| Hide internal symbols | Don't `export`; module privacy is enforced |
| Split a big module | Partitions (`module foo:bar`) |
| Keep a stable consumer interface | One module per public component, internal partitions for splits |
| Detect dependency cycles early | Per-component CMake targets, no `PUBLIC` cycles |

# References

- [API and ABI Design](api_abi_design.md)
- [Design a Reusable Library](design_a_reusable_library.md)
- [Modules](../cpp_modules.md)
- [PIMPL](../PIMPL.md)
- [CMake C++ Modules support](https://www.kitware.com/import-cmake-c20-modules/)
- *Working Draft* / [P1103, A Module System for C++](https://wg21.link/p1103)
- *Large-Scale C++ Software Design*, John Lakos.
