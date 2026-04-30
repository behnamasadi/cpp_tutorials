# Resource Management Patterns

Resources in C++ — memory, file handles, sockets, mutex locks, GPU buffers, OS handles — must be acquired, used, and released in lockstep. C++ has no garbage collector to do this for you. The good news: with RAII and a few patterns, you almost never need to think about it again.

- [1. RAII as the Foundation](#1-raii-as-the-foundation)
- [2. Smart Pointers as Resource Wrappers](#2-smart-pointers-as-resource-wrappers)
- [3. Scope Guards](#3-scope-guards)
- [4. `unique_resource` (Library Fundamentals TS)](#4-unique_resource-library-fundamentals-ts)
- [5. Move-Only Resource Types](#5-move-only-resource-types)
- [6. Two-Phase Init: When You Can't Use a Constructor](#6-two-phase-init-when-you-cant-use-a-constructor)
- [7. Custom Deleters](#7-custom-deleters)
- [8. Resource Pools](#8-resource-pools)
- [9. Anti-patterns](#9-anti-patterns)

---

# 1. RAII as the Foundation

**Resource Acquisition Is Initialization** — the most important idiom in C++. Tie a resource's lifetime to an object's lifetime; the destructor releases it. See [RAII](../RAII.md).

```cpp
#include <cstdio>
#include <utility>

class FileHandle {
    FILE* fp;
public:
    explicit FileHandle(const char* path) : fp(std::fopen(path, "r")) {}

    ~FileHandle() {
        if (fp) std::fclose(fp);
    }

    // move: transfer ownership, leave source empty
    FileHandle(FileHandle&& o) noexcept : fp(o.fp) { o.fp = nullptr; }
    FileHandle& operator=(FileHandle&& o) noexcept {
        if (this != &o) {
            if (fp) std::fclose(fp);
            fp = o.fp;
            o.fp = nullptr;
        }
        return *this;
    }

    // no copies — only one owner of the FILE*
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    FILE* get() const { return fp; }
};

int main() {
    FileHandle f("data.txt");
    if (f.get()) std::puts("opened");
    // fclose runs automatically when f goes out of scope
}
```

The full Rule of Five (or sometimes Zero): see [Rule of 3, Rule of 5](../rule_of_3_rule_of_5.md).

# 2. Smart Pointers as Resource Wrappers

`std::unique_ptr` and `std::shared_ptr` aren't just for memory. With a custom deleter, they wrap any resource:

```cpp
#include <cstdio>
#include <memory>

int main() {
    std::unique_ptr<FILE, int(*)(FILE*)> fp(std::fopen("data.bin", "rb"), &std::fclose);
    if (fp) std::fread(/* ... */);
    // fclose called automatically
}
```

For a cleaner type, write the deleter as a struct:

```cpp
struct FileCloser {
    void operator()(FILE* fp) const { if (fp) std::fclose(fp); }
};

using UniqueFile = std::unique_ptr<FILE, FileCloser>;

int main() {
    UniqueFile fp(std::fopen("data.bin", "rb"));
    // ...
}
```

When you genuinely need shared ownership of a non-memory resource, `std::shared_ptr` works the same way (the deleter is type-erased into the control block, costing one extra allocation).

See [Smart Pointers](../smart_pointers.md).

# 3. Scope Guards

When you can't or don't want to write a class — say, the cleanup is one-off or local — use a scope guard:

```cpp
#include <utility>

template <class F>
class ScopeGuard {
    F f;
    bool active = true;
public:
    explicit ScopeGuard(F fn) : f(std::move(fn)) {}
    ~ScopeGuard() { if (active) f(); }
    void dismiss() { active = false; }

    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
};

// CTAD helper
template <class F> ScopeGuard(F) -> ScopeGuard<F>;
```

Use:

```cpp
#include <cstdio>

int main() {
    FILE* fp = std::fopen("foo", "r");
    ScopeGuard close_fp{[&]{ if (fp) std::fclose(fp); }};

    // ... do work that might throw or return early ...

    // commit-or-rollback style:
    ScopeGuard rollback{[&]{ std::puts("undoing changes"); }};
    bool ok = true;
    if (ok) rollback.dismiss();   // success — keep the changes

    // close_fp still runs at end of scope
}
```

This is the equivalent of Go's `defer`, but tied to scope rather than function. Libraries: `gsl::finally`, `absl::Cleanup`, `boost::scope::scope_exit`.

# 4. `unique_resource` (Library Fundamentals TS)

A scope guard with a value attached:

```cpp
#include <experimental/scope>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    auto sock = std::experimental::unique_resource(
        ::socket(AF_INET, SOCK_STREAM, 0),
        [](int s){ if (s >= 0) ::close(s); });

    if (sock.get() < 0) return 1;
    // use sock.get() ...
    // ::close runs when sock is destroyed
}
```

Unlike a unique_ptr, `unique_resource` works for non-pointer resources (file descriptors, OS handles). `std::experimental` for now; expected in a future standard.

# 5. Move-Only Resource Types

Owning resources usually means non-copyable, movable:

```cpp
#include <sys/mman.h>
#include <utility>

class Mmap {
    void* ptr = nullptr;
    size_t len = 0;
public:
    Mmap(int fd, size_t n)
        : ptr(::mmap(nullptr, n, PROT_READ, MAP_PRIVATE, fd, 0)), len(n) {}

    ~Mmap() {
        if (ptr && ptr != MAP_FAILED) ::munmap(ptr, len);
    }

    // non-copyable
    Mmap(const Mmap&) = delete;
    Mmap& operator=(const Mmap&) = delete;

    // movable: take the pointer, leave source empty
    Mmap(Mmap&& o) noexcept
        : ptr(std::exchange(o.ptr, nullptr)),
          len(std::exchange(o.len, 0)) {}

    Mmap& operator=(Mmap&& o) noexcept {
        if (this != &o) {
            if (ptr && ptr != MAP_FAILED) ::munmap(ptr, len);
            ptr = std::exchange(o.ptr, nullptr);
            len = std::exchange(o.len, 0);
        }
        return *this;
    }
};
```

`std::exchange` is the idiomatic way to "take" a resource from another object while leaving it in a safe-to-destruct state.

# 6. Two-Phase Init: When You Can't Use a Constructor

Sometimes the resource needs information not available at construction time, or initialization can fail in ways the caller wants to inspect non-throwingly.

Options:

1. **Static factory returning `std::optional<T>` or `std::expected<T,E>`.**
   ```cpp
   #include <expected>
   #include <string>
   #include <string_view>

   class Connection {
       int fd = -1;
       Connection() = default;        // private
   public:
       static std::expected<Connection, std::string>
       connect(std::string_view host);
   };
   ```
   Best of both worlds — the resulting `Connection` is fully constructed and valid.

2. **Two-phase: default-construct, then `init()`.** Worse — the type now has an invalid state. Reach for it only when forced (e.g., container default-init requirements).

# 7. Custom Deleters

Standard library calls (`fopen`/`fclose`, `pthread_mutex_init`/`destroy`) usually pair an init function with a destroy function. Custom deleters tie the pair together:

```cpp
#include <cstdio>
#include <cstdlib>
#include <dlfcn.h>

struct FreeDeleter    { void operator()(void* p) const { std::free(p); } };
struct PcloseDeleter  { void operator()(FILE* p) const { if (p) ::pclose(p); } };
struct DlcloseDeleter { void operator()(void* h) const { if (h) ::dlclose(h); } };
```

Document the contract — does the deleter accept `nullptr`? Does it succeed unconditionally? In `noexcept` contexts? Most cleanup APIs are no-throw and null-safe; if not, wrap them defensively.

# 8. Resource Pools

When acquiring/releasing is expensive (DB connections, threads, GPU buffers), reuse instances:

```cpp
#include <mutex>
#include <vector>
#include <utility>

using Connection = int;   // stand-in for an actual DB handle

class ConnectionPool {
    std::mutex m;
    std::vector<Connection> idle;
public:
    class Lease {
        ConnectionPool* pool;
        Connection conn;
    public:
        Lease(ConnectionPool* p, Connection c) : pool(p), conn(c) {}
        ~Lease() { pool->release(conn); }    // returns it to the pool
        Connection& operator*() { return conn; }
    };

    Lease acquire();
    void release(Connection c);
};
```

Patterns: object pool, free-list allocator, slab allocator. See [Memory Management Strategies](memory_management.md).

# 9. Anti-patterns

**Manual `new`/`delete`.** Almost always wrong in modern C++. Use `make_unique` / `make_shared`.

**`new` followed by exception in same scope.**
```cpp
auto* p = new Foo();
mightThrow();             // leaks p
delete p;
```
Use `unique_ptr` immediately:
```cpp
auto p = std::make_unique<Foo>();
mightThrow();             // p released by unwinding
```

**Resource members of unclear ownership.** A class holding `Database* db_` doesn't say whether it owns `db_`. Use `std::unique_ptr<Database>` (owns), `Database&` (borrows, lifetime caller's problem), or document explicitly.

**Returning raw pointers.** Use `std::unique_ptr<T>` for transfer-of-ownership, references for borrowed access, `T` by value for cheap-to-copy.

**Two-phase init by default.** A type with an invalid state is a type with twice the bug surface. Use factories returning `optional`/`expected`.

**`shared_ptr` for things that don't actually need shared ownership.** Default to `unique_ptr`. See [When to Use shared_ptr](../shared_ptr_use_cases.md).

# References

- [RAII](../RAII.md)
- [Smart Pointers](../smart_pointers.md)
- [Rule of 3, Rule of 5](../rule_of_3_rule_of_5.md)
- [Memory Management Strategies](memory_management.md)
- [`std::unique_resource` proposal P0052](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0052r10.pdf)
- *Effective Modern C++*, Items 18–22, Scott Meyers.
