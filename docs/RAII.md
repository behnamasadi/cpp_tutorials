# RAII

**RAII** stands for *Resource Acquisition Is Initialization*. The idea is one sentence:

> Tie a resource's lifetime to the lifetime of an object on the stack.
> The constructor acquires it; the destructor releases it.

Because destructors run automatically when an object goes out of scope — even if an exception is thrown — you get cleanup *for free*, in every code path, without writing a single `try`/`catch`.

That's it. The rest of this page is just the same pattern, applied to four resources.

---

## 1. The pattern, on its own

Forget what the resource is for a moment. Just watch the constructor and destructor fire.

```cpp
#include <iostream>

class Resource {
public:
    Resource()  { std::cout << "  acquired\n"; }
    ~Resource() { std::cout << "  released\n"; }
};

int main() {
    std::cout << "before scope\n";
    {
        Resource r;          // constructor runs here
        std::cout << "  inside scope\n";
    }                        // destructor runs here, automatically
    std::cout << "after scope\n";
}
```

Output:

```text
before scope
  acquired
  inside scope
  released
after scope
```

The braces define a scope. When `r` falls off the end of that scope, `~Resource()` runs. You did not call it. You cannot forget to call it. That is the whole trick.

---

## 2. Reading a file (the most intuitive example)

You've all written this in C: open a file, do something, close it.

```cpp
// without RAII — manual cleanup, easy to leak
void readFile(const char* path) {
    FILE* f = std::fopen(path, "r");
    if (!f) return;

    parseHeader(f);          // throws on bad format → fclose never runs → leak
    parseBody(f);
    std::fclose(f);          // only runs on the happy path
}
```

The bug is invisible until something throws or you add an early `return` and forget to add a matching `fclose`. Now the RAII version:

```cpp
// with RAII — the std library already wrote the wrapper
void readFile(const std::string& path) {
    std::ifstream f(path);   // constructor: opens the file
    parseHeader(f);          // even if this throws…
    parseBody(f);
}                            // …~ifstream runs and closes the file
```

`std::ifstream` *is* an RAII class. The opening happens in its constructor, the closing in its destructor. No leak path exists, because there is no `fclose` for you to forget.

The standard library is full of these pre-built wrappers: `std::unique_ptr` for heap memory, `std::lock_guard` for mutexes, `std::fstream` for files, `std::jthread` for threads. Whenever you see one, you're seeing RAII.

---

## 3. A database connection

Same pattern, made-up API. A connection must be opened and closed.

```cpp
// without RAII
void loadUsers(const std::string& url) {
    Database* db = Database::connect(url);
    auto users = db->query("SELECT * FROM users");   // throws → leak
    db->close();
    delete db;
}
```

Two failure modes here: if `query` throws, neither `close` nor `delete` runs. The connection stays open on the server, the heap object leaks on the client. Wrap it once and the problem disappears:

```cpp
// with RAII — a tiny wrapper class
class DbConnection {
    Database* m_db;
public:
    DbConnection(const std::string& url) : m_db(Database::connect(url)) {}
    ~DbConnection() { m_db->close(); delete m_db; }

    Database* operator->() { return m_db; }   // so we can write db->query(...)
};

void loadUsers(const std::string& url) {
    DbConnection db(url);
    auto users = db->query("SELECT * FROM users");
}                                              // ~DbConnection runs, always
```

Three lines of class for unconditional cleanup forever after. That's the trade.

---

## 4. A robotics example: the motor enable guard

Now the same pattern on real hardware. A motor must be **enabled** before you command it and **disabled** afterwards (so the brake re-engages and the arm doesn't drift).

```cpp
// without RAII — the try/catch dance
void runTrajectory(Motor& motor, const Trajectory& traj) {
    motor.enable();
    try {
        for (const auto& pt : traj) {
            motor.command(pt);     // might throw on waypoint 47
        }
    } catch (...) {
        motor.disable();           // remember to disable on every error path…
        throw;
    }
    motor.disable();               // …and again on the success path
}
```

Two `disable()` calls, one `try`/`catch`, and the day you add an early `return` for some edge case and forget the third `disable()`, a 30 kg arm stays energised at the last commanded torque. Now with RAII:

```cpp
// with RAII — write a guard, write the loop, done
class MotorGuard {
    Motor& m_motor;
public:
    MotorGuard(Motor& motor) : m_motor(motor) { m_motor.enable(); }
    ~MotorGuard() { m_motor.disable(); }
};

void runTrajectory(Motor& motor, const Trajectory& traj) {
    MotorGuard guard(motor);
    for (const auto& pt : traj) {
        motor.command(pt);
    }
}                                  // disable() runs no matter how we leave
```

The cleanup is now *unconditional*. Normal return, early return, exception — all three paths go through `~MotorGuard`, which calls `motor.disable()`. You cannot forget, because there is nothing to remember.

---

## Wait — isn't this just "wrap it in a class instead of a function"?

A natural reaction after seeing these examples: *all you did was move the cleanup from a function call into a class. Why does that matter?* It's a fair question, and the answer reveals what RAII actually depends on.

Try the function version:

```cpp
void withMotor(Motor& m, std::function<void()> work) {
    m.enable();
    work();          // throws? early-returns? we never get to disable()
    m.disable();
}

withMotor(motor, [&]{
    for (auto& pt : traj) motor.command(pt);   // throws on waypoint 47
});                                            // disable() never runs — same bug
```

A function wrapper buys you nothing. You'd have to put `try`/`catch` inside `withMotor`, and now you're back to manual cleanup, just hidden one level deeper.

What a class gives you that a function doesn't:

```cpp
{
    MotorGuard g(motor);   // 1. constructor runs
    work();                // 2. anything happens here — throw, return, whatever
}                          // 3. destructor runs — guaranteed by the language
```

Step 3 is not a function call you wrote. It is the compiler emitting destructor calls as part of stack unwinding. That happens on:

- normal scope exit
- `return` from anywhere in scope
- `break` / `continue`
- an exception propagating through

There is no syntax in C++ for *"run this code at scope exit"* — **except destructors**. Other languages have dedicated syntax: Go has `defer`, Python has `with`, Java has `try-with-resources`. C++ chose to deliver the same capability through destructors of stack objects, and so *the class* became the unit of "thing that owns a resource."

So the precise statement is: **RAII is not "wrap it in a class instead of a function." It is "wrap it in a class because that is how you get a destructor, and a destructor is the only thing the language will call for you automatically on every exit path."** The class is the delivery mechanism, not the point.

Once you see it that way, every RAII type is the same skeleton: a class whose only job is to exist on the stack so its destructor runs.

---

## The same shape, every time

| Resource          | Acquire (ctor)     | Release (dtor)        | Std lib wrapper        |
| ----------------- | ------------------ | --------------------- | ---------------------- |
| Heap memory       | `new` / `malloc`   | `delete` / `free`     | `std::unique_ptr`      |
| File              | `fopen` / `open`   | `fclose` / `close`    | `std::fstream`         |
| Mutex             | `lock()`           | `unlock()`            | `std::lock_guard`      |
| Thread            | `std::thread(...)` | `join()`              | `std::jthread`         |
| Socket / CAN / fd | `socket()`         | `close()`             | *(write your own)*     |
| GPU buffer        | `cudaMalloc`       | `cudaFree`            | *(write your own)*     |
| Motor / brake     | `enable()`         | `disable()`           | *(write your own)*     |

For everything in the right column you get RAII for free. For the bottom three, you write a small class that looks exactly like `DbConnection` or `MotorGuard` above.

---

## The recipe

Whenever you find yourself writing this:

```cpp
acquire();
do_stuff();        // might throw, might return early
release();         // easy to skip, easy to forget
```

…stop and write a small RAII class instead. Three rules:

1. **Acquire in the constructor.** Throw if it fails — no half-built objects.
2. **Release in the destructor.** The destructor must not throw.
3. **Disable copying** (`= delete` the copy constructor and assignment) unless copying the resource genuinely makes sense — it usually doesn't for hardware or OS handles.

That's RAII. Once you internalise it, you stop writing cleanup code and start writing classes whose *existence* is the cleanup.
