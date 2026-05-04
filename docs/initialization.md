# C++ Initialization

## 1. Constructor Member Initializer Lists

Used **only inside constructors** to initialize data members **before** the constructor body executes.

```cpp
class Foo {
  int b;
  double c;
public:
  Foo(int i, double j) : b(i), c(j) {}
};
```

### Why use an initializer list at all? (vs. assignment in the body)

A reasonable question: why write

```cpp
Foo(int i, double j) : b(i), c(j) {}
```

instead of just

```cpp
Foo(int i, double j) { b = i; c = j; }
```

The body version looks simpler — but it's either *wrong*, *slower*, or *illegal* depending on what `b` and `c` are. The reason traces back to one fact:

> Members are **always** initialized before the constructor body runs. That phase is not optional. The initializer list is just the syntax for telling the compiler what value to use during that phase.

If you skip the initializer list, the compiler synthesises an empty one — every member gets default‑initialized first, and *then* your body runs assignments on already‑constructed objects. That gives you five concrete problems.

#### Reason 1 — `const` and reference members: body version won't compile

```cpp
class Controller {
  const double m_kp;
  Logger&      m_logger;
public:
  Controller(double kp, Logger& logger) {
    m_kp     = kp;       // error: assignment of read-only member
    m_logger = logger;   // error: 'operator=' is implicitly deleted (reference)
  }
};
```

`const` members and references can be set exactly once — at construction. They have no assignment operator. The initializer list is the **only** legal place to give them a value:

```cpp
Controller(double kp, Logger& logger) : m_kp(kp), m_logger(logger) {}
```

#### Reason 2 — members without a default constructor

If a member type has no default constructor, the compiler can't default‑construct it before your body runs:

```cpp
class Mutex {                                   // no default ctor
public:
  explicit Mutex(MutexAttr attr);
};

class RobotArm {
  Mutex m_state_mutex;
public:
  RobotArm() {
    m_state_mutex = Mutex(MutexAttr::priority_inheritance);  // error: no default ctor
  }
};
```

Same problem with types that have `=delete`d default ctors, `std::reference_wrapper`, or any type you've intentionally made non‑default‑constructible. The initializer list is again the only path:

```cpp
RobotArm() : m_state_mutex(MutexAttr::priority_inheritance) {}
```

#### Reason 3 — performance: double work for non‑trivial members

For cheap types (`int`, `double`, raw pointers) the body version costs the same — assignment is one move instruction. But for any member with a non‑trivial constructor, the body version pays for two operations:

```cpp
class Map {
  std::string           m_frame_id;
  Eigen::MatrixXd       m_occupancy;
  std::vector<Obstacle> m_obstacles;
public:
  Map(std::string frame, int rows, int cols) {
    // What actually happens:
    // 1. m_frame_id  default-constructed   (empty string)
    // 2. m_occupancy default-constructed   (0×0 matrix)
    // 3. m_obstacles default-constructed   (empty vector)
    // 4. THEN body runs:
    m_frame_id  = std::move(frame);                  // assignment, may allocate
    m_occupancy = Eigen::MatrixXd(rows, cols);       // build temporary, move-assign
    m_obstacles.reserve(1024);
  }
};
```

vs. the initializer‑list version, which constructs each member directly with the right value the first time:

```cpp
Map(std::string frame, int rows, int cols)
    : m_frame_id(std::move(frame)),
      m_occupancy(rows, cols),    // built in place, no temporary
      m_obstacles{}
{
  m_obstacles.reserve(1024);
}
```

The cost difference is small for `std::string` but real for `Eigen::MatrixXd` (allocates a heap buffer in the default ctor, then reallocates in `operator=`), `std::vector`, `std::map`, `std::function`, ROS publishers, mutexes with attributes, file handles — anything that touches the heap or the OS in its constructor.

#### Reason 4 — base class constructors require the initializer list

There is no other syntax for passing arguments to a base‑class constructor:

```cpp
class Sensor {
public:
  Sensor(const std::string& topic, double rate_hz);
};

class Lidar : public Sensor {
public:
  Lidar() {
    Sensor("/lidar", 10.0);   // does NOT call the base ctor —
                              // creates a temporary Sensor and discards it
  }
};
```

That body line compiles but does the wrong thing — it builds a throw‑away temporary. The actual `Lidar::Sensor` subobject was already default‑constructed earlier (which won't even compile if `Sensor` has no default ctor). The only correct form:

```cpp
Lidar() : Sensor("/lidar", 10.0) {}
```

#### Reason 5 — built‑in types: body version can be UB

For `int`, `double`, raw pointers, etc., the body version has a subtler trap. Between the start of the body and your assignment line, the member holds an **indeterminate value**. Reading it before writing it is undefined behaviour:

```cpp
class Counter {
  int m_count;             // no in-class initializer
public:
  Counter(int initial) {
    if (m_count < 0) {     // UB: reads indeterminate value
      m_count = 0;
    } else {
      m_count = initial;
    }
  }
};
```

With the initializer list this is impossible — `m_count` is set before the body runs:

```cpp
Counter(int initial) : m_count(initial < 0 ? 0 : initial) {}
```

(The same effect is achieved with C++11 in‑class default initializers — `int m_count = 0;` — which apply regardless of which constructor runs. See section 9.)

#### Summary

| Member kind                                | Body version                  | Initializer list |
|--------------------------------------------|-------------------------------|------------------|
| `const` or reference                       | Compile error                 | Required         |
| No default constructor                     | Compile error                 | Required         |
| Base class arguments                       | Wrong (creates temporary)     | Required         |
| Non‑trivial type (string, vector, Eigen…)  | Default‑construct + assign    | One construction |
| Built‑in (`int`, `double`, pointer)        | Indeterminate until assigned  | Set immediately  |

The body should be reserved for things that *aren't* initialization — invariant checks, logging, registering callbacks, kicking off async work. If a line in the body reads `m_x = something;`, it almost certainly belongs in the initializer list.

### Initialization order: declaration order, always

A common misconception is that the initializer list controls order, or that order is unspecified. **Both are wrong.** The C++ standard guarantees:

> Non‑static data members are initialized in the **order they are declared** in the class body, regardless of the order they appear in the constructor's initializer list. Destruction happens in the **reverse** order.

The initializer list is *how* each member is initialized, not *when*. Writing the list out of declaration order doesn't reorder anything — it just makes the code lie to the reader.

### The latent bug this causes

```cpp
template <typename T>
struct SensorBuffer {
  T*          m_data;        // declared first  → initialized first
  std::size_t m_capacity;    // declared second → initialized second

  SensorBuffer(std::size_t capacity)
      : m_capacity(capacity),
        m_data(new T[m_capacity]) {}   // BUG: reads m_capacity before it's set
};
```

The initializer list reads left‑to‑right as written, so a casual reviewer assumes `m_capacity` is assigned first. It is not. `m_data` is initialized first, using `m_capacity` while it still holds an indeterminate value, so `new T[<garbage>]` either throws `bad_alloc`, allocates a huge block, or (in release builds with optimisation) silently does the wrong thing. Pure undefined behaviour.

The fix is one of:

```cpp
// Option A — match declaration order to initializer list order
template <typename T>
struct SensorBuffer {
  std::size_t m_capacity;
  T*          m_data;
  SensorBuffer(std::size_t capacity)
      : m_capacity(capacity),
        m_data(new T[m_capacity]) {}
};

// Option B — depend on the constructor parameter, not the other member
template <typename T>
struct SensorBuffer {
  T*          m_data;
  std::size_t m_capacity;
  SensorBuffer(std::size_t capacity)
      : m_data(new T[capacity]),       // uses the parameter, no ordering trap
        m_capacity(capacity) {}
};
```

Option B is usually safer in practice — it removes the cross‑member dependency entirely.

### Compiler help

Both gcc and clang emit a warning when the initializer list order disagrees with declaration order:

```text
warning: field 'm_data' will be initialized after field 'm_capacity' [-Wreorder-ctor]
```

`-Wreorder-ctor` is part of `-Wall`. Treat it as an error (`-Werror=reorder-ctor`); it has effectively zero false positives and catches exactly this class of bug.

### Why the standard fixes the order at declaration

Two reasons. First, **destruction must be the exact reverse of construction**, and there's only one declaration order to reverse — so there must be only one construction order. Second, a class can have multiple constructors with different initializer lists; the destructor is shared, so it can't depend on which constructor ran.

### Same rule for base classes and virtual bases

* Virtual bases first (left‑to‑right in the inheritance graph).
* Then non‑virtual direct bases, in the order listed after `class X :`.
* Then non‑static data members, in declaration order.
* Then the constructor body runs.

This is also why **a base‑class constructor cannot see derived‑class state** — the derived members haven't been constructed yet, and a virtual call from the base constructor dispatches to the base version, not the override.

### Practical guidelines

1. **Match the initializer list to declaration order.** The code should read the way it executes.
2. **Don't read one member while initializing another** unless the dependency is obvious from declaration order. Prefer constructor parameters.
3. **Enable `-Wreorder-ctor` as an error.** It's already on with `-Wall`; promote it.
4. **Group declarations by initialization dependency, not by type or alignment.** If `m_data` needs `m_capacity`, declare `m_capacity` first.
5. For `const`/reference members, the initializer list is the *only* option — direct assignment in the body won't compile.

---

## 2. Aggregate Initialization (Brace Initialization)

### Definition

**Aggregate initialization** uses brace‑enclosed lists to initialize **aggregates**:

An aggregate is a type with:

* no user‑declared constructors
* no private or protected non‑static data members
* no virtual functions
* no base classes

### Example (important)

```cpp
struct S {
  int i;
};

S s{10};
```

This is **aggregate initialization**.

Conceptually equivalent to:

```cpp
S s;
s.i = 10;
```

but performed **during initialization**, not assignment.

### Copy‑list form

```cpp
S s = {10};
```

Still aggregate initialization.

### Narrowing protection

```cpp
S s{3.14}; // compile‑time error
```

---

## 3. Copy Initialization

Initializes an object **from another object or expression** using `=`.

```cpp
int a = 5;
int b = a;
```

Triggers copy or conversion constructors when applicable.

Also occurs when:

```cpp
void f(Foo x);
f(obj);          // copy or move

Foo make();
Foo x = make();  // copy elided or moved
```

---

## 4. Direct Initialization

Initializes an object **directly**, usually via a constructor.

```cpp
Foo f(10);
std::string s("abc");
int x(5);
```

---

## 5. Default Initialization

Occurs when **no initializer is provided**.

```cpp
int x;        // indeterminate value
Foo f;        // calls default constructor (if any)
new int;      // indeterminate
```

For built‑in types: **no initialization**.

---

## 6. Value Initialization

Forces **zero‑initialization** for fundamental types.

```cpp
int x{};        // zero
int y = int(); // zero
Foo f{};       // default constructor
```

Common interview pitfall: `T()` ≠ default initialization.

---

## 7. Zero Initialization

Automatically applied in specific contexts:

* static storage duration
* thread‑local storage

```cpp
static int x; // zero
```

Also part of value initialization.

---

## 8. Uniform / List Initialization (C++11)

General brace‑based syntax applicable to **everything**.

```cpp
int a{42};
std::vector<int> v{1,2,3};
MyClass m{1, 3.14};
```

Benefits:

* prevents narrowing
* uniform syntax
* works for aggregates and constructors

---

## 9. In‑Class Member Initializers

Provides default values **at declaration site**.

```cpp
class MyClass {
  int x = 10;
  double y{3.14};
};
```

Used if constructor does not override them.

---

## 10. initializer_list (Not Aggregate Initialization)

`std::initializer_list` enables **custom brace handling**.

```cpp
class IntArray {
  int size;
  int* data;
public:
  IntArray(std::initializer_list<int> list)
    : size(list.size()), data(new int[size]) {
      int i = 0;
      for (int v : list) data[i++] = v;
  }
};

IntArray a{1,2,3,4};
```

This is **list initialization via constructor**, not aggregate initialization.

---

## Mental Model (Interview Gold)

| Syntax | Meaning |
|------|--------|
| `T x;` | default initialization |
| `T x{};` | value initialization |
| `T x = y;` | copy initialization |
| `T x(y);` | direct initialization |
| `T x{y};` | list initialization (aggregate or ctor) |

---

## Key Takeaway

```cpp
S s{10};
```

is called:

* **brace initialization**
* specifically **aggregate initialization**

when `S` is an aggregate.

---

References:
* cppreference — initialization
* modernescpp.com
* C++ Core Guidelines
