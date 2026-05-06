# `std::optional`, `std::variant`, `std::any`

A practical guide to **expressing absence and alternatives without null pointers** in modern C++. Examples are drawn from real robotics scenarios — TF lookups, sensor buses, planners, parameter servers — so the trade-offs land in concrete code.

---

## 1. The problem these types replace

You constantly have values that **may or may not exist**, results that are **one of several outcomes**, or buses that carry **heterogeneous payloads**. Pre-C++17 idioms expressed all of this poorly:

- **Raw pointers as "maybe"** — `Foo* findFoo()` returns `nullptr` if absent. The type doesn't tell you whether null is even possible, who owns the memory, or what the lifetime is.
- **Sentinel values** — `-1` for "no reading", `NaN` for "not yet", `""` for "missing". The convention rots; new readers miss it.
- **Out-param + bool return** — `bool readImu(ImuSample& out)`. The output is mutated even on failure; callers forget to check the bool.
- **Inheritance for closed sets** — a virtual `Message` base class with three known subclasses, just to ship messages over a queue. Pays heap allocation and vtable cost for a problem that should be a tagged union.

Modern C++ replaces all of these with three building blocks:

| Tool             | Meaning                                  | Robotics analogue                                       |
| ---------------- | ---------------------------------------- | ------------------------------------------------------- |
| `optional<T>`    | "A `T`, or nothing"                      | TF lookup that may not be available yet                 |
| `variant<A,B,C>` | "Exactly one of these known types"       | Planner result: success, failure, or partial path       |
| `any`            | "Some value, type known only at runtime" | Parameter server / blackboard with heterogeneous values |

**Rule of thumb: prefer the tool that puts the most information in the type.** Reach for `any` only when you genuinely don't know the type at compile time.

---

## 2. `std::optional<T>` — "a value or nothing"

### When to use it

A function may legitimately produce no value, and you want the type system to force the caller to handle that.

Typical robotics uses:

- **TF lookup** between two coordinate frames — chain may not be connected yet.
- **Latest sample** from a sensor buffer — may not have arrived.
- **Optional config field** — `max_speed` unset → fall back to a default.
- **Calibration parameter lookup** — sensor may not be calibrated yet.

### Real example — TF lookup

```cpp
struct Transform { /* translation + rotation */ };

// Returns nullopt if `target` is not reachable from `source` in the TF tree.
std::optional<Transform> lookupTransform(std::string_view source,
                                         std::string_view target);

void planFromOdom() {
    auto tf = lookupTransform("odom", "base_link");
    if (!tf) {
        RCLCPP_WARN(logger_, "odom->base_link not yet available");
        return;
    }
    plan(*tf);                            // safe: just checked
}
```

Compare with the old style:

```cpp
// Bad: caller has no idea null is possible until they read the docs.
Transform* lookupTransform(std::string_view, std::string_view);

// Bad: out-param mutated even on failure; -1 / NaN sentinels rot.
bool lookupTransform(std::string_view, std::string_view, Transform& out);
```

`std::optional` makes "may not exist" part of the **return type**, not a comment.

### Properties

- Stored **inline** — no heap allocation. ~`sizeof(T) + 1` (a bool flag), plus alignment padding.
- Default-constructed optional is empty.
- Copyable / movable iff `T` is. Comparable.

### Checking presence — pick one idiom

```cpp
std::optional<double> latestJointAngle();   // encoder may not have published yet

// Best — bind + test in one expression, value scoped to the if/else.
if (auto angle = latestJointAngle()) {
    controller.update(*angle);
} else {
    controller.holdLastSetpoint();
}

// Also fine — explicit.
auto angle = latestJointAngle();
if (angle.has_value()) {
    controller.update(*angle);
}

// Equivalent to has_value() — uses operator bool.
if (angle) { /* ... */ }
```

Avoid `if (angle != std::nullopt)` — verbose, no benefit.

### Accessing the value — three ways, three failure modes

| Access            | If empty                          | Use when                                  |
| ----------------- | --------------------------------- | ----------------------------------------- |
| `*opt`            | **Undefined behavior**            | You just checked it on the line above     |
| `opt.value()`     | Throws `std::bad_optional_access` | You want a hard failure with a stack trace |
| `opt.value_or(d)` | Returns `d`                       | A sensible default exists                 |

```cpp
double angle = latestJointAngle().value_or(0.0);   // safe fallback
double v     = *latestJointAngle();                // UB if empty — only after a check
double v2    = latestJointAngle().value();         // throws — fine in non-realtime init
```

In **realtime control loops** avoid `.value()` (no exceptions on the hot path); use the bind-and-check form or `value_or`.

### Returning `std::nullopt`

```cpp
std::optional<Pose> latestPose() {
    if (!buffer_.fresh()) return std::nullopt;   // explicit absence
    return buffer_.read();
}
```

Don't return `{}` — it works (it constructs an empty optional) but reads ambiguously near `return Pose{};`.

### Chaining with monadic ops (C++23)

When you'd otherwise nest `if (opt) { ... if (opt2) { ... } }`:

```cpp
// Pose := transform(GPS fix) if available, else fall back to dead reckoning.
auto pose = readGpsFix()
              .transform([](const GpsFix& f) { return toLocalFrame(f); })
              .or_else([] { return std::optional{deadReckon()}; });
```

- `transform(f)` — applies `f` to the value if present, returns `optional<U>`.
- `and_then(f)` — like `transform` but `f` itself returns an optional (avoids `optional<optional<T>>`).
- `or_else(f)` — supplies a fallback `optional` when empty.

### Anti-patterns

```cpp
std::optional<int*>                  // pointless: int* already has nullptr
std::optional<std::unique_ptr<T>>    // unique_ptr already encodes "maybe null"
std::optional<T&>                    // illegal — references aren't reseatable
```

For an "optional reference to an existing object," choose:

```cpp
T*                                          // pragmatic, idiomatic C++
std::optional<std::reference_wrapper<T>>    // pedantically correct, ugly
```

Most robotics code uses `T*` and treats `nullptr` as "not bound yet."

### When NOT to use `optional`

- Absence is **impossible** — just return `T`.
- You need to distinguish *several* alternative types — use `variant`.
- Lifetime must be shared — use `shared_ptr`.

---

## 3. `std::variant<Ts...>` — "exactly one of these types"

### When to use it

You have a small, fixed set of alternatives that *would* otherwise be a class hierarchy or a tag enum + union.

Typical robotics uses:

- **Sensor message bus** — one queue carries `LidarScan | ImuSample | GpsFix`.
- **Planner result** — `PathFound | NoPath | Partial`.
- **Command channel** — `VelocityCmd | PoseCmd | JointTrajectory`.
- **State machines** — each state is a struct with the data it actually needs.

### Real example — planner result

A path planner can succeed, fail, or partially succeed. With a single struct you'd have an `optional<Path>`, a `string` reason, a coverage float, and unwritten rules about which fields are valid when. A variant lets each outcome carry **only the data that makes sense for it**:

```cpp
struct PathFound { std::vector<Pose> waypoints; double cost; };
struct NoPath    { std::string reason; };
struct Partial   { std::vector<Pose> waypoints; double coverage; };

using PlanResult = std::variant<PathFound, NoPath, Partial>;

PlanResult planTo(const Pose& goal);
```

The caller can't accidentally read `waypoints` from a `NoPath` — there are no waypoints there. The compiler enforces this.

### Properties

- Stored **inline** — `sizeof(variant) ≈ max(sizeof(Ts...)) + tag + padding`.
- No heap, no vtable, no RTTI.
- Always has exactly one active alternative (or `valueless_by_exception` after a throw during type change — rare).

### Building, checking, getting

```cpp
PlanResult r  = PathFound{waypoints, 12.4};
PlanResult r2 = NoPath{"goal in obstacle"};

// Index of the active type, in declaration order.
std::size_t i = r.index();              // 0 PathFound, 1 NoPath, 2 Partial

// Type-based query.
if (std::holds_alternative<NoPath>(r)) {
    const auto& f = std::get<NoPath>(r);
    log("planning failed: {}", f.reason);
}

// Safer get-pointer form (no exception on miss).
if (auto* p = std::get_if<PathFound>(&r)) {
    follow(p->waypoints);
}
```

`std::get<T>(v)` throws `std::bad_variant_access` if `T` isn't active; `std::get_if<T>(&v)` returns `nullptr` instead.

But chained `holds_alternative` / `get_if` is exactly what `std::visit` was made to replace.

### `std::visit` — the type-safe dispatcher

`std::visit(callable, variant)` calls `callable` with whichever alternative is currently active, and the compiler enforces that **every** alternative is handled.

#### Minimal example

The smallest possible working snippet — one variant, one generic lambda:

```cpp
#include <variant>
#include <iostream>

int main() {
    std::variant<int, double, std::string> v = 3.14;

    std::visit([](const auto& x) { std::cout << x << '\n'; }, v);
    // prints: 3.14
}
```

That's it. The generic lambda's `auto` parameter is deduced to whatever type is currently active (`double` here). Change `v = "hi";` and the same `visit` call prints `hi` — no code changes, no `if` chain.

#### Syntax in one place

```cpp
auto result = std::visit(callable, variant1, variant2, ...);
//                       └─ #1 ──┘ └────── #2+ ──────┘
```

| Position | What it is                            | Notes                                            |
| -------- | ------------------------------------- | ------------------------------------------------ |
| #1       | A callable                            | Lambda, function, functor, `overloaded{...}`     |
| #2+      | One or more variants                  | All are unpacked and passed as arguments to #1   |
| return   | Whatever the callable returns         | All branches must agree on a common return type  |

Three forms of callable, in order of how often you use them:

```cpp
// (a) Generic lambda — one body, deduced T. Use when every branch does the same thing.
std::visit([](const auto& x) { logger.publish(x); }, v);

// (b) overloaded{...} — different body per type. Use when branches diverge.
std::visit(overloaded{
    [](int i)              { /* ... */ },
    [](double d)           { /* ... */ },
    [](const std::string&) { /* ... */ },
}, v);

// (c) A named functor — the same as (b), but with a struct you can name and reuse.
struct Printer {
    void operator()(int i)              const { /* ... */ }
    void operator()(double d)           const { /* ... */ }
    void operator()(const std::string&) const { /* ... */ }
};
std::visit(Printer{}, v);
```

Five rules that catch most beginners:

1. **Variant comes after the callable**, not before. `std::visit(f, v)` — never `v.visit(f)`.
2. **Exhaustive or it won't compile.** Every alternative in the variant needs a matching `operator()` overload.
3. **All branches must return the same type** (or a common type). Mix `int` and `void` and you get a compile error.
4. **Take by `const auto&` / `const T&`** unless you need to mutate. The variant owns the value; visiting doesn't move it.
5. **You can pass multiple variants** — the callable then takes one argument per variant, and `visit` does the cross-product dispatch.

```cpp
// Multi-variant visit: dispatch on both at once.
std::variant<int, double> a = 1;
std::variant<int, double> b = 2.0;

auto sum = std::visit([](auto x, auto y) { return x + y; }, a, b);  // double, 3.0
```

The deeper walkthrough below shows how to actually structure (b) and (c) for a real robotics dispatcher.

#### Setup — a sensor message bus

```cpp
struct LidarScan { std::vector<float> ranges; };
struct ImuSample { float ax, ay, az, gx, gy, gz; };
struct GpsFix    { double lat, lon; float hdop; };

using SensorMsg = std::variant<LidarScan, ImuSample, GpsFix>;
```

#### Without `visit` — manual dispatch (don't do this)

```cpp
void handle(const SensorMsg& m) {
    if (std::holds_alternative<LidarScan>(m)) {
        updateOccupancyGrid(std::get<LidarScan>(m));
    } else if (std::holds_alternative<ImuSample>(m)) {
        propagateOrientation(std::get<ImuSample>(m));
    } else if (std::holds_alternative<GpsFix>(m)) {
        fuseGlobalPose(std::get<GpsFix>(m));
    }
    // Add WheelOdom to SensorMsg → compiler stays silent, messages drop on the floor.
}
```

Problems: no exhaustiveness check, double work (`holds_alternative` then `get` re-checks the index), branchy.

#### With `visit` — Step A: a hand-written visitor struct

A lambda is just a class with `operator()`. We can write that class explicitly, with one overload per sensor type:

```cpp
struct SensorVisitor {
    void operator()(const LidarScan& s) const { updateOccupancyGrid(s); }
    void operator()(const ImuSample& s) const { propagateOrientation(s); }
    void operator()(const GpsFix&    s) const { fuseGlobalPose(s); }
};

void handle(const SensorMsg& m) {
    std::visit(SensorVisitor{}, m);
}
```

`std::visit` picks the right `operator()` overload based on the active alternative — ordinary overload resolution. **Forget one alternative and this won't compile.**

#### Step B: the `overloaded` helper

The named struct is fine but tedious for one-off visitors. The `overloaded` helper builds the same kind of struct from a list of lambdas, inline:

```cpp
template <class... Fs>
struct overloaded : Fs... {       // (1) inherit from each lambda type
    using Fs::operator()...;      // (2) expose every lambda's operator() as overloads
};
template <class... Fs>
overloaded(Fs...) -> overloaded<Fs...>;   // (3) deduction guide
```

Reading it line by line:

1. **`struct overloaded : Fs...`** — inherit publicly from every type in the parameter pack `Fs`. Each lambda is its own anonymous class, so `overloaded` ends up containing all of their `operator()` member functions.
2. **`using Fs::operator()...;`** — without this, the inherited `operator()`s from different bases are ambiguous. The `using`-declaration brings each base's `operator()` into `overloaded` as a single **overload set**, so normal overload resolution picks the right one.
3. **The deduction guide** — lets you write `overloaded{lambdaA, lambdaB}` without spelling out template arguments. The compiler deduces `Fs...` from the lambdas you pass.

Write this helper **once** in a small utility header and reuse it everywhere.

#### Step C: using it

```cpp
void handle(const SensorMsg& m) {
    std::visit(overloaded{
        [](const LidarScan& s) { updateOccupancyGrid(s); },
        [](const ImuSample& s) { propagateOrientation(s); },
        [](const GpsFix&    s) { fuseGlobalPose(s); },
    }, m);
    // Add WheelOdom to SensorMsg without a handler here → compile error.
}
```

This is identical to Step A, just inline.

> **Mental model:** `overloaded{...}` = "make a struct whose `operator()` overloads are these lambdas, then hand it to `std::visit`."

#### When every branch does the same thing

A single generic lambda collapses it to one line:

```cpp
void log(const SensorMsg& m) {
    std::visit([](const auto& s) { logger.publish(s); }, m);
}
```

#### Returning a value from `visit`

`std::visit` returns whatever the callable returns (all branches must return the same type, or a common type):

```cpp
double estimateLatencyMs(const SensorMsg& m) {
    return std::visit(overloaded{
        [](const LidarScan&) { return 30.0; },
        [](const ImuSample&) { return  1.0; },
        [](const GpsFix&)    { return 50.0; },
    }, m);
}
```

### Why `variant` beats inheritance for closed sets

| Inheritance               | `variant`                         |
| ------------------------- | --------------------------------- |
| virtual dispatch          | direct call after a tag check     |
| heap allocation           | inline storage on the stack       |
| open set (anyone derives) | closed set (one source of truth)  |
| no exhaustiveness check   | compile-time exhaustiveness       |
| RTTI overhead             | no RTTI                           |

Inheritance is right when the set of types is **open** (plugins, third-party extensions). For a known, fixed set, `variant` is faster and safer.

### Expressing absence in a variant

If "nothing" is genuinely one alternative *among several*:

```cpp
using MaybeFix = std::variant<std::monostate, GpsFix>;
```

`std::monostate` is an empty placeholder type. But almost always, **`std::optional<GpsFix>` is the better choice** — single-purpose, clearer at the call site. Use `monostate` only when "nothing" coexists with multiple real alternatives.

### When NOT to use `variant`

- Open-ended types (plugin systems, user extensions).
- Type list isn't stable across versions.
- Runtime registration of new types.

For those, fall back to inheritance or `std::any`.

---

## 4. `std::any` — "some value, type known at runtime"

### When to use it

You're storing values whose types aren't known at compile time, typically in a **heterogeneous keyed store** that crosses a stable API boundary.

Typical robotics uses:

- **Parameter server / blackboard** — `set("max_speed", 1.5)`, `set("frame_id", std::string{"base_link"})`, `get<T>(key)`.
- **Behavior tree blackboards** — nodes share data of varying types under string keys.
- **Plugin metadata** — host doesn't know the shape of plugin-specific config.

If the type set is known at compile time, **use `variant` instead**.

### Real example — a tiny parameter blackboard

```cpp
class Blackboard {
    std::unordered_map<std::string, std::any> data_;
public:
    template <class T>
    void set(std::string key, T value) {
        data_[std::move(key)] = std::move(value);
    }

    template <class T>
    std::optional<T> get(const std::string& key) const {
        auto it = data_.find(key);
        if (it == data_.end()) return std::nullopt;
        if (auto* p = std::any_cast<T>(&it->second)) return *p;
        return std::nullopt;     // wrong type — caller asked for the wrong T
    }
};

Blackboard bb;
bb.set("max_speed", 1.5);
bb.set("base_frame", std::string{"base_link"});

double v = bb.get<double>("max_speed").value_or(0.0);
```

Notice the API: `any` is the **storage type**, but every read funnels through a typed `get<T>` that returns `std::optional<T>`. Callers never touch raw `any`.

### Properties

- Holds any *copy-constructible* type.
- Small-buffer optimization for tiny types; **heap-allocates** for larger ones.
- Slower than `variant` (allocation + type-erased dispatch internally).
- `any_cast<T>(a)` throws `std::bad_any_cast` on type mismatch; `any_cast<T>(&a)` returns `nullptr`.

### Why it's dangerous

- No compile-time type checking — typos in `T` become runtime errors.
- Encourages "stringly typed" designs where everything flows through a generic store.
- Slower than typed alternatives.

### Rule of thumb

> If you think you need `std::any`, first ask whether `variant` would do. If yes, use `variant`. Confine `any` to the boundaries of one subsystem.

---

## 5. Decision guide

```text
Need to express "may or may not exist"?
   └─ optional<T>

Need to express "exactly one of N known types"?
   └─ variant<Ts...>

Need to store "a value, type unknown at compile time"?
   └─ any   (last resort — almost always there's a better design)
```

### Comparison

| Feature             | `optional<T>` | `variant<Ts...>` | `any`     |
| ------------------- | ------------- | ---------------- | --------- |
| Expresses absence   | yes           | indirectly       | weakly    |
| Compile-time checks | yes           | yes              | no        |
| Heap allocation     | no            | no               | sometimes |
| Intent clarity      | excellent     | excellent        | poor      |
| Runtime overhead    | minimal       | minimal          | higher    |
| Best for APIs       | yes           | yes              | rarely    |

---

## 6. Putting it all together — replacing null pointers

Old C-style API:

```cpp
// Returns nullptr if not found. Caller-owned? Library-owned? Who frees?
Sensor* findSensor(int id);

// Out-param + bool. `out` is mutated even on failure.
bool readSample(int id, ImuSample& out);
```

Modern C++ alternatives — pick based on what you actually mean:

```cpp
// "May not exist, value semantics." Cheap, copyable.
std::optional<SensorInfo> findSensorInfo(int id);

// "May not exist, owning, expensive to copy." nullptr means absent.
std::unique_ptr<Sensor> openSensor(int id);

// "Definitely exists, non-owning." Best when you can guarantee lifetime.
Sensor& getSensor(int id);                     // throws / asserts if missing

// "Several outcomes, each with its own data."
std::variant<ImuSample, ImuStale, ImuFault> readSample(int id);
```

The right choice depends on **ownership** and **whether absence is meaningful**:

- *Cheap value, may be absent* → `optional<T>`
- *Owning, may be absent* → `unique_ptr<T>` (a `nullptr` *is* the absence)
- *Non-owning, must exist* → `T&`
- *Non-owning, may be absent* → `T*` (idiomatic) or `optional<reference_wrapper<T>>` (verbose)
- *Several distinct outcomes* → `variant<...>`

---

## 7. Interview-grade summary

- "`std::optional` encodes absence in the type system, removing null-pointer footguns."
- "`std::variant` gives compile-time-checked alternatives without inheritance or heap allocation."
- "`std::any` trades safety for flexibility and should be confined to subsystem boundaries."
- "Sentinel values, out-params, and raw pointers as 'maybe' are anti-patterns; modern C++ has dedicated types for each case."
