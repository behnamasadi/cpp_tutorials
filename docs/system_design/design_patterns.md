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
struct Shape {
  virtual ~Shape() = default;
  virtual void draw() const = 0;
};
struct Circle : Shape { void draw() const override { std::cout << "circle\n"; } };
struct Square : Shape { void draw() const override { std::cout << "square\n"; } };

std::unique_ptr<Shape> make_shape(std::string_view kind) {
  if (kind == "circle") return std::make_unique<Circle>();
  if (kind == "square") return std::make_unique<Square>();
  throw std::invalid_argument("unknown shape");
}

int main() {
  auto s = make_shape("circle");
  s->draw();
}
```

For a registry you can extend at runtime:

```cpp
class ShapeFactory {
  std::unordered_map<std::string, std::function<std::unique_ptr<Shape>()>> makers;
public:
  void add(std::string name, std::function<std::unique_ptr<Shape>()> maker) {
    makers[name] = maker;
  }
  std::unique_ptr<Shape> create(const std::string& name) {
    return makers.at(name)();
  }
};
```

## Builder

Useful when an object has many optional parameters and you want to avoid telescoping constructors.

```cpp
struct HttpRequest {
  std::string method;
  std::string url;
  std::string body;
};

class HttpRequestBuilder {
  HttpRequest req;
public:
  HttpRequestBuilder& method(std::string m) { req.method = m; return *this; }
  HttpRequestBuilder& url(std::string u)    { req.url    = u; return *this; }
  HttpRequestBuilder& body(std::string b)   { req.body   = b; return *this; }
  HttpRequest build() { return req; }
};

int main() {
  HttpRequest r = HttpRequestBuilder{}
                    .method("POST")
                    .url("/api")
                    .body("hello")
                    .build();
}
```

C++20 designated initializers cover most simple cases without a builder:

```cpp
HttpRequest r{ .method = "POST", .url = "/api", .body = "hello" };
```

## Singleton

The most-misused pattern. Almost always you want **dependency injection** instead. If you genuinely need one shared instance, use Meyers' singleton:

```cpp
class Logger {
public:
  static Logger& instance() {
    static Logger inst;          // C++11 guarantees thread-safe init
    return inst;
  }
  void log(const std::string& msg) { std::cout << msg << '\n'; }
private:
  Logger() = default;
};

int main() {
  Logger::instance().log("hello");
}
```

When *not* to use singleton: anything testable, anything with mutable state. See [Static Initialization Order Fiasco](../static_member_function_order_fiasco.md).

## Prototype

Cloning an object whose concrete type isn't known at the call site:

```cpp
struct Shape {
  virtual ~Shape() = default;
  virtual std::unique_ptr<Shape> clone() const = 0;
  virtual void draw() const = 0;
};

struct Circle : Shape {
  std::unique_ptr<Shape> clone() const override {
    return std::make_unique<Circle>(*this);
  }
  void draw() const override { std::cout << "circle\n"; }
};

void demo(const Shape& s) {
  auto copy = s.clone();
  copy->draw();
}
```

CRTP saves boilerplate; see [Virtual Constructor / Clone idiom](../virtual_destructor_virtual_constructor.md).

# 2. Structural

## Adapter

Wrap an existing interface to look like a different one.

```cpp
class LegacyLogger {
public:
  void writeLine(const char* msg) { std::cout << msg << '\n'; }
};

struct ILogger {
  virtual ~ILogger() = default;
  virtual void log(const std::string& msg) = 0;
};

class LegacyLoggerAdapter : public ILogger {
  LegacyLogger& legacy;
public:
  LegacyLoggerAdapter(LegacyLogger& l) : legacy(l) {}
  void log(const std::string& msg) override { legacy.writeLine(msg.c_str()); }
};
```

## Decorator

Add behavior around an existing object without changing its type.

```cpp
struct Stream {
  virtual ~Stream() = default;
  virtual void write(const std::string& msg) = 0;
};

class ConsoleStream : public Stream {
public:
  void write(const std::string& msg) override { std::cout << msg << '\n'; }
};

class TimestampDecorator : public Stream {
  Stream& inner;
public:
  TimestampDecorator(Stream& s) : inner(s) {}
  void write(const std::string& msg) override {
    inner.write("[12:00] " + msg);
  }
};

int main() {
  ConsoleStream raw;
  TimestampDecorator stamped(raw);
  stamped.write("hello");   // [12:00] hello
}
```

Stack decorators by composition: `TimestampDecorator(EncryptDecorator(file))`.

## Facade

A single, simple interface to a complex subsystem.

```cpp
class Demuxer { /* ... */ };
class Decoder { /* ... */ };
class Encoder { /* ... */ };
class Muxer   { /* ... */ };

class VideoTranscoder {
  Demuxer demuxer;
  Decoder decoder;
  Encoder encoder;
  Muxer   muxer;
public:
  void transcode(const std::string& in, const std::string& out) {
    // orchestrate demuxer -> decoder -> encoder -> muxer
  }
};
```

The facade itself does very little — it just orchestrates. The point is that *clients* see one type instead of four.

## Proxy

A stand-in that controls access to a real object: virtual proxy (lazy load), protection proxy (auth), remote proxy (RPC stub), smart pointer (already a proxy).

```cpp
class Image {
public:
  Image(const std::string& path) { std::cout << "loaded " << path << '\n'; }
  void render() const { std::cout << "rendering\n"; }
};

class LazyImage {
  std::string path;
  mutable std::unique_ptr<Image> img;
public:
  LazyImage(std::string p) : path(p) {}
  void render() const {
    if (!img) img = std::make_unique<Image>(path);
    img->render();
  }
};
```

## Composite

Treat a tree of objects uniformly with the leaves.

```cpp
struct Node {
  virtual ~Node() = default;
  virtual int size() const = 0;
};

struct File : Node {
  int bytes;
  File(int b) : bytes(b) {}
  int size() const override { return bytes; }
};

struct Dir : Node {
  std::vector<std::unique_ptr<Node>> children;
  int size() const override {
    int total = 0;
    for (const auto& c : children) total += c->size();
    return total;
  }
};
```

# 3. Behavioral

## Strategy

Inject an algorithm:

```cpp
class TextSorter {
  std::function<bool(const std::string&, const std::string&)> cmp;
public:
  TextSorter(std::function<bool(const std::string&, const std::string&)> c) : cmp(c) {}
  void sort(std::vector<std::string>& v) { std::sort(v.begin(), v.end(), cmp); }
};

int main() {
  std::vector<std::string> v = {"banana", "apple", "cherry"};
  TextSorter s([](const std::string& a, const std::string& b) { return a < b; });
  s.sort(v);
}
```

Compile-time strategy via templates is usually cheaper:

```cpp
template <class Cmp>
void sort_text(std::vector<std::string>& v, Cmp cmp) {
  std::sort(v.begin(), v.end(), cmp);
}
```

## Observer

Subjects notify observers when something changes. We use `int` as the event type for simplicity.

```cpp
class Subject {
  std::vector<std::function<void(int)>> subscribers;
public:
  void subscribe(std::function<void(int)> f) { subscribers.push_back(f); }
  void notify(int event) {
    for (auto& f : subscribers) f(event);
  }
};

int main() {
  Subject s;
  s.subscribe([](int e) { std::cout << "got " << e << '\n'; });
  s.notify(42);
}
```

A real implementation also handles unsubscribe (return a token / RAII subscription), thread safety, and notification during mutation. Boost.Signals2 and Qt's signals/slots are robust ones.

## Command

Encapsulate a request as an object so it can be queued, logged, undone:

```cpp
struct Command {
  virtual ~Command() = default;
  virtual void execute() = 0;
  virtual void undo() = 0;
};

class History {
  std::vector<std::unique_ptr<Command>> done;
public:
  void run(std::unique_ptr<Command> c) {
    c->execute();
    done.push_back(std::move(c));
  }
  void undo_last() {
    if (done.empty()) return;
    done.back()->undo();
    done.pop_back();
  }
};
```

## Visitor

Add operations to a closed hierarchy without modifying the hierarchy:

```cpp
struct Circle;
struct Square;

struct ShapeVisitor {
  virtual ~ShapeVisitor() = default;
  virtual void visit(Circle&) = 0;
  virtual void visit(Square&) = 0;
};

struct Shape {
  virtual ~Shape() = default;
  virtual void accept(ShapeVisitor&) = 0;
};
struct Circle : Shape { void accept(ShapeVisitor& v) override { v.visit(*this); } };
struct Square : Shape { void accept(ShapeVisitor& v) override { v.visit(*this); } };
```

In modern C++, prefer `std::variant` + `std::visit`:

```cpp
struct Circle { double r; double area() const { return 3.14 * r * r; } };
struct Square { double s; double area() const { return s * s; } };

using AnyShape = std::variant<Circle, Square>;

double area(const AnyShape& s) {
  return std::visit([](const auto& x) { return x.area(); }, s);
}
```

No vtable, no heap, exhaustiveness-ish (the compiler warns on missing overloads). See [std::visit](../std_visit.md), [Double Dispatch](../double_dispatch.md).

## State

An object's behavior changes with its internal state:

```cpp
class Connection {
  enum class State { Disconnected, Connecting, Connected };
  State state = State::Disconnected;
public:
  void connect() { state = State::Connected; }
  void send(const std::string& msg) {
    if (state != State::Connected)
      throw std::logic_error("not connected");
    std::cout << "sending: " << msg << '\n';
  }
};
```

For larger state machines, see [Event-Driven Architecture and State Machines](event_driven_state_machines.md).

## Template Method

Base class defines the algorithm skeleton; subclasses fill in the steps.

```cpp
class ReportGenerator {
public:
  virtual ~ReportGenerator() = default;
  void generate() {
    fetch();
    transform();
    render();
  }
protected:
  virtual void fetch()     = 0;
  virtual void transform() = 0;
  virtual void render()    = 0;
};
```

Often replaceable by Strategy (composition over inheritance).

## Chain of Responsibility

Pass a request along a chain until someone handles it. We use `int` as the request type.

```cpp
struct Handler {
  Handler* next = nullptr;
  virtual ~Handler() = default;
  virtual bool handle(int request) = 0;

  bool dispatch(int request) {
    if (handle(request)) return true;
    return next ? next->dispatch(request) : false;
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
