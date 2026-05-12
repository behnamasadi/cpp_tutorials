# Error reporting in C and C++: `errno` and `std::error_code`

C and C++ have two parallel mechanisms for reporting *recoverable* errors from a function without throwing an exception:

- **C-style `errno`** — a thread-local integer set by libc and POSIX functions. Old, ubiquitous, type-unsafe.
- **`std::error_code`** (C++11, `<system_error>`) — a typed pair of `(value, category)` that adds context and composes across libraries. Recommended for new C++ code.

Both shine in environments where exceptions are off the table: realtime control loops, signal handlers, freestanding firmware, or any code path where the deterministic overhead matters more than the convenience of `throw`.

- [1. C-style `errno`](#1-c-style-errno)
- [2. `std::error_code`](#2-stderror_code)
- [3. Migrating from `errno` to `std::error_code`](#3-migrating-from-errno-to-stderror_code)
- [4. When to use which](#4-when-to-use-which)

---

## 1. C-style `errno`

Many C library functions signal failure by returning a sentinel (`-1`, `NULL`, `0`) **and** setting the global `errno`. It's declared in `<errno.h>` (`<cerrno>` in C++) and is thread-local since C11 — you can read it without worrying about other threads stomping on it.

```cpp
#include <cerrno>
#include <cstdio>
#include <cstring>

std::FILE* fp = std::fopen("missing.txt", "r");
if (!fp) {
    std::printf("errno=%d (%s)\n", errno, std::strerror(errno));
    // errno=2 (No such file or directory)
}
```

### Common `errno` values

| Value | Macro | Meaning |
|------|-----------------|---------|
| 1    | `EPERM`         | Operation not permitted |
| 2    | `ENOENT`        | No such file or directory |
| 3    | `ESRCH`         | No such process |
| 4    | `EINTR`         | Interrupted system call |
| 5    | `EIO`           | I/O error |
| 6    | `ENXIO`         | No such device or address |
| 9    | `EBADF`         | Bad file descriptor |
| 11   | `EAGAIN`        | Resource temporarily unavailable (try again) |
| 12   | `ENOMEM`        | Out of memory |
| 13   | `EACCES`        | Permission denied |
| 32   | `EPIPE`         | Broken pipe |
| 110  | `ETIMEDOUT`     | Connection timed out |

The numeric values are platform-specific; **always** use the macros, never the bare integers.

### Reading `errno` correctly

Three rules that bite people:

1. **Check the function's return first.** `errno` is only meaningful after a function has signaled failure via its own return. Reading it after a successful call gives whatever was leftover.
2. **Save it immediately.** Almost anything (including the very `printf` you use to log the error) can clobber `errno`. Capture it into a local:

   ```cpp
   int err = errno;
   log("open failed: %s", std::strerror(err));
   ```
3. **Don't set `errno` yourself across library boundaries.** If you're writing a library, return your own error type — don't pollute a global the caller may rely on.

### Translating `errno` to a string

- `std::strerror(errno)` — convenient, but not thread-safe in C (POSIX `strerror_r` and Windows `strerror_s` are the reentrant variants).
- `std::perror("context")` — writes `"context: <message>\n"` to stderr.

In C++ code, prefer wrapping the value into a `std::error_code` (next section), which gives you the same message via `.message()` plus a typed category.

---

## 2. `std::error_code`

`std::error_code` represents an error as a `(value, category)` pair. The value is an integer; the category identifies which error domain it belongs to (POSIX errors, Windows API errors, your library's errors, etc.). Two error codes compare equal only if both their values and their categories match.

This composes much better than raw integers: a "value of 2" from POSIX (`ENOENT`) and "value of 2" from your application's error enum mean entirely different things, and the category keeps them straight.

### 2.1 Basic use — reporting an error via an out parameter

```cpp
#include <fstream>
#include <iostream>
#include <system_error>

void open_file(const std::string& path, std::error_code& ec) {
    std::ifstream file(path);
    if (!file) {
        ec = std::make_error_code(std::errc::no_such_file_or_directory);
        return;
    }
    ec.clear();   // success
}

int main() {
    std::error_code ec;
    open_file("nope.txt", ec);
    if (ec) {
        std::cout << "open failed: " << ec.message()
                  << " (code " << ec.value()
                  << ", category " << ec.category().name() << ")\n";
    }
}
```

Key methods:
- `ec.value()` — the numeric error value.
- `ec.message()` — human-readable description.
- `ec.category()` — the `std::error_category` reference (e.g. `generic_category`, `system_category`).
- `bool(ec)` — true if there is an error (i.e. `value() != 0`).

### 2.2 Wrapping an `errno`

`std::system_category()` represents OS-level errors. To bridge from `errno`:

```cpp
#include <cerrno>
#include <cstdio>
#include <system_error>

void remove_file(const std::string& path, std::error_code& ec) {
    if (std::remove(path.c_str()) != 0)
        ec = std::error_code(errno, std::system_category());
    else
        ec.clear();
}
```

This preserves the OS-native error value and category, so callers can interrogate it portably (or compare it against a `std::errc` portable name — see §2.4).

### 2.3 Standard-library APIs that take an `error_code&`

Most of `<filesystem>`, `<thread>` (joinable, sleep), and the regex engine offer non-throwing overloads:

```cpp
std::error_code ec;
auto sz = std::filesystem::file_size("data.bin", ec);
if (ec) { /* handle without exception */ }
```

That's the idiom — pass an `error_code` to opt out of exceptions, omit it to opt in.

### 2.4 Comparing against portable conditions

`std::errc` is an enum of *portable* error names (`std::errc::no_such_file_or_directory`, etc.). Compare an `error_code` against it via `std::error_condition`:

```cpp
if (ec == std::errc::permission_denied) {
    // works whether ec came from system_category() on Linux or Windows
}
```

The category's `default_error_condition()` does the cross-platform translation.

### 2.5 Defining a custom error category

For library-specific errors, derive from `std::error_category`:

```cpp
enum class parse_err { ok = 0, empty_input, bad_number, out_of_range };

class parse_category_t : public std::error_category {
public:
    const char* name() const noexcept override { return "parse"; }
    std::string message(int v) const override {
        switch (static_cast<parse_err>(v)) {
            case parse_err::empty_input:  return "empty input";
            case parse_err::bad_number:   return "not a number";
            case parse_err::out_of_range: return "value out of range";
            default:                      return "ok";
        }
    }
};

const std::error_category& parse_category() {
    static parse_category_t c;
    return c;
}

// hook into the std::error_code machinery
std::error_code make_error_code(parse_err e) {
    return { static_cast<int>(e), parse_category() };
}

namespace std { template<> struct is_error_code_enum<parse_err> : true_type {}; }
```

Now callers can write `if (ec == parse_err::bad_number)` and it Just Works.

---

## 3. Migrating from `errno` to `std::error_code`

A typical conversion at a library boundary:

```cpp
// before — leaks errno semantics to callers
int read_sensor(double& out) {
    if (ioctl(fd, ...) < 0) return -errno;
    out = ...;
    return 0;
}

// after — typed, composable, exception-free
std::error_code read_sensor(double& out) {
    if (ioctl(fd, ...) < 0)
        return { errno, std::system_category() };
    out = ...;
    return {};
}
```

For richer error reporting where you want both a typed result and an error reason, see `std::expected<T, E>` (C++23) — see [error_handling_strategies.md](system_design/error_handling_strategies.md).

---

## 4. When to use which

- **Pure C interop** — accept that `errno` exists, but save its value immediately and translate to your own type as soon as possible.
- **Realtime / no-exceptions code** — `std::error_code` (or `std::expected<T, std::error_code>`) is the right primitive.
- **High-level application code with exceptions enabled** — throw `std::system_error(ec, msg)`, which wraps an `error_code` into a typed exception. Lets you keep the rich error info while using normal C++ exception handling at the boundary.
- **A library that may be used either way** — provide both forms (throwing and `error_code&` overload), as `<filesystem>` does.

[error_code example](../src/error_code.cpp), [errno example](../src/error_handling.cpp)
