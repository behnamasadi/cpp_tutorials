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

### Why initializer lists matter

* Required for `const` and reference members
* Required for members without default constructors
* Avoids default‑construction + assignment
* Initialization order follows **member declaration order**, not list order

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
