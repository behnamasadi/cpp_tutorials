# Virtual functions, abstract classes, and the vtable

C++ has two forms of polymorphism:

1. **Static (compile-time)** — templates, overload resolution. Dispatch is fixed when the program is compiled.
2. **Dynamic (run-time)** — virtual functions. Dispatch is resolved at the moment of the call, from the **actual type** of the object the pointer or reference points to.

This page covers the dynamic form: declaring virtual functions, the override/final keywords, abstract classes, virtual destructors (the most commonly forgotten one), and the vtable/vptr mechanism that makes it all work.

- [1. Declaring a virtual function](#1-declaring-a-virtual-function)
- [2. `override` and `final`](#2-override-and-final)
- [3. Pure virtual functions and abstract classes](#3-pure-virtual-functions-and-abstract-classes)
- [4. Virtual destructors](#4-virtual-destructors)
- [5. Why there is no virtual constructor](#5-why-there-is-no-virtual-constructor)
- [6. The vtable and the vptr](#6-the-vtable-and-the-vptr)
- [7. Cost and when to avoid it](#7-cost-and-when-to-avoid-it)

---

## 1. Declaring a virtual function

A function declared `virtual` in a base class can be **overridden** in a derived class. When you call it through a base-class pointer or reference, the actual implementation is chosen by the dynamic type of the object — not the static type of the pointer.

```cpp
struct Sensor {
    virtual double read() { return 0.0; }     // base-class default
    virtual ~Sensor() = default;              // see §4 — always do this
};

struct ImuSensor : Sensor {
    double read() override { return imu_.acceleration_z(); }
private:
    Imu imu_;
};

void log_one(Sensor& s) {
    std::cout << s.read();                     // dispatches at run time
}

ImuSensor imu;
log_one(imu);                                  // calls ImuSensor::read
```

A few mechanics worth being explicit about:

- The `virtual` keyword is required on the **first declaration** in the base. In the derived class it's optional (the function is virtual by virtue of having the same signature as a virtual base function) — but `override` (§2) is required by good practice.
- Overrides don't need to repeat the parameter defaults of the base. In fact, default arguments are looked up by the **static** type, so giving the override different defaults silently produces bugs:
  ```cpp
  struct Base   { virtual void f(int x = 1) { std::cout << x; } };
  struct Deriv  : Base { void f(int x = 2) override { std::cout << x; } };

  Deriv d;
  Base& b = d;
  b.f();          // prints 1 — default from Base, dispatch to Deriv
  ```

## 2. `override` and `final`

The contextual keywords `override` and `final` (C++11) protect you from silent bugs in the override mechanism.

**`override`** asks the compiler to verify that this function actually overrides a base-class virtual:

```cpp
struct Base {
    virtual void update(double dt);
};

struct Child : Base {
    void update(float dt) override;   // compile error — signature mismatch (float vs double)
};
```

Without `override`, that mismatch would silently introduce a *new* function rather than override the base one — and you'd quietly call the wrong implementation forever. Always write `override`.

**`final`** prevents further overrides (on a method) or further inheritance (on a class):

```cpp
struct Base { virtual void tick(); };
struct Middle : Base { void tick() override final; };   // no class derived from Middle may override tick
struct Leaf final : Middle {};                          // no class may inherit from Leaf
```

`final` is a contract for readers ("this isn't an extension point") and an optimization hint — the compiler can devirtualize calls to a `final` method when it can prove the type.

**Style rule.** Each function declaration should specify **exactly one** of `virtual` / `override` / `final`:

- `virtual` — the first appearance in the base.
- `override` — overrides of a base's virtual in a derived class.
- `final` — overrides that mustn't be further overridden.

## 3. Pure virtual functions and abstract classes

A **pure virtual function** has `= 0` instead of a body:

```cpp
struct Sensor {
    virtual double read() = 0;            // pure virtual — no default implementation
    virtual ~Sensor() = default;
};
```

A class with any pure virtual function is **abstract** — you can't instantiate it directly:

```cpp
Sensor s;        // ❌ error: cannot construct abstract class
```

Derived classes must override every pure virtual to become concrete:

```cpp
struct Imu : Sensor {
    double read() override { return acceleration_z(); }
};

Imu imu;         // ✅ ok
```

### Making a class abstract without any meaningful pure virtuals

If you want a class to be abstract just to prevent direct instantiation, declare a pure virtual destructor **and give it a definition**:

```cpp
// header
class Component {
public:
    virtual ~Component() = 0;
};

// implementation file
Component::~Component() = default;
```

This is the rare case where a pure virtual function gets a body — derived destructors will call it during destruction.

## 4. Virtual destructors

The most commonly forgotten virtual function — and the one most likely to bite you. If you ever delete a derived object through a base pointer, the base destructor **must** be virtual:

```cpp
struct Base {
    ~Base() { /* non-virtual */ }
};
struct Derived : Base {
    Derived()  { buf_ = new char[1024]; }
    ~Derived() { delete[] buf_; }
    char* buf_;
};

Base* p = new Derived();
delete p;       // ⚠️ calls Base::~Base only — leaks Derived::buf_; UB by §3 of the standard
```

Fix:

```cpp
struct Base {
    virtual ~Base() = default;     // ✅
};
```

Now `delete p` invokes `Derived::~Derived()` first (which frees `buf_`), then `Base::~Base()`. Destruction order is always the reverse of construction:

```
construction:    Base()    → Derived()
destruction:     ~Derived() → ~Base()
```

**Rule of thumb.** Any class intended to be used polymorphically (anyone might `delete` it through a base pointer, or hold it via `std::unique_ptr<Base>`) needs a virtual destructor. If you don't intend the class to be a base, mark it `final` instead — then non-virtual destruction is safe and slightly cheaper.

Stack-allocated objects don't trigger this bug:

```cpp
Derived d;         // destructor of Derived runs at end of scope regardless
```

…because there's no base pointer involved. The bug appears only when destruction is *dispatched* through a base reference/pointer.

## 5. Why there is no virtual constructor

A constructor builds a specific type — to call it, the type must already be known. You can't ask "construct whatever this pointer's dynamic type would be" because there is no object yet, no vtable to consult.

The standard pattern for polymorphic creation is a **virtual clone** method (essentially a copy constructor that *is* virtual):

```cpp
struct Shape {
    virtual ~Shape() = default;
    virtual std::unique_ptr<Shape> clone() const = 0;
};

struct Circle : Shape {
    std::unique_ptr<Shape> clone() const override {
        return std::make_unique<Circle>(*this);    // calls Circle's copy ctor
    }
};

std::unique_ptr<Shape> a = std::make_unique<Circle>();
auto b = a->clone();   // dynamic type: Circle, even though a is Shape*
```

This is the "virtual constructor idiom" — note the constructor itself is **not** virtual; the dispatch happens through a `clone()` method that knows its own derived type.

## 6. The vtable and the vptr

Dynamic dispatch is implemented with two compiler-generated pieces of data:

- **Vtable** — a per-class table of function pointers, one entry per virtual method. Built at compile time, lives in `.rodata` (or `.data.rel.ro`).
- **Vptr** — a hidden pointer member added to every object of a class with virtual functions. Points to that class's vtable. Initialized by each constructor; updated on destruction.

```cpp
struct Base {
    virtual void foo();
    virtual void bar();
    void plain();                  // non-virtual — no vtable slot
};

struct Derived : Base {
    void foo() override;           // overrides Base::foo
    // bar inherited from Base
    void quux();                   // non-virtual addition
};
```

The layout looks roughly like this:

```
  Base object              vtable for Base
  ┌─────────────┐          ┌────────────────────────────┐
  │  vptr ──────┼─────────►│ &Base::foo                 │
  │  members…   │          │ &Base::bar                 │
  └─────────────┘          └────────────────────────────┘

  Derived object           vtable for Derived
  ┌─────────────┐          ┌────────────────────────────┐
  │  vptr ──────┼─────────►│ &Derived::foo              │ (overridden)
  │  Base bits  │          │ &Base::bar                 │ (inherited slot)
  │  Derived bits│         └────────────────────────────┘
  └─────────────┘
```

A call `p->foo()` through a `Base*` becomes, in pseudo-asm:

```
load   vptr           from *p
load   target = vptr[index_of_foo]   ; compile-time constant index
call   target
```

Two extra memory loads versus a non-virtual call. The vtable index of `foo` is fixed at compile time per class layout — that's why you can't add or remove virtual functions without recompiling everything that derives from the class (the **fragile base class** problem in ABI design).

Non-virtual methods like `plain()` are not in the vtable; the compiler emits a direct call to them, statically resolved.

### Single-inheritance details

- The vptr is typically the **first** word of the object (so the compiler knows its offset without consulting the type).
- One vtable per class (not per object). All `Base` instances share the same vtable; all `Derived` instances share theirs.
- The vptr is set by the **constructor** of the current class. During `Base`'s constructor body, the vptr points to `Base`'s vtable — even if you're constructing a `Derived`. That's why calling a virtual function inside a base-class constructor dispatches to the base version, not the derived one (a common surprise).

### Multiple inheritance, virtual inheritance, RTTI

Things get more interesting:

- **Multiple inheritance** means multiple vptrs per object (one per base subobject), and `thunks` to adjust `this` when crossing inheritance boundaries.
- **Virtual inheritance** (the `virtual` keyword on a base specifier, used to solve the diamond problem) adds a vtable pointer to the virtual base subobject so its layout can be shared.
- **RTTI** (`typeid`, `dynamic_cast`) reuses the vtable: every vtable carries a pointer to a `std::type_info` describing the class. See [RTTI.md](RTTI.md).

The exact layout is implementation-defined and ABI-specific — Itanium ABI (Linux, macOS) and the MSVC ABI differ in details, which is why mixing object files from different compilers across class boundaries is unsafe.

## 7. Cost and when to avoid it

A virtual call costs:

- Two extra dependent loads (vptr, then vtable slot) — typically L1 hits, so ~3–5 cycles uncontended.
- An indirect branch — modern branch predictors are good but not perfect; a single mispredict on a hot path can dwarf the load cost.
- Lost inlining — the compiler can't inline through an indirect call, so the entire body of the function stays a black box. On small methods this is the biggest cost by far.

For most code this is invisible. For tight inner loops (a 1 kHz control loop calling `read()` on a polymorphic sensor 1000× per cycle) it adds up. Three escape hatches when you've measured a problem:

- **`final` on the method or class.** The compiler can prove no further override exists and devirtualize — turning the call into a direct one, opening it to inlining.
- **CRTP (Curiously Recurring Template Pattern).** Replace runtime polymorphism with compile-time polymorphism. See [src/CRTP.cpp](../src/CRTP.cpp).
- **`std::variant` with `std::visit`.** Closed set of types known at compile time, no heap allocation, no virtual calls. See [std_variant.md](std_variant.md).

Don't preemptively optimize away virtual functions — the ergonomics of OO dispatch are usually worth the indirection. Profile first; reach for these tools when the cost is real.

[virtual function source](../src/class/virtual_function_abstract_class.cpp), [virtual destructor source](../src/class/virtual_destructor_virtual_constructor.cpp), [VTABLE source](../src/VTABLE_and_VPTR.cpp)
