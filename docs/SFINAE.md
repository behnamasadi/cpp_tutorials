## 1. What problem are we solving?

We want this behavior:

```cpp
divide(10, 2);          // OK
divide(10.0, 2.0);     // OK
divide(std::string{}, std::string{});   // ❌ not allowed
```

But **C++ templates are instantiated only when used**, so we need a way to:

* **Enable** a function for some types
* **Disable** it for others
* **Without producing a compilation error**

This is exactly what **SFINAE** does.

---

## 2. What is SFINAE?

**SFINAE** means:

> **Substitution Failure Is Not An Error**

When the compiler tries to substitute a template type, and that substitution **fails**, the compiler:

* ❌ does **not** error
* ✅ simply **removes that function from overload resolution**

So the function **silently disappears**.

---

## 3. Detecting whether a type supports division

We first ask:

> “Can I write `a / b` for this type?”

### Step 3.1 – Primary template (assume NO division)

```cpp
template <typename T, typename = void>
struct has_division : std::false_type {};
```

This says:

* By default, `T` **does not** support division

---

### Step 3.2 – Specialization (only exists if `T / T` is valid)

```cpp
template <typename T>
struct has_division<
    T,
    std::void_t<decltype(std::declval<T>() / std::declval<T>())>
> : std::true_type {};
```

What happens here:

* `std::declval<T>()` creates a **fake T** (no object needed)
* `decltype(a / b)` checks if the expression is valid
* `std::void_t<...>` turns a valid expression into `void`

If `T / T` is:

* ✅ valid → this specialization exists → `true_type`
* ❌ invalid → substitution fails → specialization ignored

**This is SFINAE in action.**

---

## 4. Enabling the function only when division exists

### Step 4.1 – Function enabled for divisible types

```cpp
template <typename T, std::enable_if_t<has_division<T>::value, int> = 0>
T divide(const T& a, const T& b) {
    return a / b;
}
```

Key idea:

* `enable_if_t<condition, int>` exists **only if condition is true**
* If `has_division<T>::value == false`
  → the function is **removed from overload resolution**

---

### Step 4.2 – Fallback for non-divisible types

```cpp
template <typename T, std::enable_if_t<!has_division<T>::value, int> = 0>
void divide(const T&, const T&) {
    std::cout << "No division available for this type\n";
}
```

So now:

| Type          | Which overload exists |
| ------------- | --------------------- |
| `int`         | division overload     |
| `double`      | division overload     |
| `std::string` | fallback overload     |

---

## 5. Full working example

```cpp
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

// 1) Detect division support
template <typename T, typename = void>
struct has_division : std::false_type {};

template <typename T>
struct has_division<
    T,
    std::void_t<decltype(std::declval<T>() / std::declval<T>())>
> : std::true_type {};

// 2) Enabled only if T supports division
template <typename T, std::enable_if_t<has_division<T>::value, int> = 0>
T divide(const T& a, const T& b) {
    return a / b;
}

// 3) Enabled only if T does NOT support division
template <typename T, std::enable_if_t<!has_division<T>::value, int> = 0>
void divide(const T&, const T&) {
    std::cout << "No division available for this type\n";
}

int main() {
    std::cout << divide(10, 2) << "\n";          // OK
    std::cout << divide(10.0, 2.0) << "\n";      // OK

    std::string s1 = "a", s2 = "b";
    divide(s1, s2);                              // graceful fallback
}
```

---

## 6. Why this matters in real APIs

You’ll see SFINAE used to:

* Enable algorithms only for **numeric types**
* Prevent misuse of generic APIs
* Create **type-safe** libraries (Eigen, STL, ranges, fmt, etc.)
* Provide **better overload selection** instead of runtime checks

---

## 7. Mental model to remember

Think of SFINAE like this:

> “If this template substitution doesn’t make sense,
> pretend this function never existed.”

No error. No warning. Just… gone.

---

## 8. Interview-level takeaway

If asked:

> *“What is SFINAE and why do we need it?”*

Answer:

> SFINAE lets template overloads disappear when a type does not satisfy certain compile-time requirements, enabling safe, expressive, and type-checked generic APIs without runtime overhead.

---

