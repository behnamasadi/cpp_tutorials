# Memory Management Strategies

`new`/`delete` (or `malloc`/`free`) are general-purpose allocators. They handle any size, any lifetime, any thread — and pay for that flexibility. When you know more about your allocation pattern, custom allocators give 5–100× speedups.

- [1. Why Custom Allocators](#1-why-custom-allocators)
- [2. Bump / Linear / Arena Allocator](#2-bump--linear--arena-allocator)
- [3. Stack Allocator](#3-stack-allocator)
- [4. Pool Allocator](#4-pool-allocator)
- [5. Slab Allocator](#5-slab-allocator)
- [6. Free-List Allocator](#6-free-list-allocator)
- [7. `std::pmr` (C++17)](#7-stdpmr-c17)
- [8. Per-Thread / Per-Frame Allocators](#8-per-thread--per-frame-allocators)
- [9. Choosing One](#9-choosing-one)
- [10. Pitfalls](#10-pitfalls)

---

# 1. Why Custom Allocators

Stock `malloc` is slower than necessary because it has to:
- Find a free block of the right size from a global heap.
- Synchronize across threads.
- Handle arbitrary deallocation order.
- Defragment over time.
- Return memory to the OS occasionally.

If your allocation pattern is *narrower* than this — fixed-size objects, scope-bounded lifetime, single-thread — you can drop almost all of that overhead.

Typical wins:
- Game engines per-frame allocations: 100× over `malloc`.
- Object pools for hot game objects: 10–50×.
- Arena allocators for short-lived parsing: 5–20×.

# 2. Bump / Linear / Arena Allocator

Maintain a single buffer and a pointer. `allocate(n)` advances the pointer; **deallocation is a no-op**. The whole arena is reset at once.

```cpp
class Arena {
    char* buf_;
    size_t cap_;
    size_t pos_ = 0;
public:
    explicit Arena(size_t cap) : buf_(static_cast<char*>(::operator new(cap))), cap_(cap) {}
    ~Arena() { ::operator delete(buf_); }

    void* allocate(size_t n, size_t align) {
        size_t p = (pos_ + align - 1) & ~(align - 1);
        if (p + n > cap_) throw std::bad_alloc();
        pos_ = p + n;
        return buf_ + p;
    }
    void reset() { pos_ = 0; }   // O(1) "free everything"
};
```

Best for: parsing (each parse pass uses an arena, dropped at the end), per-frame allocations in a game loop, request-scoped allocations in a server.

Worst for: long-lived objects with mixed lifetimes — you'd have to keep them around forever.

# 3. Stack Allocator

A bump allocator that supports LIFO deallocation: each `allocate` returns a marker, `deallocate(marker)` rolls back to it.

```cpp
class StackAllocator {
    char* buf_; size_t cap_; size_t pos_ = 0;
public:
    using Marker = size_t;
    Marker mark() const { return pos_; }
    void rollback(Marker m) { pos_ = m; }
    void* allocate(size_t n, size_t align) { /* same as Arena */ }
};
```

Game engines often have a per-thread stack allocator that's pushed/popped around stages of the frame.

# 4. Pool Allocator

Fixed-size, fast `allocate` and `deallocate`. Maintain a free list of equal-size blocks:

```cpp
template<size_t BlockSize>
class PoolAllocator {
    struct FreeNode { FreeNode* next; };
    FreeNode* free_ = nullptr;
    std::vector<std::unique_ptr<char[]>> chunks_;
    size_t chunkBlocks_;

    void grow() {
        auto chunk = std::make_unique<char[]>(BlockSize * chunkBlocks_);
        for (size_t i = 0; i < chunkBlocks_; ++i) {
            auto* node = reinterpret_cast<FreeNode*>(chunk.get() + i * BlockSize);
            node->next = free_; free_ = node;
        }
        chunks_.push_back(std::move(chunk));
    }
public:
    explicit PoolAllocator(size_t chunkBlocks = 1024) : chunkBlocks_(chunkBlocks) {}
    void* allocate() {
        if (!free_) grow();
        auto* p = free_; free_ = free_->next; return p;
    }
    void deallocate(void* p) {
        auto* node = static_cast<FreeNode*>(p);
        node->next = free_; free_ = node;
    }
};
```

Used for: object pools (`Bullet`, `Particle`), node-based containers (`std::list`, `std::map` nodes), connection objects.

# 5. Slab Allocator

Pool allocator extended to multiple size classes. Each "slab" is a pool for a particular size; the allocator dispatches by size. The Linux kernel uses this; it's the SLAB/SLUB allocator family.

```cpp
class SlabAllocator {
    PoolAllocator<16>   p16;
    PoolAllocator<32>   p32;
    PoolAllocator<64>   p64;
    PoolAllocator<128>  p128;
public:
    void* allocate(size_t sz) {
        if (sz <= 16)  return p16.allocate();
        if (sz <= 32)  return p32.allocate();
        if (sz <= 64)  return p64.allocate();
        if (sz <= 128) return p128.allocate();
        return ::operator new(sz);   // fall back for big requests
    }
};
```

# 6. Free-List Allocator

Variable-sized blocks, maintained in a sorted free list with coalescing on free. This is what `malloc` is, more or less. You'd write your own when you need control over fragmentation and locality but can't bound sizes.

Use this only when bumps/pools/slabs don't fit. The implementation has fragmentation, coalescing, and split logic; it's worth pulling in [`mimalloc`](https://github.com/microsoft/mimalloc) or [`jemalloc`](https://jemalloc.net/) instead.

# 7. `std::pmr` (C++17)

Polymorphic memory resources standardize the allocator interface so any container can use any allocator at runtime — without the allocator being part of the container's type.

```cpp
#include <memory_resource>

std::array<std::byte, 4096> buf;
std::pmr::monotonic_buffer_resource arena{buf.data(), buf.size()};
std::pmr::vector<std::pmr::string> v{&arena};
v.emplace_back("hello");   // both vector and string allocate from arena
```

Available resources:
- `monotonic_buffer_resource` — bump/arena.
- `unsynchronized_pool_resource` / `synchronized_pool_resource` — pool with size classes.
- `null_memory_resource` — throws on any allocation (useful in `noexcept` paths).
- Custom: derive from `std::pmr::memory_resource`.

`std::pmr` is the right answer when you want allocator strategy without templating every container on it.

# 8. Per-Thread / Per-Frame Allocators

Thread-local arenas eliminate synchronization:

```cpp
thread_local Arena g_per_thread_arena{1 << 20};

void* tls_alloc(size_t n, size_t a) { return g_per_thread_arena.allocate(n, a); }
void  tls_reset()                   { g_per_thread_arena.reset(); }
```

In a server, reset between requests. In a game, reset between frames. The release happens at the boundary, so no per-allocation tracking is needed.

# 9. Choosing One

| Pattern | Use |
|---|---|
| Many short-lived allocations of varying size, all freed together | **Arena / bump** |
| Many same-size objects, individual deallocation | **Pool** |
| Multiple size classes, individual deallocation | **Slab** or `pmr::pool_resource` |
| LIFO scope-based allocations | **Stack allocator** |
| Per-thread short-lived | `thread_local Arena` |
| Generic, can't predict | Stock `malloc` / `mimalloc` / `jemalloc` |

Profile *before* writing a custom allocator. The overhead of bad locality from a fast allocator can erase the speedup of "fast allocate."

# 10. Pitfalls

**Forgetting alignment.** `allocate(n, alignof(T))` — not just `allocate(n)`. Misaligned access is UB on many platforms and slow on x86.

**Calling destructors.** Custom allocators give you raw memory. You still need to construct (`new (p) T(...)`) and destroy (`p->~T()`) — `std::pmr` and PMR-aware containers handle this automatically.

**Mixed allocators.** An object allocated from arena A but freed via `delete` (which calls `::operator delete`) corrupts the heap. Pair allocate with the matching deallocate.

**Holding pointers across an arena reset.** Resetting an arena invalidates every pointer into it. Every. One. Don't store these in long-lived data structures.

**Concurrent access to a single-threaded arena.** Bump allocators are not thread-safe. Add a mutex (slow) or use per-thread arenas (fast).

**Forgetting the OS still has to commit.** A 1-GB arena that you reset between frames may not actually return RSS to the OS. Use `madvise(MADV_DONTNEED)` or a tighter cap.

# References

- [Allocator](../allocator.md)
- [Track memory allocations](../track_memory_allocations_overriding_new_operator.md)
- [`std::pmr` cppreference](https://en.cppreference.com/w/cpp/header/memory_resource)
- *Game Engine Architecture*, Jason Gregory — chapter on memory allocators is excellent.
- *Building Your Own Memory Allocator*, talk by Howard Hinnant.
- [mimalloc](https://github.com/microsoft/mimalloc), [jemalloc](https://jemalloc.net/), [tcmalloc](https://github.com/google/tcmalloc) — production general-purpose allocators.
