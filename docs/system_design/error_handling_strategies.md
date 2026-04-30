# Error Handling Strategies and Fault Tolerance

Error handling is a *system-level* design decision, not a per-function one. Mixing strategies in the same codebase is the usual disaster. This page covers the available mechanisms, when each is the right choice, and how to build fault-tolerant systems on top of them.

- [1. The Mechanisms](#1-the-mechanisms)
- [2. Picking One: Decision Matrix](#2-picking-one-decision-matrix)
- [3. `std::expected` (C++23)](#3-stdexpected-c23)
- [4. `std::error_code` and `std::system_error`](#4-stderror_code-and-stdsystem_error)
- [5. Errors vs Programmer Bugs](#5-errors-vs-programmer-bugs)
- [6. Fault Tolerance Patterns](#6-fault-tolerance-patterns)
- [7. References](#7-references)

---

# 1. The Mechanisms

| Mechanism | Cost on success | Failure model | When natural |
|---|---|---|---|
| **Exceptions** | ~Zero (table-based unwinding) | Stack unwinds, RAII releases | Deeply nested calls; rare failures |
| **Error codes / `errno`** | One return + check | Caller branches | C interop; embedded; predictable failures |
| **`std::error_code`** | One return + check | Strongly-typed code + category | Library boundary errors |
| **`std::expected<T,E>`** | One return + check | Pattern match on result | Modern, value-semantic; expressive |
| **`std::optional<T>`** | One return + check | Has-value vs not | Failure carries no info ("not found") |
| **`std::variant<T,Errs...>`** | One return + visit | Exhaustive match | Multi-error, sum-type style |
| **Boolean + outparam** | One return + outparam | Caller checks | C-style |
| **`abort` / `terminate`** | Zero | Process dies | Bug; unrecoverable invariant violation |

# 2. Picking One: Decision Matrix

| Constraint | Choose |
|---|---|
| Real-time / embedded with `-fno-exceptions` | `std::expected` or error codes |
| Library exposed across ABI boundary | `std::error_code` or C-style |
| Application code, deep call stack, exceptions enabled | Exceptions |
| Performance-critical hot path, frequent expected failures | `expected`/`optional` |
| Programmer bug detection (e.g. failed assertion) | `assert` or `std::abort` (not exceptions) |
| Cross-language (Python/Rust/Go) | C-style return code |

The single biggest mistake: using exceptions for *expected* control flow (e.g. "key not found in map"). Exceptions are zero-cost on the success path but expensive on the throw path — orders of magnitude slower than a return.

# 3. `std::expected` (C++23)

`std::expected<T, E>` holds either a value of type `T` or an error of type `E`.

```cpp
#include <charconv>
#include <expected>
#include <iostream>
#include <string>

enum class ParseError { Empty, BadDigit, OutOfRange };

std::expected<int, ParseError> parseInt(const std::string& s) {
    if (s.empty()) return std::unexpected(ParseError::Empty);
    int value = 0;
    auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), value);
    if (ec == std::errc::invalid_argument)    return std::unexpected(ParseError::BadDigit);
    if (ec == std::errc::result_out_of_range) return std::unexpected(ParseError::OutOfRange);
    return value;
}

int main() {
    auto r = parseInt("42");
    if (r) std::cout << *r << "\n";
    else   std::cout << "error: " << static_cast<int>(r.error()) << "\n";
}
```

It also supports monadic chaining (`and_then`, `or_else`, `transform`):

```cpp
std::string input = "42";
auto result = parseInt(input)
    .transform([](int x) { return x * 2; })
    .or_else([](ParseError) { return std::expected<int, ParseError>{0}; });
// result holds 84 on success, or 0 if parseInt failed.
```

For C++17/20 codebases, `tl::expected` is a high-quality drop-in.

# 4. `std::error_code` and `std::system_error`

`std::error_code` is a small (`int + category*`) value designed for the *library boundary*: any subsystem can define a category, and consumers can compare/categorize without knowing the source.

```cpp
#include <filesystem>
#include <iostream>
#include <system_error>

int main() {
    std::error_code ec;
    auto sz = std::filesystem::file_size("foo.txt", ec);
    if (ec) {
        std::cerr << ec.message() << " (cat=" << ec.category().name() << ")\n";
    } else {
        std::cout << "size = " << sz << "\n";
    }
}
```

You can throw the code as `std::system_error` if you want exception semantics:

```cpp
#include <cerrno>
#include <system_error>

void openFile() {
    throw std::system_error(errno, std::generic_category(), "open failed");
}
```

Custom categories let you map your domain errors into the same protocol:

```cpp
#include <iostream>
#include <string>
#include <system_error>

enum class NetErr { Timeout = 1, DnsFail, Refused };

class NetErrCategory : public std::error_category {
public:
    const char* name() const noexcept override { return "net"; }
    std::string message(int v) const override {
        switch (static_cast<NetErr>(v)) {
            case NetErr::Timeout: return "timeout";
            case NetErr::DnsFail: return "dns failure";
            case NetErr::Refused: return "connection refused";
        }
        return "unknown";
    }
};

int main() {
    NetErrCategory cat;
    std::cout << cat.name() << ": "
              << cat.message(static_cast<int>(NetErr::DnsFail)) << "\n";
}
```

# 5. Errors vs Programmer Bugs

These are not the same thing and must not share a mechanism.

| | Error | Bug |
|---|---|---|
| Cause | Environment (network, disk, user input) | Wrong code (off-by-one, null deref, wrong invariant) |
| Recovery | Possible and intended | Impossible — code is wrong |
| Mechanism | exception / `expected` / `error_code` | `assert`, `std::abort`, contract violation |
| In release? | Yes, handled | Often `assert`-stripped, but better: keep |

Catching a bug as if it were an error hides defects and ships them to production. Conversely, `abort()`-ing on a network timeout is poor engineering. Distinguishing these is the foundation of robust design.

# 6. Fault Tolerance Patterns

Beyond per-call error handling, system-level fault tolerance:

**Timeouts.** Every call to anything external must have one. No exceptions. ([Real-Time Systems](realtime_systems.md))

**Retries with exponential backoff and jitter.**
```cpp
#include <chrono>
#include <expected>
#include <random>
#include <thread>

std::expected<int, int> call();  // returns a value or an error code

std::chrono::milliseconds jitter() {
    static std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> dist(0, 25);
    return std::chrono::milliseconds(dist(rng));
}

std::expected<int, int> callWithRetry() {
    std::chrono::milliseconds delay{50};
    for (int attempt = 0; attempt < 5; ++attempt) {
        if (auto r = call(); r) return r;
        std::this_thread::sleep_for(delay + jitter());
        delay *= 2;
    }
    return std::unexpected(-1);
}
```
Don't retry idempotent operations only. Don't retry forever — bound it.

**Circuit breaker.** Track failure rate; once a threshold is crossed, fail fast for a cooling-off window instead of dogpiling a sick service. Three states: Closed (normal), Open (fail fast), Half-Open (probe).

**Bulkheads.** Isolate failures: dedicate a thread pool / connection pool / memory budget per subsystem so one component going wrong can't drain shared resources.

**Graceful degradation.** When a non-essential subsystem fails, return a degraded result (e.g., serve cached data, skip personalization) rather than failing the whole request.

**Idempotency keys.** When retrying a write, the server must dedupe by key — otherwise a retry doubles the effect.

**Fail-fast vs fail-soft.** Choose per subsystem. The boot path fails fast (config wrong → exit). The user request path fails soft (one feature broken → still return the page).

**Crash-only design.** Make recovery == startup. No "graceful shutdown" path that's only exercised in production. This forces the recovery path to actually work.

# 7. References

- [Exception Safety Guarantees](exception_safety_guarantees.md)
- [Exception Handling, noexcept](../exception_handling.md)
- [Error Code](../error_code.md)
- [`std::expected` cppreference](https://en.cppreference.com/w/cpp/utility/expected)
- *Release It!*, Michael Nygard — the canonical book on production fault tolerance.
- *Designing Data-Intensive Applications*, Martin Kleppmann — chapter 8 on reliability.
