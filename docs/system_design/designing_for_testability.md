# Designing for Testability

Testability is a property of the design, not a property of the test suite. When code is hard to test, the design is fighting you. The fix is rarely "more mocks" — it's making the code substitutable.

- [1. The Symptoms of Untestable Code](#1-the-symptoms-of-untestable-code)
- [2. Seams](#2-seams)
- [3. Dependency Injection for Testability](#3-dependency-injection-for-testability)
- [4. Pure Functions Where Possible](#4-pure-functions-where-possible)
- [5. Time, Random, and Files](#5-time-random-and-files)
- [6. The Humble Object Pattern](#6-the-humble-object-pattern)
- [7. Fakes vs Mocks Revisited](#7-fakes-vs-mocks-revisited)
- [8. Visibility for Testing](#8-visibility-for-testing)
- [9. Don't Test Private Methods](#9-dont-test-private-methods)
- [10. Quick Reference](#10-quick-reference)

---

# 1. The Symptoms of Untestable Code

If any of these are true, the code is fighting back:

- "I need a real database / network to test this."
- "The test takes 10 seconds because it sleeps."
- "I can't test the error path without breaking the file system."
- "The test calls `srand(0)` because the function reads `rand()` directly."
- "I have to set the system clock to test the cron logic."
- "The test reads global state set by another test."
- "I have to make a member public to test it."
- "The class has 12 dependencies in its constructor."
- "We use `static` everywhere, can't substitute anything."

Each symptom maps to a known design smell. The cures are below.

# 2. Seams

A *seam* is a place in the code where you can substitute behavior without modifying it. Michael Feathers's *Working Effectively with Legacy Code* coined the term.

| Seam type | How to introduce |
|---|---|
| **Object** | Take a dependency by interface, inject it. |
| **Preprocessor** | `#ifdef TEST` to swap implementations (avoid; brittle). |
| **Link-time** | Compile a different `.cpp` for tests. Common for stubs of OS calls. |
| **Template** | Take the dependency as a template parameter. Compile-time DI. |
| **Function pointer** | Use `std::function` so callers can swap. |
| **Subclass-and-override** | Mark methods virtual; override in tests. (Last resort.) |

Adding a seam costs a small abstraction; not having one costs every future test.

# 3. Dependency Injection for Testability

Constructor-inject every external dependency:

```cpp
#include <chrono>
#include <string>

struct Database { std::string url; };
struct Clock { virtual std::chrono::system_clock::time_point now() = 0; };
struct Token { std::string value; };

// Untestable
class TokenIssuer {
    Database db{"prod-db.local"};                // hard-coded
    std::chrono::system_clock::time_point now   // direct call to global clock
        = std::chrono::system_clock::now();
public:
    Token issue();
};

// Testable
class TokenIssuer {
    Database& db;
    Clock& clock;
public:
    TokenIssuer(Database& d, Clock& c) : db(d), clock(c) {}
    Token issue();
};
```

The test wires in `FakeDatabase` and `FakeClock`. Production wires in `PostgresDatabase` and `SystemClock`. See [Dependency Injection](dependency_injection.md).

# 4. Pure Functions Where Possible

A pure function — same input → same output, no side effects — is the most testable thing in computing. No setup, no teardown, no fakes:

```cpp
#include <cassert>

// Pure: trivially testable
struct Money { int cents; };
Money applyDiscount(Money m, double pct) { return {int(m.cents * (1 - pct))}; }

int main() {
    assert(applyDiscount({100}, 0.5).cents == 50);
    assert(applyDiscount({200}, 0.25).cents == 150);
}
```

Push as much logic as possible into pure functions. Wrap them with a thin imperative shell that handles I/O and side effects (this is sometimes called "functional core, imperative shell").

# 5. Time, Random, and Files

These three globals are the most common reason "this code is hard to test."

**Time.** Replace direct calls with a `Clock` interface:
```cpp
#include <chrono>
#include <iostream>

struct Clock {
    virtual std::chrono::system_clock::time_point now() const = 0;
    virtual ~Clock() = default;
};

struct SystemClock : Clock {
    std::chrono::system_clock::time_point now() const override {
        return std::chrono::system_clock::now();
    }
};

struct FakeClock : Clock {
    std::chrono::system_clock::time_point t;
    std::chrono::system_clock::time_point now() const override { return t; }
    void advance(std::chrono::seconds s) { t += s; }
};

int main() {
    FakeClock fake;
    auto before = fake.now();
    fake.advance(std::chrono::seconds(60));
    auto after = fake.now();
    std::cout << "advanced " << (after - before).count() << " ticks\n";
}
```

**Random.** Inject the RNG. `std::mt19937_64& rng` as a parameter. Tests pass a seeded one for determinism.

**Filesystem.** Take a `std::filesystem::path` parameter for the directory to use. Tests use a `tmpdir`. Or wrap in an interface and have a memory-backed fake.

# 6. The Humble Object Pattern

When code is genuinely hard to test (UI rendering, OS callbacks, hardware interrupts), make the untestable layer thin and dumb. All real logic moves to a testable companion class.

```cpp
// Stand-ins for the GUI framework so we can focus on the pattern.
struct QMouseEvent { int x, y; };
struct QWidget { virtual void mousePressEvent(QMouseEvent*) {} };
enum class Action { None, Submit, Cancel };
void executeAction(Action) { /* call into the GUI */ }

// Untestable: the GUI button itself
class Button : public QWidget {
    void mousePressEvent(QMouseEvent*) override { /* lots of logic */ }
};

// Better: humble shell + testable presenter
class ButtonPresenter {
public:
    Action handlePress(int x, int y) const {
        if (x < 0 || y < 0) return Action::Cancel;
        return Action::Submit;
    }
};

class HumbleButton : public QWidget {
    ButtonPresenter p;
    void mousePressEvent(QMouseEvent* e) override {
        Action a = p.handlePress(e->x, e->y);
        executeAction(a);
    }
};

int main() {
    ButtonPresenter p;
    // Tests target the presenter directly — no GUI needed.
    Action a = p.handlePress(10, 20);
    (void)a;
}
```

Tests target the presenter. The button is so trivial that "test it" devolves to "click it manually once."

# 7. Fakes vs Mocks Revisited

Most of the testability problems beginners blame on "no good mocks library" disappear when you write a fake — a real working in-memory implementation:

```cpp
#include <optional>
#include <string>
#include <unordered_map>

using UserId = int;
struct User { UserId id; std::string name; };

// The seam: an abstract Database interface.
struct Database {
    virtual void save(const User& u) = 0;
    virtual std::optional<User> find(UserId id) = 0;
    virtual ~Database() = default;
};

// A fake — a real working in-memory implementation.
class FakeDatabase : public Database {
    std::unordered_map<UserId, User> users;
public:
    void save(const User& u) override { users[u.id] = u; }
    std::optional<User> find(UserId id) override {
        auto it = users.find(id);
        if (it == users.end()) return std::nullopt;
        return it->second;
    }
};

int main() {
    FakeDatabase db;
    db.save({1, "Alice"});
    auto found = db.find(1);
    // Test reads what the production code wrote.
}
```

Tests write to it, read from it. Reads what was written. The test reads like the spec; refactoring the production code doesn't break the test.

A mock-heavy alternative often calcifies the implementation: every change in `OrderService` breaks twenty tests asserting "and then `db.save` was called once with these exact args." See [Testing Strategies](testing_strategies.md#5-mocking--used-correctly).

# 8. Visibility for Testing

Sometimes a test needs to peek inside a class. Options, in increasing order of compromise:

1. **Refactor.** If you need to test a private helper, often it should be a free function or a separate class.
2. **Friendship.** `friend class FooTest;` — explicit, narrow.
3. **`protected:` + subclass for tests.** Cleaner than `friend` if there's a hierarchy already.
4. **`#ifdef TESTING` to expose.** Brittle, last resort.
5. **Reach in via `reinterpret_cast`.** No.

The cleanest fix is almost always option 1: if a private function is complex enough to test, it deserves to be its own well-named entity.

# 9. Don't Test Private Methods

Tests should target the public behavior. Reasons:

- Private methods are implementation details; you'll refactor them often.
- A test on a private method *is* a test on implementation, not behavior.
- If you can't test a class's behavior through its public API, the class probably has too many responsibilities.

When tempted to test a private:
- Extract it to a separate class with a public API.
- Test the public API of the original class with inputs that exercise the private code path.

# 10. Quick Reference

| Problem | Fix |
|---|---|
| Test needs a real database | Inject the DB; use a fake in tests |
| Test needs the system clock | Inject a `Clock`; use `FakeClock` in tests |
| Test needs a network call | Inject the HTTP client; use a fake |
| Code uses `static` state | Refactor to instance state; inject if shared |
| Code calls `new` directly | Inject a factory; use a stub factory in tests |
| Singleton everywhere | Replace with DI; pass the instance, don't pull it from global |
| 12-arg constructor | The class has too many responsibilities — split it |
| Test is flaky | Find the global state or timing assumption; remove it |
| UI logic is untestable | Humble Object — extract presenter |
| Have to make member public | Refactor instead; or `friend` narrowly |

# References

- [Dependency Injection](dependency_injection.md)
- [Testing Strategies](testing_strategies.md)
- [SOLID Principles](solid_principles.md)
- *Working Effectively with Legacy Code*, Michael Feathers — the canonical book on adding tests to untested code.
- *Test-Driven Development*, Kent Beck.
- *xUnit Test Patterns*, Gerard Meszaros.
