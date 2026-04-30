# Testing Strategies

Tests aren't a separate phase that follows coding; they're a property of the design. A well-tested system has different shapes of tests, each catching a different class of bug, and each running at a different cost.

- [1. The Testing Pyramid](#1-the-testing-pyramid)
- [2. Unit Tests](#2-unit-tests)
- [3. Integration Tests](#3-integration-tests)
- [4. End-to-End Tests](#4-end-to-end-tests)
- [5. Mocking — Used Correctly](#5-mocking--used-correctly)
- [6. Property-Based Testing](#6-property-based-testing)
- [7. Fuzzing](#7-fuzzing)
- [8. Contract Testing](#8-contract-testing)
- [9. Sanitizers as Tests](#9-sanitizers-as-tests)
- [10. CI Strategy](#10-ci-strategy)

---

# 1. The Testing Pyramid

```
       /\
      /E2E\         few, slow, expensive, full system
     /------\
    /  int.  \     more, mid-speed, multiple components together
   /----------\
  /   unit     \   many, fast, single function/class
  --------------
```

| | Unit | Integration | E2E |
|---|---|---|---|
| Per test, time | <1 ms | 10 ms – 1 s | 1–60 s |
| Number of tests | 1000s | 100s | 10s |
| Diagnoses | "this function is wrong" | "two components disagree" | "the whole stack is wrong" |
| Cost to maintain | Low | Medium | High (flaky, slow, brittle) |

Inverted pyramid (lots of E2E, few units) is the typical anti-pattern: slow CI, painful failures, hard to localize bugs.

# 2. Unit Tests

Test one function or class in isolation. Should be fast (<1 ms), deterministic, no external state.

C++ unit-test frameworks:
- **GoogleTest** — most common in industry. Mature; integrates with everything.
- **Catch2** — header-only, expressive `REQUIRE(x == y)` syntax, excellent for small projects.
- **doctest** — fastest compile times; lowest ceremony.
- **Boost.Test** — if you already use Boost.

```cpp
#include <gtest/gtest.h>
#include <optional>
#include <string>

struct Credential { bool expired; };

class TokenIssuer {
public:
    enum class Error { None, Expired };

    std::optional<std::string> issueFor(const std::string& user, Credential c) {
        if (c.expired) { lastError = Error::Expired; return std::nullopt; }
        lastError = Error::None;
        return "token-for-" + user;
    }

    Error lastError = Error::None;
};

TEST(TokenIssuer, RefusesExpiredCredentials) {
    TokenIssuer issuer;
    Credential expired{true};

    auto token = issuer.issueFor("user42", expired);

    EXPECT_FALSE(token.has_value());
    EXPECT_EQ(issuer.lastError, TokenIssuer::Error::Expired);
}
```

What makes a unit test good:
- **One concept per test.** If it fails, you know which behavior broke.
- **Arrange / Act / Assert** structure visible at a glance.
- **No coupling to other tests.** Order shouldn't matter.
- **Tests behavior, not implementation.** Refactoring shouldn't break tests if behavior is the same.

# 3. Integration Tests

Test how components fit together. Hit a real (or in-memory) database, spin up a real HTTP server, exercise the wire protocol.

- Don't mock the boundary you're integrating across — the bug you're trying to find is likely *in* the integration.
- Use containers: PostgreSQL in Docker, Redis, Kafka. [Testcontainers](https://testcontainers.com/) is the standard.
- For databases: spin up a real one, run schema migrations, run the test, drop. ~1s per test if optimized.
- Keep integration tests narrow — exercise *one* integration at a time, not the whole stack.

# 4. End-to-End Tests

The full system, exercised through its real entry points. UI-driven for web apps; full-stack for services. Slow, brittle, expensive — but they catch a class of bug nothing else does (deployment misconfiguration, real-world latency, third-party API changes).

Discipline:
- **Have few of them.** A handful that exercise critical user journeys.
- **Treat flake as a defect.** A flaky E2E that everyone ignores becomes worse than no test.
- **Run on a schedule, not on every commit.** Or as a "smoke" subset on PR, full suite nightly.

# 5. Mocking — Used Correctly

Mocks substitute a dependency in a unit test. **Most tests don't need mocks** — a real `std::vector` works in tests, and so does an in-memory hash map for "the cache."

| Term | Meaning |
|---|---|
| **Dummy** | Object passed but not used. |
| **Stub** | Returns canned data. "If asked for user 42, return Alice." |
| **Fake** | Lightweight working impl. In-memory DB; not for prod, real for tests. |
| **Mock** | Verifies it was *called* in a particular way. |
| **Spy** | Records calls; you check after. |

Prefer **fakes over mocks**. A `FakeDatabase` that's a real working implementation lets you write tests in terms of behavior ("then the user is saved") instead of choreography ("then `save()` was called once with these args"). Mock tests tend to break on every refactor because they assert on the implementation.

GoogleMock for when you do want mocks:
```cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>

struct Order { int id; };

class Database {
public:
    virtual ~Database() = default;
    virtual void save(const Order& o) = 0;
};

class MockDatabase : public Database {
public:
    MOCK_METHOD(void, save, (const Order&), (override));
};

class OrderService {
public:
    OrderService(Database& db) : db(db) {}
    void placeOrder(const Order& o) { db.save(o); }
private:
    Database& db;
};

using ::testing::_;

TEST(OrderService, SavesAcceptedOrder) {
    MockDatabase db;
    EXPECT_CALL(db, save(_)).Times(1);

    OrderService svc(db);
    svc.placeOrder(Order{1});
}
```

Reach for it sparingly.

# 6. Property-Based Testing

Instead of "for input X, output should be Y," state a *property* the code should satisfy for all inputs, and let the framework generate inputs:

```cpp
#include <rapidcheck.h>
#include <algorithm>
#include <vector>

int main() {
    rc::check("reverse twice yields original", [](std::vector<int> v) {
        auto r = v;
        std::reverse(r.begin(), r.end());
        std::reverse(r.begin(), r.end());
        RC_ASSERT(r == v);
    });
}
```

Properties to test:
- **Round-trip**: `decode(encode(x)) == x`
- **Idempotence**: `f(f(x)) == f(x)`
- **Invariants**: "after sort, every adjacent pair is in order"
- **Equivalence**: "the optimized impl agrees with the slow reference"

C++ frameworks: [rapidcheck](https://github.com/emil-e/rapidcheck), [proptest-cxx](https://github.com/proptest-cpp/proptest-cpp). Great for parsers, serializers, math.

# 7. Fuzzing

Generate random/structured inputs targeting code paths, run with sanitizers, watch for crashes or assertion failures.

- **libFuzzer** (built into Clang) — in-process coverage-guided fuzzer.
- **AFL++** — out-of-process; better for big binaries.
- **Honggfuzz** — alternative with strong instrumentation.
- **OSS-Fuzz** — Google's continuous fuzzing for OSS projects; free if you qualify.

```cpp
#include <cstddef>
#include <cstdint>

void parse_my_format(const uint8_t* data, size_t size);

// libFuzzer entry point
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    parse_my_format(data, size);    // crash here = bug
    return 0;
}
```

```bash
clang++ -fsanitize=fuzzer,address,undefined fuzz.cpp parse.cpp -o fuzz
./fuzz corpus/
```

What fuzzes well:
- Parsers (JSON, custom binary formats).
- Deserializers (protobuf, ASN.1).
- State machines (random sequences of events).
- File-format readers.

Run fuzzers on a schedule or on PR. Fuzzing finds bugs nothing else does — particularly UB, memory safety, and adversarial input bugs.

# 8. Contract Testing

Two services communicate over a contract (REST schema, protobuf). Each side tests against a recorded contract instead of mocking the other.

- Provider records: "given this request, I produce this response."
- Consumer asserts: "given that contract, my code parses correctly."
- Tooling: [Pact](https://docs.pact.io/), Postman/Insomnia for snapshot tests.

Cheaper than full integration; catches the "we changed our schema and didn't tell anyone" failure mode. Useful in microservice-heavy environments.

# 9. Sanitizers as Tests

Compiler-level dynamic analyzers that turn UB into reproducible test failures. Run them in CI, not just locally.

| Sanitizer | Catches |
|---|---|
| **ASan** | Heap/stack OOB, use-after-free, double-free, leaks |
| **UBSan** | Undefined behavior — int overflow, null deref, invalid casts |
| **TSan** | Data races |
| **MSan** | Reads of uninitialized memory (Clang only) |
| **LSan** | Memory leaks (often bundled with ASan) |

```bash
clang++ -fsanitize=address,undefined -O1 -g foo.cpp -o foo
./foo                                    # crashes loudly on bugs
```

A green test suite under sanitizers is far stronger evidence of correctness than a green plain suite.

See [Memory Error Detection With Sanitizers](../memory_leaking_valgrind.md).

# 10. CI Strategy

The shape of a healthy CI:

| Stage | What | Time budget |
|---|---|---|
| Pre-commit hook | Format, lint, fast unit tests | <10 s |
| PR check | Full unit suite, build matrix, ASan/UBSan | <10 min |
| Merge to main | + Integration tests, container build | <30 min |
| Nightly | + E2E, fuzzing run, perf tests, TSan | <2 h |

Discipline:
- **No flaky tests.** Quarantine, then fix or delete.
- **Test in the matrix you ship.** Test on every compiler/OS you claim to support.
- **Run a sanitized build in CI.** If sanitizers fire, the build fails.
- **Test the release build, not just debug.** Many bugs only show under `-O2`.
- **Reproducible builds.** Same commit → same artifact. Required for trustworthy debugging.

# References

- [Designing for Testability](designing_for_testability.md)
- [Memory Error Detection With Sanitizers](../memory_leaking_valgrind.md)
- [GoogleTest](https://github.com/google/googletest), [Catch2](https://github.com/catchorg/Catch2), [doctest](https://github.com/doctest/doctest)
- [libFuzzer documentation](https://llvm.org/docs/LibFuzzer.html), [AFL++](https://github.com/AFLplusplus/AFLplusplus)
- [Testcontainers](https://testcontainers.com/)
- *Working Effectively with Legacy Code*, Michael Feathers — on adding tests to untested code.
- *Software Engineering at Google*, chapters 11–14 — testing at scale.
