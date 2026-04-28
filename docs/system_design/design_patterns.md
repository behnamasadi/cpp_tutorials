# Object-Oriented Design Patterns in C++

The classic Gang-of-Four patterns, modernized for C++17/20. Many of them have idiomatic C++ realizations that don't look like the textbook UML — value semantics, templates, and `std::variant` give shorter, faster, safer alternatives.

- [1. Creational](#1-creational)
  - [Factory Method / Abstract Factory](#factory-method--abstract-factory)
  - [Builder](#builder)
  - [Singleton](#singleton)
  - [Prototype](#prototype)
- [2. Structural](#2-structural)
  - [Adapter](#adapter)
  - [Decorator](#decorator)
  - [Facade](#facade)
  - [Proxy](#proxy)
  - [Composite](#composite)
- [3. Behavioral](#3-behavioral)
  - [Strategy](#strategy)
  - [Observer](#observer)
  - [Command](#command)
  - [Visitor](#visitor)
  - [State](#state)
  - [Template Method](#template-method)
  - [Chain of Responsibility](#chain-of-responsibility)
- [4. Modern C++ Replaces Several](#4-modern-c-replaces-several)

---

# 1. Creational

## Factory Method / Abstract Factory

A factory hides the choice of concrete type from the caller.

```cpp
struct Shape { virtual ~Shape() = default; virtual void draw() const = 0; };
struct Circle : Shape { void draw() const override; };
struct Square : Shape { void draw() const override; };

std::unique_ptr<Shape> makeShape(std::string_view kind) {
    if (kind == "circle") return std::make_unique<Circle>();
    if (kind == "square") return std::make_unique<Square>();
    throw std::invalid_argument("unknown shape");
}
```

For a registry-driven factory:

```cpp
class ShapeFactory {
    using Creator = std::function<std::unique_ptr<Shape>()>;
    std::unordered_map<std::string, Creator> map_;
public:
    void registerType(std::string name, Creator c) { map_[std::move(name)] = std::move(c); }
    std::unique_ptr<Shape> create(const std::string& name) const { return map_.at(name)(); }
};
```

## Builder

Useful when an object has many optional parameters and you want to avoid telescoping constructors.

```cpp
class HttpRequestBuilder {
    HttpRequest req_;
public:
    HttpRequestBuilder& method(std::string m) { req_.method = std::move(m); return *this; }
    HttpRequestBuilder& url(std::string u)    { req_.url = std::move(u); return *this; }
    HttpRequestBuilder& header(std::string k, std::string v) { req_.headers[k] = v; return *this; }
    HttpRequestBuilder& body(std::string b)   { req_.body = std::move(b); return *this; }
    HttpRequest build() && { return std::move(req_); }
};

auto req = HttpRequestBuilder{}.method("POST").url("/api").body("...").build();
```

C++20 designated initializers cover most simple cases without a builder:
```cpp
HttpRequest r{ .method="POST", .url="/api", .body="..." };
```

## Singleton

The most-misused pattern. Almost always you want **dependency injection** instead. If you genuinely need one shared instance, prefer Meyers' singleton:

```cpp
class Logger {
public:
    static Logger& instance() {
        static Logger inst;   // C++11 guarantees thread-safe init
        return inst;
    }
private:
    Logger() = default;
};
```

When *not* to use singleton: anything that has state, anything testable. See [Static Initialization Order Fiasco](../static_member_function_order_fiasco.md).

## Prototype

Cloning an object whose concrete type isn't known. Standard implementation:

```cpp
struct Shape {
    virtual ~Shape() = default;
    virtual std::unique_ptr<Shape> clone() const = 0;
};
struct Circle : Shape {
    std::unique_ptr<Shape> clone() const override {
        return std::make_unique<Circle>(*this);
    }
};
```

CRTP saves boilerplate; see [Virtual Constructor / Clone idiom](../virtual_destructor_virtual_constructor.md).

# 2. Structural

## Adapter

Wrap an existing interface to look like a different one.

```cpp
class LegacyLogger {
public:
    void writeLine(const char* msg);
};

struct ILogger { virtual void log(std::string_view) = 0; virtual ~ILogger() = default; };

class LegacyLoggerAdapter : public ILogger {
    LegacyLogger& legacy_;
public:
    explicit LegacyLoggerAdapter(LegacyLogger& l) : legacy_(l) {}
    void log(std::string_view m) override { legacy_.writeLine(std::string{m}.c_str()); }
};
```

## Decorator

Add behavior around an existing object without changing its type.

```cpp
struct Stream { virtual void write(std::string_view) = 0; virtual ~Stream() = default; };

class TimestampDecorator : public Stream {
    Stream& inner_;
public:
    explicit TimestampDecorator(Stream& s) : inner_(s) {}
    void write(std::string_view msg) override {
        inner_.write(std::format("[{}] {}", now(), msg));
    }
};
```

Stack decorators by composition: `EncryptDecorator{ CompressDecorator{ FileStream{...} } }`.

## Facade

A single, simple interface to a complex subsystem.

```cpp
class VideoTranscoder {
public:
    void transcode(const std::filesystem::path& in, const std::filesystem::path& out);
private:
    Demuxer demuxer_;
    Decoder decoder_;
    Filter filter_;
    Encoder encoder_;
    Muxer muxer_;
};
```

The facade itself does very little — it just orchestrates. The point is that *clients* see one type instead of five.

## Proxy

A stand-in that controls access to a real object: virtual proxy (lazy load), protection proxy (auth), remote proxy (RPC stub), smart pointer (already a proxy).

```cpp
class LazyImage {
    std::filesystem::path path_;
    mutable std::unique_ptr<Image> img_;
public:
    explicit LazyImage(std::filesystem::path p) : path_(std::move(p)) {}
    void render() const {
        if (!img_) img_ = std::make_unique<Image>(loadFromDisk(path_));
        img_->render();
    }
};
```

## Composite

Treat a tree of objects uniformly with the leaves.

```cpp
struct Node { virtual ~Node() = default; virtual int size() const = 0; };
struct File : Node { int sz; int size() const override { return sz; } };
struct Dir  : Node {
    std::vector<std::unique_ptr<Node>> children;
    int size() const override {
        int s = 0; for (const auto& c : children) s += c->size(); return s;
    }
};
```

# 3. Behavioral

## Strategy

Inject an algorithm:

```cpp
class TextSorter {
    std::function<bool(std::string_view, std::string_view)> cmp_;
public:
    explicit TextSorter(decltype(cmp_) c) : cmp_(std::move(c)) {}
    void sort(std::vector<std::string>& v) { std::ranges::sort(v, cmp_); }
};
```

Compile-time strategy via templates is usually cheaper:

```cpp
template<class Cmp> void sortText(std::vector<std::string>& v, Cmp cmp) {
    std::ranges::sort(v, cmp);
}
```

## Observer

Subjects notify observers when something changes. Naive version:

```cpp
class Subject {
    std::vector<std::function<void(const Event&)>> subs_;
public:
    void subscribe(std::function<void(const Event&)> f) { subs_.push_back(std::move(f)); }
    void notify(const Event& e) { for (auto& f : subs_) f(e); }
};
```

Production-grade: handle unsubscribe (return a token / RAII subscription), thread safety, notification during mutation. Boost.Signals2 and Qt's signals/slots are robust implementations.

## Command

Encapsulate a request as an object so it can be queued, logged, undone:

```cpp
struct Command { virtual ~Command() = default; virtual void execute() = 0; virtual void undo() = 0; };

class History {
    std::vector<std::unique_ptr<Command>> done_;
public:
    void run(std::unique_ptr<Command> c) { c->execute(); done_.push_back(std::move(c)); }
    void undoLast() { if (!done_.empty()) { done_.back()->undo(); done_.pop_back(); } }
};
```

## Visitor

Add operations to a closed hierarchy without modifying the hierarchy:

```cpp
struct Circle; struct Square;
struct ShapeVisitor {
    virtual void visit(Circle&) = 0;
    virtual void visit(Square&) = 0;
    virtual ~ShapeVisitor() = default;
};
struct Shape { virtual void accept(ShapeVisitor&) = 0; virtual ~Shape() = default; };
struct Circle : Shape { void accept(ShapeVisitor& v) override { v.visit(*this); } };
struct Square : Shape { void accept(ShapeVisitor& v) override { v.visit(*this); } };
```

In modern C++, prefer `std::variant` + `std::visit`:

```cpp
using AnyShape = std::variant<Circle, Square, Triangle>;
auto area(const AnyShape& s) {
    return std::visit([](auto& x){ return x.area(); }, s);
}
```

No vtable, no heap, exhaustiveness-ish (compiler warns on missing overloads). See [std::visit](../std_visit.md), [Double Dispatch](../double_dispatch.md).

## State

An object's behavior changes with its internal state. State machine version: see [Event-Driven Architecture and State Machines](event_driven_state_machines.md).

```cpp
class Connection {
    enum class State { Disconnected, Connecting, Connected, Closing };
    State state_ = State::Disconnected;
public:
    void send(std::string_view) {
        switch (state_) {
            case State::Connected: /* actually send */ break;
            case State::Connecting: throw std::logic_error("not yet ready");
            default:                throw std::logic_error("not connected");
        }
    }
};
```

## Template Method

Base class defines the algorithm skeleton; subclasses fill in steps.

```cpp
class ReportGenerator {
public:
    void generate() { fetch(); transform(); render(); }
    virtual ~ReportGenerator() = default;
protected:
    virtual void fetch() = 0;
    virtual void transform() = 0;
    virtual void render() = 0;
};
```

Often replaceable by Strategy (composition over inheritance).

## Chain of Responsibility

Pass a request along a chain until someone handles it:

```cpp
struct Handler {
    Handler* next = nullptr;
    virtual bool handle(Request& r) = 0;
    virtual ~Handler() = default;

    bool dispatch(Request& r) {
        return handle(r) || (next && next->dispatch(r));
    }
};
```

HTTP middleware stacks are exactly this.

# 4. Modern C++ Replaces Several

| Pattern | Modern C++ Often Just |
|---|---|
| Strategy | A `std::function` parameter or a template parameter |
| Visitor | `std::variant` + `std::visit` |
| Iterator | Range-based `for`, ranges library |
| Singleton | A static local in a free function (or DI) |
| Prototype | Copy constructor, `clone()` only when polymorphic |
| Observer | `std::function` callbacks; signals libraries |
| Command | `std::function<void()>` or a lambda with capture |
| State | `std::variant<States...>` + `std::visit` for transitions |
| Adapter | A free function or a thin lambda |

The principle: prefer **value semantics, free functions, and templates** over deep class hierarchies. Many GoF patterns exist to work around features that C++ already gives you (closures, sum types, generics).

# References

- *Design Patterns: Elements of Reusable Object-Oriented Software*, Gamma/Helm/Johnson/Vlissides (the GoF).
- *C++ Software Design*, Klaus Iglberger — modern, value-semantic recasts.
- *Modern C++ Design*, Andrei Alexandrescu — policy-based design.
- [SOLID Principles](solid_principles.md), [Concurrency Patterns](concurrency_patterns.md), [Event-Driven and State Machines](event_driven_state_machines.md)
