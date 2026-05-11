# Casting Between Base and Derived

- [Polymorphic types](#polymorphic-types)
- [Upcast: Derived → Base](#upcast-derived--base)
  - [`static_cast` upcast](#static_cast-upcast)
- [Downcast: Base → Derived](#downcast-base--derived)
  - [`static_cast` downcast (unchecked)](#static_cast-downcast-unchecked)
  - [`dynamic_cast` downcast (checked)](#dynamic_cast-downcast-checked)
- [Casting smart pointers](#casting-smart-pointers)
- [Robotics example: dispatching on sensor type](#robotics-example-dispatching-on-sensor-type)
- [References](#references)

## Polymorphic types

A class that **declares or inherits a virtual function** is called a *polymorphic class*.
`dynamic_cast` is a run-time cast, so the classes involved must be polymorphic for the
down-cast to be legal. Concretely: the base class must contain at least one `virtual`
member (a `virtual` destructor is the usual choice, since you almost always want one in
a polymorphic hierarchy anyway).

At run time, `dynamic_cast` queries the object's RTTI to ask "are you really the type
I'm asking for?". If yes, `dynamic_cast<T*>` returns the pointer; if no, it returns
`nullptr` (for pointers) or throws `std::bad_cast` (for references).

If your classes are **not** polymorphic, a base-to-derived `dynamic_cast` will fail to
compile.

## Upcast: Derived → Base

Casting up the hierarchy is always safe — every `Derived` object *is-a* `Base`. You
rarely need to write the cast explicitly; the conversion is implicit.

### `static_cast` upcast

```cpp
#include <iostream>

namespace staticcast {
    class Base {
    public:
        void whoAmI() { std::cout << "I'm Base\n"; }
    };
    class Derived : public Base {
    public:
        void whoAmI() { std::cout << "I'm Derived\n"; }
    };
    class anOtherClass : public Base {};
}

void staticcastD2B()
{
    staticcast::Derived* d = new staticcast::Derived;
    d->whoAmI();

    // OK: Derived* -> Base* is a safe upcast.
    staticcast::Base* b = static_cast<staticcast::Base*>(d);
    b->whoAmI();

    // Won't compile: Derived and anOtherClass are unrelated siblings.
    // staticcast::anOtherClass* x = static_cast<staticcast::anOtherClass*>(d);

    delete d;
}
```

Note that `whoAmI()` is **not** virtual here, so `b->whoAmI()` resolves statically to
`Base::whoAmI()` even though `b` points to a `Derived`. If you want runtime dispatch,
mark the function `virtual`.

## Downcast: Base → Derived

Down-casts are dangerous: the actual object might not be the derived type you're
assuming. C++ offers two options with very different guarantees.

### `static_cast` downcast (unchecked)

`static_cast` does **no** runtime check. If you cast a `Base*` to a `Derived*` and the
pointee isn't really a `Derived`, you get undefined behavior the moment you use the
result. Use this only when you can prove the type from context (e.g. a CRTP pattern
or a tag you've already inspected).

### `dynamic_cast` downcast (checked)

`dynamic_cast` verifies the runtime type and yields `nullptr` on failure. The cost is
an RTTI lookup, but in exchange you get a safe, observable failure mode.

```cpp
#include <iostream>

namespace dynamiccast {
    class Base {
        virtual void foo() {}  // makes Base polymorphic; private is fine
    public:
        virtual ~Base() = default;
        void whoAmI() { std::cout << "I'm Base\n"; }
    };
    class Derived : public Base {
    public:
        void whoAmI() { std::cout << "I'm Derived\n"; }
    };
    class anOtherClass : public Base {
    public:
        void whoAmI() { std::cout << "I'm anOtherClass\n"; }
    };
}

void dynamiccastD2B()
{
    dynamiccast::Derived* d = new dynamiccast::Derived;
    d->whoAmI();

    // OK: upcast.
    dynamiccast::Base* b = dynamic_cast<dynamiccast::Base*>(d);
    b->whoAmI();

    // Compiles, but the cross-cast fails at runtime: d is not an anOtherClass.
    dynamiccast::anOtherClass* x = dynamic_cast<dynamiccast::anOtherClass*>(d);
    std::cout << (x == nullptr
                  ? "x is nullptr\n"
                  : "x has been cast to anOtherClass\n");

    delete d;
}
```

Round-tripping **Base → Derived → Base** with `dynamic_cast`:

```cpp
#include <cstdlib>
#include <iostream>

dynamiccast::Base* CreateRandom()
{
    if ((std::rand() % 2) == 0)
        return new dynamiccast::Derived;
    else
        return new dynamiccast::anOtherClass;
}

void dynamiccastB2D()
{
    for (int n = 0; n < 10; ++n) {
        dynamiccast::Base* base = CreateRandom();

        auto* anOtherClassObject = dynamic_cast<dynamiccast::anOtherClass*>(base);
        auto* derivedObject      = dynamic_cast<dynamiccast::Derived*>(base);

        std::cout << "================== n is: " << n << " =================\n";
        std::cout << (anOtherClassObject == nullptr
                      ? "anOtherClassObject is nullptr\n"
                      : "base has been cast to anOtherClassObject\n");
        std::cout << (derivedObject == nullptr
                      ? "derivedObject is nullptr\n"
                      : "base has been cast to derivedObject\n");

        delete base;
    }
}

int main()
{
    // staticcastD2B();
    // dynamiccastD2B();
    dynamiccastB2D();
}
```

Exactly one of the two `dynamic_cast`s succeeds on each iteration — the other returns
`nullptr` — because `Derived` and `anOtherClass` are sibling types under `Base`.

## Casting smart pointers

Raw-pointer casts don't compose with `std::shared_ptr` — you can't `static_cast` a
`shared_ptr<Base>` to a `shared_ptr<Derived>` because they're unrelated template
instantiations. The standard library provides dedicated helpers that share ownership
of the underlying control block:

- `std::static_pointer_cast<T>` — analogue of `static_cast`, unchecked
- `std::dynamic_pointer_cast<T>` — analogue of `dynamic_cast`, returns an empty
  `shared_ptr` on failure
- `std::const_pointer_cast<T>` / `std::reinterpret_pointer_cast<T>`

```cpp
#include <iostream>
#include <memory>

struct Base {
    int a = 0;
    virtual void whoAmI() const { std::cout << "I am base!\n"; }
    virtual ~Base() = default;
};

struct Derived : Base {
    void whoAmI() const override { std::cout << "I am derived!\n"; }
};

void smartPointerBaseToDerived()
{
    std::shared_ptr<Base> ptr_to_base = std::make_shared<Derived>();

    // ptr_to_base->whoAmI() resolves virtually to Derived::whoAmI(),
    // but if whoAmI() were Derived-only you'd need to cast first.

    // Shares ownership; constructs a temporary shared_ptr<Derived>.
    std::static_pointer_cast<Derived>(ptr_to_base)->whoAmI();

    // Same dispatch, but no extra shared_ptr is created.
    static_cast<Derived*>(ptr_to_base.get())->whoAmI();

    // Checked variant — yields an empty shared_ptr if the runtime type is wrong.
    if (auto d = std::dynamic_pointer_cast<Derived>(ptr_to_base)) {
        d->whoAmI();
    }
}
```

The raw-pointer form (`static_cast<Derived*>(ptr_to_base.get())`) is fine for a single
call where ownership doesn't leave the scope, but **never** wrap that raw pointer in a
new `shared_ptr` — you'd end up with two independent control blocks and a double
delete.

## Robotics example: dispatching on sensor type

A common situation: the driver layer exposes a heterogeneous list of `Sensor*`
handles, but a particular consumer (say, a SLAM front-end) only knows what to do with
a `LidarSensor`. `dynamic_cast` is the safe way to filter:

```cpp
#include <iostream>
#include <vector>
#include <memory>

struct Sensor {
    virtual ~Sensor() = default;
    virtual void poll() = 0;
};

struct LidarSensor : Sensor {
    void poll() override { /* read 360° scan from CAN */ }
    std::vector<float> latestScan() const { return {0.0f, 1.2f, 2.3f}; }
};

struct ImuSensor : Sensor {
    void poll() override { /* read IMU over SPI */ }
};

struct GpsSensor : Sensor {
    void poll() override { /* read NMEA over serial */ }
};

void feedSlam(const std::vector<std::unique_ptr<Sensor>>& sensors)
{
    for (const auto& s : sensors) {
        s->poll();
        if (auto* lidar = dynamic_cast<LidarSensor*>(s.get())) {
            auto scan = lidar->latestScan();
            std::cout << "feeding SLAM with " << scan.size() << " ranges\n";
        }
        // IMU / GPS handles are ignored here — they go through different consumers.
    }
}
```

Failure modes to keep in mind on a real robot:

- **Forgetting `virtual ~Sensor()`** — deleting through `Sensor*` then leaks (or worse,
  corrupts the heap) for any derived type that owns resources.
- **Hot loops** — `dynamic_cast` is not free; if you do this every control cycle at
  1 kHz, prefer a tagged enum or a visitor pattern. Cast once at registration time,
  store the typed pointer.
- **References vs. pointers** — `dynamic_cast<LidarSensor&>(*s)` throws `std::bad_cast`
  on mismatch. Catch it, or use the pointer form and test for `nullptr`.

## References

1. [`std::dynamic_pointer_cast` — cppreference](https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast)
2. [`std::dynamic_pointer_cast` — bccnsoft mirror](https://doc.bccnsoft.com/docs/cppreference2015/en/cpp/memory/shared_ptr/pointer_cast.html)
3. [Downcasting `shared_ptr<Base>` to `shared_ptr<Derived>` — Stack Overflow](https://stackoverflow.com/questions/1358143/downcasting-shared-ptrbase-to-shared-ptrderived)
