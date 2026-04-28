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
class FileHandle {
    FILE* fp_;
public:
    explicit FileHandle(const char* path)
        : fp_(std::fopen(path, "r")) {
        if (!fp_) throw std::system_error(errno, std::generic_category(), path);
    }
    ~FileHandle() { if (fp_) std::fclose(fp_); }
    FileHandle(FileHandle&& o) noexcept : fp_(std::exchange(o.fp_, nullptr)) {}
    FileHandle& operator=(FileHandle&& o) noexcept {
        if (this != &o) {
            if (fp_) std::fclose(fp_);
            fp_ = std::exchange(o.fp_, nullptr);
        }
        return *this;
    }
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;
    FILE* get() const noexcept { return fp_; }
};
```

The full Rule of Five (or sometimes Zero): see [Rule of 3, Rule of 5](../rule_of_3_rule_of_5.md).

# 2. Smart Pointers as Resource Wrappers

`std::unique_ptr` and `std::shared_ptr` aren't just for memory. With a custom deleter, they wrap any resource:

```cpp
auto fp = std::unique_ptr<FILE, decltype(&std::fclose)>(
    std::fopen("data.bin", "rb"), &std::fclose);

auto win = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>(
    SDL_CreateWindow(...), &SDL_DestroyWindow);
```

For a cleaner type, write the deleter as a struct:

```cpp
struct FileCloser { void operator()(FILE* fp) const noexcept { if (fp) std::fclose(fp); } };
using UniqueFile = std::unique_ptr<FILE, FileCloser>;
```

When you genuinely need shared ownership of a non-memory resource, `std::shared_ptr` works the same way (the deleter is type-erased into the control block, costing one extra allocation).

See [Smart Pointers](../smart_pointers.md).

# 3. Scope Guards

When you can't or don't want to write a class — say, the cleanup is one-off or local — use a scope guard:

```cpp
template<class F>
class ScopeGuard {
    F f_;
    bool active_ = true;
public:
    explicit ScopeGuard(F f) : f_(std::move(f)) {}
    ~ScopeGuard() { if (active_) f_(); }
    void dismiss() noexcept { active_ = false; }
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
};

template<class F> ScopeGuard(F) -> ScopeGuard<F>;
```

Use:

```cpp
auto fp = std::fopen("foo", "r");
ScopeGuard close_fp{[&]{ if (fp) std::fclose(fp); }};

auto raw = native_alloc(n);
ScopeGuard free_raw{[&]{ native_free(raw); }};
if (!setup(raw)) return;            // both cleaned up automatically
free_raw.dismiss();                 // commit — keep the resource
return raw;
```

This is the equivalent of Go's `defer`, but tied to scope rather than function. Libraries: `gsl::finally`, `absl::Cleanup`, `boost::scope::scope_exit`.

# 4. `unique_resource` (Library Fundamentals TS)

A scope guard with a value attached:

```cpp
auto sock = std::experimental::unique_resource(
    socket(AF_INET, SOCK_STREAM, 0),
    [](int s){ ::close(s); });

if (sock.get() < 0) return;
::bind(sock.get(), ...);
```

Unlike a unique_ptr, `unique_resource` works for non-pointer resources (file descriptors, OS handles). `std::experimental` for now; expected in a future standard.

# 5. Move-Only Resource Types

Owning resources usually means non-copyable, movable:

```cpp
class Mmap {
    void* ptr_ = nullptr;
    size_t len_ = 0;
public:
    Mmap(int fd, size_t len)
        : ptr_(::mmap(nullptr, len, PROT_READ, MAP_PRIVATE, fd, 0)), len_(len) {
        if (ptr_ == MAP_FAILED) throw std::system_error(errno, std::generic_category());
    }
    ~Mmap() { if (ptr_ && ptr_ != MAP_FAILED) ::munmap(ptr_, len_); }

    Mmap(const Mmap&) = delete;
    Mmap& operator=(const Mmap&) = delete;
    Mmap(Mmap&& o) noexcept
        : ptr_(std::exchange(o.ptr_, MAP_FAILED)), len_(std::exchange(o.len_, 0)) {}
    Mmap& operator=(Mmap&&) noexcept; // ditto, with cleanup of *this first
};
```

`std::exchange` is the idiomatic way to "take" a resource from another object while leaving it in a safe-to-destruct state.

# 6. Two-Phase Init: When You Can't Use a Constructor

Sometimes the resource needs information not available at construction time, or initialization can fail in ways the caller wants to inspect non-throwingly.

Options:

1. **Static factory returning `std::optional<T>` or `std::expected<T,E>`.**
   ```cpp
   class Connection {
       Connection(...) = default;
   public:
       static std::expected<Connection, NetErr> connect(std::string_view host);
   };
   ```
   Best of both worlds — the resulting `Connection` is fully constructed and valid.

2. **Two-phase: default-construct, then `init()`.** Worse — the type now has an invalid state. Reach for it only when forced (e.g., container default-init requirements).

# 7. Custom Deleters

Standard library calls (`fopen`/`fclose`, `pthread_mutex_init`/`destroy`) usually pair an init function with a destroy function. Custom deleters tie the pair together:

```cpp
struct FreeDeleter   { void operator()(void* p) const noexcept { std::free(p); } };
struct PcloseDeleter { void operator()(FILE* p) const noexcept { if (p) ::pclose(p); } };
struct DlcloseDeleter{ void operator()(void* h) const noexcept { if (h) ::dlclose(h); } };
```

Document the contract — does the deleter accept `nullptr`? Does it succeed unconditionally? In `noexcept` contexts? Most cleanup APIs are no-throw and null-safe; if not, wrap them defensively.

# 8. Resource Pools

When acquiring/releasing is expensive (DB connections, threads, GPU buffers), reuse instances:

```cpp
class ConnectionPool {
    std::mutex m_;
    std::vector<Connection> idle_;
    size_t inUse_ = 0;
    size_t cap_ = 16;
public:
    class Lease {
        ConnectionPool* pool_;
        Connection conn_;
    public:
        Lease(ConnectionPool* p, Connection c) : pool_(p), conn_(std::move(c)) {}
        ~Lease() { pool_->release(std::move(conn_)); }
        Connection& operator*() { return conn_; }
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
