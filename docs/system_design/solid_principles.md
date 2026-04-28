# SOLID Principles in C++

SOLID is five design principles for object-oriented software, popularized by Robert C. Martin. They are guidelines, not laws — but most "this code is hard to change" complaints map to violations of one of them.

- [1. Single Responsibility Principle (SRP)](#1-single-responsibility-principle-srp)
- [2. Open/Closed Principle (OCP)](#2-openclosed-principle-ocp)
- [3. Liskov Substitution Principle (LSP)](#3-liskov-substitution-principle-lsp)
- [4. Interface Segregation Principle (ISP)](#4-interface-segregation-principle-isp)
- [5. Dependency Inversion Principle (DIP)](#5-dependency-inversion-principle-dip)
- [6. SOLID and Modern C++](#6-solid-and-modern-c)

---

# 1. Single Responsibility Principle (SRP)

> A class should have one, and only one, reason to change.

"Reason to change" means *who* asks for the change — a stakeholder, a subsystem, a layer. If billing rules and PDF formatting are bundled in one class, then both finance and the design team can break it.

```cpp
// Bad — Invoice does business logic, persistence, and rendering.
class Invoice {
public:
    double total() const;
    void saveToDB();
    void renderPDF(std::ostream&);
};

// Better — split by reason to change.
class Invoice { public: double total() const; };
class InvoiceRepository { public: void save(const Invoice&); };
class InvoicePdfRenderer  { public: void render(const Invoice&, std::ostream&); };
```

Symptom of an SRP violation: any small feature change touches the same class repeatedly, but for unrelated reasons.

# 2. Open/Closed Principle (OCP)

> Software entities should be open for extension, but closed for modification.

You should be able to add new behavior without editing existing, tested code. In C++ this usually means polymorphism (virtual functions, templates, or `std::variant` + `std::visit`).

```cpp
// Closed for modification: Shape doesn't need to change when we add new shapes.
struct Shape { virtual double area() const = 0; virtual ~Shape() = default; };
struct Circle : Shape { double r; double area() const override { return M_PI*r*r; } };
struct Square : Shape { double s; double area() const override { return s*s; } };

double totalArea(const std::vector<std::unique_ptr<Shape>>& shapes) {
    double sum = 0;
    for (const auto& s : shapes) sum += s->area();
    return sum;
}
```

The compile-time alternative (template strategy or `std::variant`) trades runtime polymorphism for inlining and zero vtable cost. Pick the axis (compile-time vs runtime) based on whether the set of types is closed.

# 3. Liskov Substitution Principle (LSP)

> Subtypes must be substitutable for their base types.

If `Derived` is a `Base`, code written against `Base&` must keep working without surprises when handed a `Derived&`. The classic violation: `Square` inheriting from `Rectangle` and breaking the invariant `setWidth(w); setHeight(h);` ⇒ `width()==w && height()==h`.

Concrete LSP red flags:
- Override **strengthens preconditions** (rejects inputs the base accepted).
- Override **weakens postconditions** (returns less than the base promised).
- Override **throws new exception types** the base didn't declare.
- Override **changes observable side effects** (e.g., `Sorted::push_back` reorders).

If you find yourself writing `if (auto* d = dynamic_cast<Derived*>(b)) { /* special case */ }`, you're patching an LSP violation at the call site.

# 4. Interface Segregation Principle (ISP)

> Clients should not be forced to depend on methods they do not use.

Fat interfaces couple unrelated clients. Splitting them lets each consumer depend on the smallest surface they need, which also reduces recompilation and test scope.

```cpp
// Bad — Printer forces every client to know about scanning and faxing.
struct AllInOne {
    virtual void print(const Doc&) = 0;
    virtual void scan(Doc&)        = 0;
    virtual void fax(const Doc&)   = 0;
};

// Better — segregate by role.
struct Printer { virtual void print(const Doc&) = 0; virtual ~Printer() = default; };
struct Scanner { virtual void scan(Doc&) = 0;        virtual ~Scanner() = default; };
struct Fax     { virtual void fax(const Doc&) = 0;   virtual ~Fax() = default; };
```

A class that *is* an all-in-one device just inherits from all three. Clients depend only on the role they use.

# 5. Dependency Inversion Principle (DIP)

> Depend on abstractions, not on concretions. High-level modules should not depend on low-level modules.

Concretely: don't hard-code `MySqlConnection` inside your business logic. Take a `Database&` (or any abstraction representing what you actually need) and let the caller wire in the implementation.

```cpp
struct Clock { virtual std::chrono::system_clock::time_point now() const = 0; virtual ~Clock() = default; };

class TokenIssuer {
    Clock& clock_;
public:
    explicit TokenIssuer(Clock& c) : clock_(c) {}
    Token issue() { /* uses clock_.now() */ }
};
```

Now `TokenIssuer` is testable with a `FakeClock` and reusable with any clock source. See [Dependency Injection](dependency_injection.md) for wiring strategies.

# 6. SOLID and Modern C++

SOLID was formulated for class-based OOP. Modern C++ has more tools, and applying SOLID dogmatically can produce vtable-heavy designs that miss zero-cost abstractions.

- **Templates and concepts** give you OCP and DIP without virtual dispatch. A function templated on `Clock` (a concept) substitutes a different clock at compile time.
- **`std::variant` + `std::visit`** gives you closed-set polymorphism with no heap and no vtable.
- **Type erasure** (`std::function`, `std::any`, hand-rolled) gives you DIP without the caller seeing a base class.
- **Free functions and ADL** often satisfy SRP better than splitting one class into three.
- **Composition over inheritance** — most LSP and ISP problems disappear if classes hold strategies as members instead of inheriting them.

The test for whether a SOLID-flavored refactor was worth it: did the next change actually become easier? If not, you may have added abstraction for its own sake.

# References

- *Agile Software Development: Principles, Patterns, and Practices*, Robert C. Martin.
- *C++ Software Design*, Klaus Iglberger — modern (C++17/20) take on SOLID using value semantics, variants, and type erasure.
- [isocpp.org Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
