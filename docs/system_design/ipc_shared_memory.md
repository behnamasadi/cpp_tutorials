# Inter-Process Communication and Shared Memory

When two processes need to talk, the right mechanism depends on volume, latency tolerance, sender/receiver topology, and whether they share a kernel. Shared memory is the fastest because the kernel is out of the data path; it's also the trickiest because it's the data path.

- [1. The IPC Mechanisms](#1-the-ipc-mechanisms)
- [2. Decision Matrix](#2-decision-matrix)
- [3. Pipes and FIFOs](#3-pipes-and-fifos)
- [4. Unix Domain Sockets](#4-unix-domain-sockets)
- [5. POSIX Shared Memory](#5-posix-shared-memory)
- [6. `mmap` and Memory-Mapped Files](#6-mmap-and-memory-mapped-files)
- [7. Boost.Interprocess](#7-boostinterprocess)
- [8. Synchronization in Shared Memory](#8-synchronization-in-shared-memory)
- [9. Lock-Free Queues in Shared Memory](#9-lock-free-queues-in-shared-memory)
- [10. Pitfalls](#10-pitfalls)

---

# 1. The IPC Mechanisms

| Mechanism | Topology | Throughput | Latency | Cross-machine |
|---|---|---|---|---|
| Anonymous pipe | Parent ↔ child | High | Med | No |
| Named pipe (FIFO) | Any | High | Med | No |
| Unix domain socket | Any | High | Low (~1–2 μs) | No |
| TCP socket | Any | Medium | High (~10–100 μs same host) | Yes |
| POSIX message queue | Many → one | Med | Low | No |
| Shared memory + mutex | Any | Very high | Very low (~100 ns) | No |
| `mmap` file | Any (read-mostly) | Very high | Very low | No (NFS unreliable) |
| dbus / dbus-broker | Buses, signals | Low | Med | No |
| `io_uring` | Within single process pair | Highest | Lowest | No |

# 2. Decision Matrix

| Need | Pick |
|---|---|
| Parent shells out to a child, captures output | Anonymous pipe (`popen`/`pipe`) |
| Two unrelated processes, modest traffic | Unix domain socket |
| Cross-machine | TCP / gRPC / nanomsg |
| Many small messages, low latency | Unix domain socket; UDS+SCM_RIGHTS for fd passing |
| Bulk data, low latency, both processes trusted | Shared memory |
| One writer, one reader, max throughput | SHM ring buffer (lock-free SPSC) |
| Producer-consumer with backpressure | SHM bounded queue |
| Read-only data shared by many readers | `mmap` of read-only file |
| Configuration via files | Just a file + signal/poll for changes |

# 3. Pipes and FIFOs

```cpp
#include <unistd.h>
#include <cstdio>

int main() {
    int p[2];
    pipe(p);                 // p[0] = read end, p[1] = write end

    if (fork() == 0) {       // child
        close(p[0]);
        write(p[1], "hello", 5);
        close(p[1]);
        _exit(0);
    }

    // parent
    close(p[1]);
    char buf[16] = {};
    int n = read(p[0], buf, sizeof(buf));
    close(p[0]);
    printf("got %d bytes: %s\n", n, buf);
}
```

Pipes carry an unstructured byte stream. Boundaries between writes are not preserved on the read side — reading 100 bytes from a pipe written as `"abc"` then `"def"` may return `"abcdef"`, `"abc"`, or `"a"`. Frame your messages.

Named pipes (FIFOs) are pipes with a filesystem path: `mkfifo /tmp/myfifo`, then `open()` on either end.

# 4. Unix Domain Sockets

The default IPC for unrelated processes on the same host. Identical API to TCP sockets, but:

- Faster (kernel never enters network stack).
- Can pass file descriptors between processes via `SCM_RIGHTS` ancillary data.
- Can carry `SOCK_DGRAM` (preserved message boundaries) or `SOCK_STREAM` (byte stream).
- Subject to filesystem permissions on the socket path.

```cpp
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>

int main() {
    int s = socket(AF_UNIX, SOCK_STREAM, 0);

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strcpy(addr.sun_path, "/tmp/myapp.sock");

    connect(s, (sockaddr*)&addr, sizeof(addr));
    write(s, "hi", 2);
    close(s);
}
```

Use `SOCK_SEQPACKET` if you want preserved message boundaries with stream-like ordering — Linux-specific but useful.

# 5. POSIX Shared Memory

```cpp
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

struct MySharedStruct {
    int counter;
    char message[64];
};

int main() {
    int fd = shm_open("/myseg", O_CREAT | O_RDWR, 0600);
    ftruncate(fd, sizeof(MySharedStruct));

    MySharedStruct* shared = (MySharedStruct*)mmap(
        nullptr, sizeof(MySharedStruct),
        PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    // Use *shared from any process that opens the same name.
    shared->counter = 42;

    // On exit:
    munmap(shared, sizeof(MySharedStruct));
    shm_unlink("/myseg");      // removes the named segment
}
```

Key points:
- The segment lives in `/dev/shm` on Linux, sized by `ftruncate`.
- Names start with `/` by convention and live in a flat namespace.
- The segment persists in the kernel until `shm_unlink` even if all processes exit.
- All processes mapping it see the same physical pages.

# 6. `mmap` and Memory-Mapped Files

`mmap` of a regular file is the same mechanism applied to disk-backed memory. Useful for:

- Loading large datasets where you'd otherwise allocate + read.
- Sharing read-only data across processes (the OS de-dupes pages).
- Custom databases (SQLite uses mmap optionally; LevelDB/RocksDB internally).

```cpp
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("data.bin", O_RDONLY);

    struct stat st;
    fstat(fd, &st);

    const char* p = (const char*)mmap(
        nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);                         // mapping persists

    // ... use p[0..st.st_size] like a normal array
    char first = p[0];
    (void)first;

    munmap((void*)p, st.st_size);
}
```

Gotchas:
- I/O errors become `SIGBUS` on access — there's no return code.
- `mmap` of files on NFS / network filesystems is not coherent; don't.
- 32-bit processes can't `mmap` big files.

# 7. Boost.Interprocess

Provides a portable, ergonomic abstraction over POSIX SHM and Windows file mappings, plus STL-compatible containers backed by shared memory:

```cpp
#include <boost/interprocess/managed_shared_memory.hpp>
namespace bip = boost::interprocess;

int main() {
    bip::managed_shared_memory shm(bip::open_or_create, "MyShared", 1 << 20);

    // Find a named int in the segment, or create one initialized to 0.
    int* counter = shm.find_or_construct<int>("counter")(0);
    *counter += 1;
}
```

It also gives you:
- `interprocess_mutex`, `interprocess_condition` — work across process boundaries.
- `vector`, `map`, `string` with allocators that target shared memory.
- Named, scoped, and anonymous SHM segments.

When to use Boost.IPC: portable, complex shared data structures with sane synchronization. When to roll your own: hot path with strict size or layout requirements.

# 8. Synchronization in Shared Memory

The thing nobody warns you about: standard `std::mutex` is **not** valid across processes. The mutex object lives in process-local memory and ties to a process-local thread.

For cross-process locks:
- **`pthread_mutex_t` with `PTHREAD_PROCESS_SHARED`**: place the mutex in shared memory, set the attribute, init.
- **`std::atomic` over shared memory**: works, *if* the type is lock-free (`is_always_lock_free`). Most platforms make `std::atomic<int>` lock-free; user-defined structs may not be.
- **Boost.Interprocess `interprocess_mutex`**: portable cross-process mutex.
- **Robust mutexes** (`PTHREAD_MUTEX_ROBUST`): the lock owner can crash without permanently locking the mutex; the next acquirer gets `EOWNERDEAD` and must restore invariants. Critical for any production SHM with separate failure domains.

```cpp
#include <pthread.h>

struct Shared {
    pthread_mutex_t mtx;
    int counter;
};

void init_shared_mutex(Shared* shared) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST);

    pthread_mutex_init(&shared->mtx, &attr);
    pthread_mutexattr_destroy(&attr);
}
```

# 9. Lock-Free Queues in Shared Memory

A common high-throughput IPC pattern: SPSC ring buffer in a shared memory segment.

```cpp
#include <atomic>
#include <cstddef>

struct alignas(64) SpscShmRing {
    static const size_t kCap      = 1 << 16;   // 65536 slots
    static const size_t kSlotSize = 64;        // bytes per slot

    std::atomic<size_t> head;                  // writer index
    char pad1[64 - sizeof(std::atomic<size_t>)];

    std::atomic<size_t> tail;                  // reader index
    char pad2[64 - sizeof(std::atomic<size_t>)];

    std::byte data[kCap * kSlotSize];
};
```

Two processes `mmap` the same segment, agree on slot size, and use the standard SPSC ring buffer protocol. See [Lock-Free Data Structures §4](../lock_free_data_structures.md#4-single-producer-single-consumer-spsc-ring-buffer).

Gotchas specific to SHM:
- Both processes must use the **same memory model** — agree on alignment, padding, and structure layout.
- The ring buffer's head/tail must be atomics on shared memory; ensure your `std::atomic` is lock-free for the type used.
- A crashing peer leaves the ring in an unknown state. Add a generation counter or a heartbeat.
- Don't store pointers — they're meaningful only in the writer's address space. Use offsets.

# 10. Pitfalls

**Pointer values across processes.** A pointer in process A's mapping doesn't dereference correctly in process B even if both map the same segment — addresses can differ. Use offsets relative to the segment base.

**Padding and alignment differences.** If both processes are built with the same compiler version and flags, fine. Mixing 32/64-bit or different STLs in shared structs is hopeless. Use POD types you fully control.

**Cleanup on crash.** A crashed peer doesn't run destructors. Mutexes locked, segments not unlinked. Use robust mutexes, watchdogs, or "garbage collect on next start" strategies.

**Names that persist.** `shm_open` creates persistent kernel objects. Forgetting `shm_unlink` (or having it skipped on crash) means stale segments accumulate in `/dev/shm`. Some systems auto-clean on reboot; production code should explicitly clean.

**Permissions.** Shared segments default to the creator's UID. Cross-user IPC requires `chmod` on the segment.

**Different ABI on each side.** If process A runs C++, process B runs Rust, you're communicating through bytes. Define the on-wire layout explicitly. Don't expose `std::string`.

**Trusting peer data.** Anything in shared memory is mutable by any peer with access. If peers don't fully trust each other, treat shared memory the same way as untrusted network input.

# References

- [Lock-Free Data Structures](../lock_free_data_structures.md)
- [Process, IPC, PCB](../process.md)
- [`shm_open(3)`](https://man7.org/linux/man-pages/man3/shm_open.3.html), [`mmap(2)`](https://man7.org/linux/man-pages/man2/mmap.2.html)
- [Boost.Interprocess docs](https://www.boost.org/doc/libs/release/doc/html/interprocess.html)
- *The Linux Programming Interface*, Michael Kerrisk — chapters 48–53.
- [eCAL](https://eclipse-ecal.github.io/ecal/) — open-source, SHM-based IPC framework with pub/sub, used in robotics.
