Below is a **practical, interview-ready mental model** for
`std::optional`, `std::variant`, and `std::any` — focused on **avoiding null pointers** and **expressing absence safely**.

No fluff, no emojis, straight engineering trade-offs.

---

## 1. The core problem they solve

Classic C++ problems:

❌ Returning `nullptr`
❌ Returning invalid sentinel values
❌ Output parameters
❌ Boolean + output value patterns
❌ Implicit assumptions (“this pointer might be null”)

Modern C++ replaces these with **type-level guarantees**.

---

## 2. `std::optional<T>`

### What it expresses

> **A value may or may not exist**

### Think of it as

> A pointer without ownership, allocation, or lifetime issues

### Typical use cases

* Function may fail to produce a value
* Cache lookup
* Optional configuration
* Parsing results
* Lazy initialization

### Example

```cpp
std::optional<int> parseInt(const std::string& s) {
    try {
        return std::stoi(s);
    } catch (...) {
        return std::nullopt;
    }
}
```

Usage:

```cpp
if (auto v = parseInt("42")) {
    std::cout << *v;
}
```

### Why it’s better than a pointer

❌ `int*` → who owns it? is it null?
✅ `std::optional<int>` → explicit absence

### Key properties

* No heap allocation
* Value stored inline
* Checked access
* Forces caller to acknowledge absence

### When NOT to use

* When absence is **impossible**
* When you need multiple alternative types
* When lifetime must be shared

---

## 3. `std::variant<Ts...>`

### What it expresses

> **Exactly one of several known types**

### Think of it as

> A type-safe tagged union

### Typical use cases

* State machines
* Message passing
* AST nodes
* API results with multiple valid forms
* Replacing base-class polymorphism when behavior is simple

### Example

```cpp
using Result = std::variant<int, std::string>;

Result r = 42;
```

Usage:

```cpp
std::visit([](auto&& v) {
    std::cout << v;
}, r);
```

### Expressing absence

```cpp
using Result = std::variant<std::monostate, int>;
```

But:
❌ worse than `std::optional<int>`
✅ use `optional` unless absence is one of many states

### Why it’s better than inheritance

❌ virtual dispatch
❌ heap allocation
❌ fragile base classes

✅ compile-time exhaustiveness
✅ no RTTI
✅ no allocation

### Key properties

* One active alternative at a time
* Known set of types
* Compile-time checked handling

### When NOT to use

* Open-ended types
* Plugin systems
* When type list is not stable

---

## 4. `std::any`

### What it expresses

> **Some value of unknown type**

### Think of it as

> Type-erased storage with runtime type checking

### Typical use cases

* Framework internals
* Plugin APIs
* Heterogeneous containers
* Metadata
* Configuration blobs

### Example

```cpp
std::any a = 42;

if (a.type() == typeid(int)) {
    int v = std::any_cast<int>(a);
}
```

### Why it’s dangerous

❌ No compile-time guarantees
❌ Runtime errors if misused
❌ Easy to abuse
❌ Slower

### Expressing absence

```cpp
std::any a;   // empty
```

But:
❌ unclear intent
❌ no documentation in the type
❌ worse than `optional`

### Rule of thumb

> If you think you need `std::any`, question the design first

---

## 5. Comparison table

| Feature             | `optional<T>` | `variant<Ts...>` | `any`      |
| ------------------- | ------------- | ---------------- | ---------- |
| Expresses absence   | YES           | Indirectly       | YES (weak) |
| Type safety         | Strong        | Strong           | Weak       |
| Compile-time checks | YES           | YES              | NO         |
| Heap allocation     | NO            | NO               | Maybe      |
| Intent clarity      | Excellent     | Excellent        | Poor       |
| Runtime overhead    | Minimal       | Minimal          | Higher     |
| Best for APIs       | YES           | YES              | Rarely     |

---

## 6. Avoiding null pointers — best practices

### ❌ Old style

```cpp
Foo* findFoo();
```

### ✅ Modern C++

```cpp
std::optional<Foo> findFoo();
```

or if ownership matters:

```cpp
std::unique_ptr<Foo> findFoo();
```

### Reference rule

If something **must exist**, use:

```cpp
Foo&
```

If it **might not exist**, use:

```cpp
std::optional<std::reference_wrapper<Foo>>
```

---

## 7. Decision guide (memorize this)

### Ask yourself:

1. **Can it be absent?**
   → `std::optional<T>`

2. **Is it one of a fixed set of types?**
   → `std::variant<Ts...>`

3. **Is the type unknown or open-ended?**
   → `std::any` (last resort)

---

## 8. Interview-grade sound bites

You can safely say:

* “`std::optional` encodes absence in the type system.”
* “`std::variant` gives compile-time checked alternatives without polymorphism.”
* “`std::any` trades safety for flexibility and should be isolated.”
* “Null pointers express failure poorly; modern C++ prefers semantic types.”

---

