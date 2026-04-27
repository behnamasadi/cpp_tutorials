# clang-tidy

`clang-tidy` is a static analyzer / linter for C and C++ that ships with LLVM. It reads your code without running it and flags problems — many of which it can also auto-fix.

It is **not** the same as `clang-format`:

| | clang-format | clang-tidy |
|---|---|---|
| Touches | Whitespace, line breaks, indentation | Code logic, names, idioms |
| Speed | Instant | Slower (semantic analysis) |
| Needs build info? | No | Yes (`compile_commands.json`) |
| Auto-fix | Always | Often (with `-fix`) |

## Installation

**Ubuntu / Debian**
```
sudo apt install clang-tidy
```

**macOS**
```
brew install llvm
```

**Windows**
```
choco install llvm
```
or download LLVM from https://releases.llvm.org

Verify:
```
clang-tidy --version
```

## What it catches

Checks are grouped into categories. A few examples from each:

| Category | Check | What it catches |
|---|---|---|
| `bugprone-*` | `bugprone-use-after-move` | Using a variable after `std::move` |
| `modernize-*` | `modernize-use-nullptr` | Replaces `NULL` / `0` with `nullptr` |
| `modernize-*` | `modernize-use-auto` | `std::vector<int>::iterator it` → `auto it` |
| `modernize-*` | `modernize-loop-convert` | Index `for` loops → range-for |
| `performance-*` | `performance-unnecessary-copy-initialization` | `auto x = vec[0];` when `const auto&` suffices |
| `performance-*` | `performance-for-range-copy` | `for (auto x : container)` should be `const auto&` |
| `readability-*` | `readability-identifier-naming` | Enforce `snake_case` / `CamelCase` |
| `cppcoreguidelines-*` | `cppcoreguidelines-pro-type-member-init` | Uninitialized members |
| `cert-*` | `cert-err58-cpp` | CERT secure coding violations |
| `clang-analyzer-*` | path-sensitive analysis | Null derefs, leaks, use-after-free |

Full list: https://clang.llvm.org/extra/clang-tidy/checks/list.html

## Generate `compile_commands.json`

clang-tidy needs to know each file's include paths, defines, and C++ standard. CMake produces this automatically:

```cmake
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

After configuring, the file appears in your build directory. Symlink it to the project root so clang-tidy finds it:

```
ln -s build/compile_commands.json compile_commands.json
```

## Running clang-tidy

**One file**
```
clang-tidy src/main.cpp
```

**Pick specific checks**
```
clang-tidy --checks='-*,modernize-*,performance-*,bugprone-*' src/main.cpp
```

**Apply suggested fixes**
```
clang-tidy -fix src/main.cpp
```

**Run on every file in a directory**
```
find src -name '*.cpp' | xargs clang-tidy
```

## `.clang-tidy` config file

Place a `.clang-tidy` file at the project root to make checks consistent across the team:

```yaml
Checks: >
  -*,
  bugprone-*,
  modernize-*,
  performance-*,
  readability-*,
  cppcoreguidelines-*,
  -modernize-use-trailing-return-type,
  -readability-identifier-length

WarningsAsErrors: ''
HeaderFilterRegex: '.*'
FormatStyle: file
```

- `Checks` — comma-separated list. `-*` disables all, then re-enables groups. Prefix a check with `-` to silence it.
- `WarningsAsErrors` — set to the same expression to fail builds on warnings.
- `HeaderFilterRegex` — which headers to analyze (default skips system headers).
- `FormatStyle: file` — apply `clang-format` to fix-ups using your `.clang-format`.

## Run via CMake on every build

Add to `CMakeLists.txt`:

```cmake
set(CMAKE_CXX_CLANG_TIDY clang-tidy)
```

Now every translation unit is linted as it compiles. Combine with the detection block already in this project's CMake:

```cmake
find_program(CLANG_TIDY_EXECUTABLE NAMES clang-tidy)
if(CLANG_TIDY_EXECUTABLE)
    set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXECUTABLE})
endif()
```

## VS Code integration

The **clangd** extension (`llvm-vs-code-extensions.vscode-clangd`) runs clang-tidy diagnostics inline as you type. With clangd installed:

```json
{
  "clangd.arguments": [
    "--clang-tidy",
    "--compile-commands-dir=build"
  ]
}
```

The C/C++ extension (`ms-vscode.cpptools`) also has built-in clang-tidy support — enable `C_Cpp.codeAnalysis.clangTidy.enabled` in settings.

## CI integration

Run on changed files only in pull requests:

```
git diff --name-only origin/master...HEAD | grep -E '\.(cpp|h|hpp)$' | xargs clang-tidy -p build
```

Fail the job if any warnings appear:

```
clang-tidy --warnings-as-errors='*' -p build src/main.cpp
```

## Common gotchas

- **"unable to find compile_commands.json"** — pass `-p path/to/build` or symlink it to project root.
- **Slow runs** — clang-tidy reparses each TU; on large projects use `run-clang-tidy.py` (ships with LLVM) for parallel execution.
- **Fix conflicts** — when two checks suggest overlapping edits, run clang-tidy multiple times until output is stable.
- **Generated / third-party code** — exclude with `HeaderFilterRegex` or wrap in `// NOLINT` / `// NOLINTNEXTLINE` comments.

## Suppressing a single warning

```cpp
int x; // NOLINT(cppcoreguidelines-init-variables)

// NOLINTNEXTLINE(modernize-use-nullptr)
foo(0);
```

## References

- Official docs: https://clang.llvm.org/extra/clang-tidy/
- Check list: https://clang.llvm.org/extra/clang-tidy/checks/list.html
- `run-clang-tidy.py`: https://github.com/llvm/llvm-project/blob/main/clang-tools-extra/clang-tidy/tool/run-clang-tidy.py
