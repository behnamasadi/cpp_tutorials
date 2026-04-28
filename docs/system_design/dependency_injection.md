# Dependency Injection and Inversion of Control

Dependency Injection (DI) is the practice of giving an object its dependencies from the outside instead of having it construct or look them up itself. It's the most direct way to apply the Dependency Inversion Principle (the "D" in [SOLID](solid_principles.md)).

- [1. The Problem It Solves](#1-the-problem-it-solves)
- [2. Forms of Injection](#2-forms-of-injection)
- [3. Compile-Time DI with Templates](#3-compile-time-di-with-templates)
- [4. Runtime DI with Interfaces](#4-runtime-di-with-interfaces)
- [5. DI Containers — Use With Care](#5-di-containers--use-with-care)
- [6. Lifetime, Ownership, and Scopes](#6-lifetime-ownership-and-scopes)
- [7. Testing Benefits](#7-testing-benefits)
- [8. Anti-patterns](#8-anti-patterns)

---

# 1. The Problem It Solves

Consider:

```cpp
class OrderService {
    MySqlDatabase db_;          // hard-coded
    SmtpEmailer emailer_;       // hard-coded
public:
    void placeOrder(const Order& o) {
        db_.save(o);
        emailer_.sendConfirmation(o.customerEmail);
    }
};
```

Problems:
- Cannot test without a real MySQL and SMTP server.
- Cannot swap MySQL for Postgres without modifying `OrderService`.
- Tight coupling — every change to `MySqlDatabase` triggers `OrderService` recompiles.

DI flips it:

```cpp
class OrderService {
    Database& db_;
    Emailer&  emailer_;
public:
    OrderService(Database& db, Emailer& e) : db_(db), emailer_(e) {}
    void placeOrder(const Order& o) {
        db_.save(o);
        emailer_.sendConfirmation(o.customerEmail);
    }
};
```

`OrderService` no longer cares which database or emailer; it depends only on the interface. The wiring decision is made by *whoever constructs* `OrderService` — typically a small "composition root" near `main()`.

# 2. Forms of Injection

| Form | Mechanism | Pro | Con |
|---|---|---|---|
| **Constructor** | Dependencies as ctor params | All deps visible; objects always valid | Can produce long ctors |
| **Setter** | Setters after construction | Optional/late-bound deps | Object invalid between ctor and setters |
| **Method** | Pass dep into the call that needs it | Per-call flexibility | Deeper signatures, repeated args |
| **Property** | Public member | Trivial | Breaks encapsulation; rarely worth it |

**Default to constructor injection.** Use method injection for cross-cutting one-call dependencies (e.g., a logger passed into a single operation). Use setter injection only for genuinely optional dependencies.

# 3. Compile-Time DI with Templates

In C++ you don't have to use virtual dispatch. Templates can inject dependencies at compile time with zero runtime cost:

```cpp
template<class Db, class Emailer>
class OrderService {
    Db& db_;
    Emailer& emailer_;
public:
    OrderService(Db& db, Emailer& e) : db_(db), emailer_(e) {}
    void placeOrder(const Order& o) {
        db_.save(o);
        emailer_.sendConfirmation(o.customerEmail);
    }
};
```

With C++20 [concepts](https://en.cppreference.com/w/cpp/language/constraints), you can constrain what counts as a `Db`:

```cpp
template<class T>
concept DatabaseLike = requires(T& t, const Order& o) {
    { t.save(o) } -> std::same_as<void>;
};
```

When to prefer compile-time DI: hot paths, embedded code, or when the set of implementations is small and known. When to prefer runtime DI: plugin systems, code where the dependency varies per request, or when you need to ship an ABI-stable interface.

# 4. Runtime DI with Interfaces

```cpp
struct Database { virtual ~Database() = default; virtual void save(const Order&) = 0; };
struct Emailer  { virtual ~Emailer() = default;  virtual void sendConfirmation(const std::string&) = 0; };

class OrderService {
    Database* db_;
    Emailer*  emailer_;
public:
    OrderService(Database* db, Emailer* e) : db_(db), emailer_(e) {}
    void placeOrder(const Order& o) { db_->save(o); emailer_->sendConfirmation(o.customerEmail); }
};

// Composition root
int main() {
    PostgresDb db{...};
    SmtpEmailer em{...};
    OrderService svc{&db, &em};
    runApp(svc);
}
```

Use raw `T&`/`T*` when the caller owns the dependency and outlives the consumer. Use `std::shared_ptr<T>` when ownership is genuinely shared. Avoid `std::unique_ptr<T>` parameters unless you really mean transfer of ownership.

# 5. DI Containers — Use With Care

DI containers (Boost.DI, Hypodermic, Fruit) automate the wiring. They're useful when:
- You have *many* implementations selected by config.
- Object graphs are deep and assembly is repetitive.
- You need explicit support for scopes (singleton, per-request, per-thread).

They become a problem when:
- Wiring becomes "magic" and hard to follow with a debugger.
- Compile errors devolve into 30-line template traces.
- You only have a handful of services and a 50-line `main()` would do the job.

**Default: write the composition root by hand.** Reach for a container when the manual version is genuinely painful.

# 6. Lifetime, Ownership, and Scopes

DI raises a question OOP usually leaves vague: who owns the dependency, and how long does it live?

| Scope | Meaning | C++ realization |
|---|---|---|
| Singleton | One instance for the program | Static in composition root, or `Meyers' singleton` |
| Per-thread | One per thread | `thread_local` in factory |
| Per-request | New instance per logical operation | Constructed in the request handler |
| Transient | New instance every injection | Factory function injected instead |

A common bug: injecting a `Database&` into a long-lived service and then having the database go out of scope in `main` before the service does. **Lifetime ordering is a real architectural decision** — write it down.

# 7. Testing Benefits

The point of all this is that tests can substitute fakes:

```cpp
struct FakeDb : Database {
    std::vector<Order> saved;
    void save(const Order& o) override { saved.push_back(o); }
};
struct NullEmailer : Emailer { void sendConfirmation(const std::string&) override {} };

TEST(OrderService, savesOrder) {
    FakeDb db; NullEmailer em;
    OrderService svc{&db, &em};
    svc.placeOrder(Order{42, "foo@bar"});
    EXPECT_EQ(db.saved.size(), 1u);
}
```

No mocks framework required for simple cases. Hand-rolled fakes are usually clearer than mock expectations. See [Testing Strategies](testing_strategies.md) and [Designing for Testability](designing_for_testability.md).

# 8. Anti-patterns

**Service Locator** — a global registry queried at runtime.
```cpp
auto db = ServiceLocator::get<Database>();   // dependencies invisible at the call site
```
Looks like DI but isn't: dependencies are no longer explicit in the type. Avoid except as a last-resort migration tool.

**Static / global singletons.** Same problem; impossible to substitute in tests, racy at shutdown.

**`new` in constructors.**
```cpp
OrderService() : db_(new MySqlDatabase{}) {}   // hard-coded again
```
You've moved the dependency from a member to a heap allocation but kept all the coupling.

**Injecting too much.** A class taking 12 dependencies is telling you it has too many responsibilities. Refactor to smaller collaborators.

**Injecting only for tests.** If a parameter exists "for testability" but is always the same in production, the abstraction is paying for itself only in tests. That can still be the right call — but be honest about it.

# References

- [SOLID Principles](solid_principles.md)
- [Designing for Testability](designing_for_testability.md)
- [Testing Strategies](testing_strategies.md)
- [Boost.DI documentation](https://boost-ext.github.io/di/)
- *Dependency Injection Principles, Practices, and Patterns*, Mark Seemann.
