# Memory Layout of C/C++ Programs

- [Memory Layout of C/C++ Programs](#memory-layout-of-cc-programs)
  * [Overview diagram](#overview-diagram)
  * [Text (.text)](#text-text)
  * [Read-only data (.rodata)](#read-only-data-rodata)
  * [Data (.data)](#data-data)
  * [BSS (.bss)](#bss-bss)
  * [Heap](#heap)
  * [Stack](#stack)
  * [Inspecting the layout of a real binary](#inspecting-the-layout-of-a-real-binary)
  * [Why this matters in robotics / realtime code](#why-this-matters-in-robotics--realtime-code)

A Linux/ELF process splits its virtual address space into several regions. On a robot, knowing where each variable actually lives — flash-backed read-only memory, heap, or thread stack — is the difference between a deterministic 1 kHz control loop and a segfault during a hard stop.

## Overview diagram

Layout for a typical x86-64 Linux process (high addresses at the top). Stack grows down, heap grows up; the gap in between is filled by `mmap` regions (shared libs, large `malloc`s, memory-mapped device files).

```
 high address
 ---------------------------------
 |   kernel space (not yours)    |
 ---------------------------------
 |   env vars + argv strings     |
 ---------------------------------
 |              STACK            |
 |-------------------------------| <- frame pointer       ┐
 |        Return Address         |                        |
 |        Saved registers        |                        |  current stack frame
 |        Local variables        |                        |
 |        Spilled arguments      |                        |
 |-------------------------------| <- stack pointer       ┘
 |              | grows down     |
 |              v                |
 |                               |
 |   mmap region (libs, large    |
 |   mallocs, /dev/mem, shared   |
 |   memory, framebuffers, …)    |
 |                               |
 |              ^                |
 |              | grows up       |
 |              HEAP             |
 |-------------------------------|
 |  .bss (zero-initialized)      |  uninitialized globals/statics
 |-------------------------------|
 |  .data (initialized)          |  globals/statics with nonzero initializer
 |-------------------------------|
 |  .rodata (read-only)          |  string literals, const tables, vtables
 |-------------------------------|
 |  .text (code, read+execute)   |  <- PC points somewhere in here
 ---------------------------------
 low address
```

`size(1)` reports three columns:
- `text` — `.text` + `.rodata` + a few small read-only sections
- `data` — `.data` and friends
- `bss`  — `.bss`

So when you add a string literal you'll see `text` grow, not `data`.

## Text (.text)

Machine instructions live here. The pages are mapped **read + execute, no write**, so a stray write through a function pointer faults instead of corrupting code. Optimization, inlining, and `-fno-omit-frame-pointer` all change what shows up in `.text`, but never its read-only status.

```cpp
int main() { return 0; }
```
```
text    data    bss     dec     hex  filename
1806    544     8       2358    936  layout
```

Add a function and `.text` grows:
```cpp
void foo() {}
int main() { return 0; }
```
```
text    data    bss     dec     hex  filename
1915    544     8       2467    9a3  layout
```

## Read-only data (.rodata)

String literals, `const` globals, jump tables, and C++ vtables/typeinfo land in `.rodata`. The pages are read-only but **not** executable. `size` lumps `.rodata` into the `text` column, which is what trips most people up.

```cpp
int main() {
    char* p1 = "behnam";  // "behnam\0" lives in .rodata; p1 is on the stack
    return 0;
}
```
```
text    data    bss     dec     hex  filename
1829    544     8       2381    94d  layout
```

Writing through `p1` segfaults because `.rodata` is read-only:
```cpp
p1[0] = 'C';   // compiles, SIGSEGV at runtime
```
This is also why modern compilers warn (or error with `-Wwrite-strings`) when you assign a literal to `char*` instead of `const char*`.

Reassigning the pointer itself is fine — the pointer variable lives on the stack:
```cpp
p1 = "bar";              // OK
char* const p2 = "foo";
p2 = "bar";              // error: assignment of read-only variable
```

In robotics this matters for things like fixed lookup tables (CRC polynomials for CAN frames, sin/cos LUTs for motor commutation, Denavit–Hartenberg parameters) — declare them `constexpr`/`const` so they go to `.rodata` and stay out of cache lines that get dirtied.

## Data (.data)

Globals and `static` locals with a **nonzero** initializer. The image on disk carries the actual bytes, and the loader copies them into a writable page at startup.

```cpp
static int global_static = 1;
int        global_var    = 1;

int main() {
    static int local_static = 1;
}
```

Compared to the empty-`main` baseline:
```
text    data    bss     dec     hex
1855    556     4       2415    96f
```
`data` went up by 12 bytes (three `int`s, padding included); `bss` actually shrank because the compiler can fold/relocate small things differently when more sections exist — exact bytes are toolchain-dependent.

A robotics gotcha: a non-trivial global object (`std::vector<float> trajectory{0, 0.1f, 0.2f, …};`) means its constructor runs **before `main`**, in unspecified order across translation units (the "static initialization order fiasco"). For anything touching hardware, prefer initializing inside `main` (or a `Robot::init()`) so you control the order.

## BSS (.bss)

"Block Started by Symbol" — uninitialized (or zero-initialized) globals and statics. The ELF image stores only the *size*, not the bytes; the kernel hands you zeroed pages on first touch. That's why a 100 MB zero-initialized buffer costs nothing on disk.

```cpp
static int global_static;
int        global_var;

int main() {
    static int local_static;
}
```

C and C++ both guarantee these are zero at program start, so don't write `int counter = 0;` at file scope thinking you're being explicit — it actually moves the variable from `.bss` to `.data` and bloats the binary.

## Heap

Returned by `malloc` / `new` / `mmap`. Grows up toward the mmap region. The allocator (glibc's ptmalloc, jemalloc, mimalloc, …) manages free lists on top of `brk` and `mmap` syscalls; large allocations (typically >128 KB) go straight to `mmap` and bypass the brk-extended heap entirely.

```cpp
float* trajectory = new float[1000];   // somewhere in the heap
delete[] trajectory;                   // returned to the allocator, not necessarily to the OS
```

Things to watch for in real systems:

- **Fragmentation.** Long-running control nodes that alloc/free variable-sized buffers (point clouds, image frames) can keep the resident set high even after `free`, because freed chunks aren't necessarily contiguous. Reuse buffers, or use pool/arena allocators.
- **Latency.** `malloc` can block on a mutex, can page-fault, can even call into the kernel. None of that is acceptable inside a 1 kHz control loop — preallocate at init time, then never allocate on the hot path. `mlockall(MCL_CURRENT | MCL_FUTURE)` after that prevents the heap from being paged out.
- **Leaks.** Detect with Valgrind (`memcheck`) or AddressSanitizer (`-fsanitize=address`). See [memory_leaking_valgrind.md](memory_leaking_valgrind.md).
- **Use-after-free / double-free.** AddressSanitizer catches these at runtime far more reliably than Valgrind, at a ~2× slowdown.

For deeper coverage see [dynamic_memory_allocation.md](dynamic_memory_allocation.md) and [track_memory_allocations_overriding_new_operator.md](track_memory_allocations_overriding_new_operator.md).

## Stack

Each thread gets its own stack. On Linux the default for the main thread is 8 MB (`ulimit -s`); `pthread_create` defaults are smaller (often 2 MB) unless set via `pthread_attr_setstacksize`. The kernel reserves a guard page just below the stack; if your code writes past it (deep recursion, a huge `alignas(64) float buf[1<<20];` local) you get SIGSEGV — see [stack_overflow.md](stack_overflow.md).

Each function call pushes a **stack frame** containing:
1. Return address
2. Saved callee-saved registers
3. Local variables
4. Arguments that didn't fit in registers (on x86-64 SysV: 7th+ integer args, after rdi/rsi/rdx/rcx/r8/r9)
5. Padding for 16-byte alignment before the next `call`

Two pointers track this:
- **Stack pointer (rsp)** — top of the stack (lowest valid address since it grows down). The boundary between allocated and unallocated frame space.
- **Frame pointer (rbp)** — pinned to the start of the current frame so locals can be addressed at fixed offsets like `[rbp-8]`. With `-fomit-frame-pointer` (the default at `-O1+`) the compiler skips this and addresses everything off `rsp`, freeing up a register at the cost of slightly harder unwinding.

The stack is the right place for short-lived, fixed-size data: pose matrices, small ring-buffer indices, the `JointState` you're about to publish. Allocation is free (just an `rsp` subtraction) and deallocation is automatic on return — exactly what you want in a realtime loop.

What does **not** belong on the stack:
- Large buffers (>~64 KB on a pthread). Put them in `.bss`, the heap, or a pool.
- Variable-length arrays (`int buf[n]`) — C99-only, banned in C++, and a stack-overflow footgun if `n` comes from a sensor.
- Anything you need to outlive the function (return a pointer to a local → dangling reference).

For more on RAII, unwinding, and exception safety see [stack_unwinding.md](stack_unwinding.md).

## Inspecting the layout of a real binary

```bash
size ./robot_node                          # text/data/bss summary
nm  -S --size-sort ./robot_node            # symbols + sizes, sorted
readelf -S ./robot_node                    # all sections with flags (A/W/X)
objdump -h ./robot_node                    # similar, plus VMA/LMA
cat /proc/$(pidof robot_node)/maps         # live virtual memory map of a running process
pmap -x $(pidof robot_node)                # resident vs. virtual per region
```

`readelf -S` is the one to reach for when something "feels like it should be const" but the binary keeps growing — check whether the symbol landed in `.rodata` (flag `A`) or `.data` (flag `WA`).

## Why this matters in robotics / realtime code

- **Determinism:** `.text`/`.rodata`/`.data`/`.bss` sizes are fixed at link time → no runtime allocation, no fragmentation, no page faults after `mlockall`. The hot path of a controller should touch only stack + preallocated heap.
- **Crashes that look like cosmic rays:** writing through a `char*` to a string literal, or returning a pointer to a stack local, will segfault on a robot in the field but might look fine on your dev box if the page happens to be writable in some build configs. Build with `-fsanitize=address,undefined` in CI.
- **Binary size on flash-constrained targets:** moving `const` tables to `.rodata` (vs leaving them as runtime-initialized globals) shrinks `.data`, which in turn shrinks the loaded image and the time-to-`main`.
- **Shared memory IPC:** ROS 2 with `rmw_iceoryx`, or hand-rolled `shm_open` between a realtime node and a logger, places objects in an `mmap`-backed region between the heap and the stack. Those addresses show up in `/proc/<pid>/maps` as a separate range — useful when debugging "why does this pointer not work in the other process."
