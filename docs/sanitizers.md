# Sanitizers (ASan, TSan, UBSan, MSan, LSan)

Sanitizers are **compile-time instrumentation** that detect bugs at runtime. The compiler rewrites your program — adding shadow memory, lock tracking, integer-overflow checks, and so on — and the resulting binary refuses to misbehave silently. When something goes wrong it prints a precise diagnostic with stack traces and exits.

Compared to Valgrind's purely dynamic instrumentation, sanitizers are roughly an order of magnitude faster (Valgrind's Memcheck is ~20–50× slowdown; ASan is ~2×). The tradeoff is that you have to rebuild your code with the right flag — but for a developer who controls the build, that's almost always the right choice.

This doc supersedes the brief ASan coverage in [memory_leaking_valgrind.md](memory_leaking_valgrind.md). Use Valgrind when you can't recompile (closed-source binary, exotic toolchain); use sanitizers everywhere else.

- [1. AddressSanitizer (ASan)](#1-addresssanitizer-asan)
- [2. ThreadSanitizer (TSan)](#2-threadsanitizer-tsan)
- [3. UndefinedBehaviorSanitizer (UBSan)](#3-undefinedbehaviorsanitizer-ubsan)
- [4. MemorySanitizer (MSan)](#4-memorysanitizer-msan)
- [5. LeakSanitizer (LSan)](#5-leaksanitizer-lsan)
- [6. CMake integration](#6-cmake-integration)
- [7. Compatibility table](#7-compatibility-table)
- [8. Caveats and gotchas](#8-caveats-and-gotchas)

---

# 1. AddressSanitizer (ASan)

**Flag:** `-fsanitize=address -fno-omit-frame-pointer -g`

Catches the entire family of memory-corruption bugs:

| Bug                       | What it means                                            |
|---------------------------|----------------------------------------------------------|
| Heap-buffer-overflow      | Reading/writing past the end of `new`/`malloc`'d memory  |
| Stack-buffer-overflow     | Same, on a local array                                   |
| Global-buffer-overflow    | Same, on a `static`/global array                         |
| Use-after-free            | Touching memory after `delete`/`free`                    |
| Use-after-return          | Returning a pointer to a local that's now gone           |
| Double-free               | `delete` or `free` called twice on the same pointer      |
| Memory leak               | Allocation not freed by program exit (LSan, bundled)     |

How it works: every allocation is surrounded by **redzones** of poisoned bytes. ASan maintains a parallel "shadow memory" that records, for every 8 bytes of real memory, whether they're addressable. Each load/store goes through a fast inline check against the shadow.

## Heap-buffer-overflow example

```cpp
// bug.cpp
#include <cstdlib>
int main() {
  int* a = new int[5];
  a[5] = 42;          // one past the end
  delete[] a;
}
```

```bash
g++ -fsanitize=address -fno-omit-frame-pointer -g bug.cpp -o bug && ./bug
```

Output (abridged):

```
==12345==ERROR: AddressSanitizer: heap-buffer-overflow on address 0x602000000034
WRITE of size 4 at 0x602000000034 thread T0
    #0 0x... in main bug.cpp:4
    #1 0x... in __libc_start_main
0x602000000034 is located 0 bytes after 20-byte region [0x602000000020,0x602000000034)
allocated by thread T0 here:
    #0 0x... in operator new[](unsigned long)
    #1 0x... in main bug.cpp:3
SUMMARY: AddressSanitizer: heap-buffer-overflow bug.cpp:4 in main
```

The diagnostic tells you the **type of bug**, the **exact line**, the **allocation site**, and how far past the buffer you wrote.

## Use-after-free

```cpp
int* a = new int[5];
delete[] a;
a[0] = 1;             // ASan: "heap-use-after-free"
```

ASan keeps freed memory **quarantined** for a while so a use-after-free doesn't get masked by a fresh allocation reusing the address.

# 2. ThreadSanitizer (TSan)

**Flag:** `-fsanitize=thread -g`

Detects **data races**: two threads accessing the same memory location without synchronization, where at least one is a write. Also catches a few flavors of misused locks (e.g., destroying a mutex while another thread holds it).

How it works: TSan tracks vector clocks for every memory location and every synchronization event (mutex lock, atomic store, thread join). At each access it checks the clock of the last writer against the current thread's clock. If they're not ordered by happens-before, that's a race.

## Racing example

```cpp
// race.cpp
#include <thread>
#include <iostream>

int counter = 0;                              // unsynchronized

void inc() { for (int i = 0; i < 100000; ++i) ++counter; }

int main() {
  std::thread t1(inc), t2(inc);
  t1.join(); t2.join();
  std::cout << counter << '\n';               // not 200000
}
```

```bash
g++ -fsanitize=thread -g race.cpp -o race -pthread && ./race
```

Output (abridged):

```
WARNING: ThreadSanitizer: data race (pid=12345)
  Write of size 4 at 0x55... by thread T2:
    #0 inc() race.cpp:6
  Previous write of size 4 at 0x55... by thread T1:
    #0 inc() race.cpp:6
  Location is global 'counter' of size 4 at 0x55...
SUMMARY: ThreadSanitizer: data race race.cpp:6 in inc()
```

Fix: make `counter` an `std::atomic<int>`, or guard it with a mutex.

TSan also flags **lock-order inversions** (potential deadlocks): if thread A locks `m1` then `m2`, and thread B locks `m2` then `m1`, TSan warns even if the actual deadlock didn't happen this run.

# 3. UndefinedBehaviorSanitizer (UBSan)

**Flag:** `-fsanitize=undefined -g`

The cheapest sanitizer to enable (~5–20% slowdown) and the one with the best ratio of bugs found per minute of effort. Catches:

- Signed integer overflow
- Shift past bit width
- Division by zero
- Null pointer dereference and method-on-null
- Misaligned pointer load/store (see [align.md](align.md))
- Reaching the end of a non-`void` function without `return`
- `enum` value out of range
- Floating-point cast overflow
- Invalid `bool` (a `bool` containing something other than 0 or 1)

## Example

```cpp
// ub.cpp
#include <climits>
int main() {
  int x = INT_MAX;
  int y = x + 1;       // signed overflow — UB
  return y;
}
```

```bash
g++ -fsanitize=undefined -g ub.cpp -o ub && ./ub
```

```
ub.cpp:4:13: runtime error: signed integer overflow:
    2147483647 + 1 cannot be represented in type 'int'
```

By default UBSan prints and continues. Add `-fno-sanitize-recover=undefined` to make it abort on first hit (better for CI).

A useful sub-flag set for a strict CI build:

```bash
-fsanitize=undefined,integer,nullability -fno-sanitize-recover=all
```

# 4. MemorySanitizer (MSan)

**Flag:** `-fsanitize=memory -fno-omit-frame-pointer -g` *(Clang only)*

Detects **uses of uninitialized memory**. Different from ASan, which only tracks *whether memory is addressable* — MSan tracks *whether each bit has been initialized*.

```cpp
// msan.cpp
#include <cstdio>
int main() {
  int a[10];
  if (a[3] == 42)            // reading uninitialized
    printf("hit\n");
}
```

```bash
clang++ -fsanitize=memory -fno-omit-frame-pointer -g msan.cpp -o msan && ./msan
```

```
==12345==WARNING: MemorySanitizer: use-of-uninitialized-value
    #0 0x... in main msan.cpp:4
```

**Big caveat:** MSan needs **every library you link** to be MSan-instrumented, including the C++ standard library. In practice this means building libc++ from source with MSan flags, or using a prebuilt MSan-instrumented libc++. For most teams this is heavy enough that they reach for Valgrind's Memcheck instead — slower but no rebuild of dependencies.

# 5. LeakSanitizer (LSan)

**Flag:** `-fsanitize=leak` *(or get it for free as part of ASan)*

At program exit, walks the heap and reports any allocations that no live pointer can reach.

```cpp
// leak.cpp
#include <cstdlib>
int main() {
  int* p = (int*)malloc(40);
  p = nullptr;               // last reference dropped — leak
}
```

```bash
g++ -fsanitize=address -g leak.cpp -o leak && ./leak
```

```
==12345==ERROR: LeakSanitizer: detected memory leaks
Direct leak of 40 byte(s) in 1 object(s) allocated from:
    #0 0x... in malloc
    #1 0x... in main leak.cpp:4
SUMMARY: AddressSanitizer: 40 byte(s) leaked in 1 allocation(s).
```

LSan distinguishes **direct leaks** (no pointer to the block exists anywhere) from **indirect leaks** (the block is only reachable through another leaked block — fixing the parent leak frees these too).

To suppress known third-party leaks, set `LSAN_OPTIONS=suppressions=lsan.supp` with patterns like `leak:libfoo.so`.

# 6. CMake integration

Wire the sanitizer to a build option so you can turn it on per-config without editing the source:

```cmake
option(SANITIZER "Sanitizer to enable: address|thread|undefined|memory|leak|none" none)

if(NOT SANITIZER STREQUAL "none")
  add_compile_options(-fsanitize=${SANITIZER} -fno-omit-frame-pointer -g)
  add_link_options(-fsanitize=${SANITIZER})
endif()
```

Then build with:

```bash
cmake -B build -DSANITIZER=address && cmake --build build
cmake -B build-tsan -DSANITIZER=thread && cmake --build build-tsan
```

Many projects keep separate build directories per sanitizer (`build-asan/`, `build-tsan/`, `build-ubsan/`) and run the test suite under each in CI.

A common pattern is a debug build that always has UBSan and ASan on:

```cmake
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()
```

# 7. Compatibility table

Sanitizers don't all play nice together. They each rewrite memory layout in incompatible ways:

| Combo                | Works? | Notes                                              |
|----------------------|:------:|----------------------------------------------------|
| ASan + UBSan         | yes    | The most useful pairing. Most projects ship this.  |
| ASan + LSan          | yes    | LSan is bundled into ASan by default.              |
| TSan + UBSan         | yes    | Common in multi-threaded codebases.                |
| MSan + UBSan         | yes    | Clang only.                                        |
| **ASan + TSan**      | no     | Both reserve large virtual memory regions.         |
| **ASan + MSan**      | no     | Conflicting shadow-memory layouts.                 |
| **TSan + MSan**      | no     | Same reason.                                       |
| **MSan + LSan**      | no     | LSan needs ASan-style allocator.                   |

In practice most teams run **two CI configurations**: one with `-fsanitize=address,undefined` and one with `-fsanitize=thread,undefined`. That covers the bulk of bugs without conflicting flags.

# 8. Caveats and gotchas

### Performance

| Sanitizer | Slowdown   | Memory overhead |
|-----------|------------|-----------------|
| ASan      | ~2×        | ~3× RSS         |
| TSan      | ~5–15×     | ~5–10× RSS      |
| UBSan     | ~1.05–1.2× | negligible      |
| MSan      | ~3×        | ~2–3× RSS       |
| LSan      | negligible | small           |

ASan is fast enough to run your full test suite. TSan is fast enough for unit tests but often too slow for end-to-end tests on a large system.

### Frame pointers and symbolization

Always pass `-fno-omit-frame-pointer -g`. Without frame pointers the stack traces are unreliable; without `-g` they're just hex addresses. For optimized builds, keep `-O1` or higher — sanitizers' overhead compounds badly with `-O0`.

If your stack traces show `??:0`, install `llvm-symbolizer` and point ASan at it: `export ASAN_SYMBOLIZER_PATH=$(which llvm-symbolizer)`.

### Useful environment variables

```bash
export ASAN_OPTIONS=detect_leaks=1:halt_on_error=1:abort_on_error=1
export TSAN_OPTIONS=halt_on_error=1:second_deadlock_stack=1
export UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1
```

`abort_on_error=1` is essential under a debugger — without it ASan calls `_exit` and your debugger never gets a chance to break.

### Things that break

- **`mlockall(MCL_FUTURE)`** — sanitizers need to map gigabytes of shadow memory after startup; locking all future maps fails. Drop the call when sanitizing.
- **`ulimit -v`** — same reason; the virtual address reservation looks huge (terabytes) but it's mostly unbacked. Don't cap virtual memory.
- **Custom allocators** — replacing `operator new` or hooking `malloc` confuses ASan/MSan/LSan, which install their own. Either disable your custom allocator in sanitizer builds or use the documented interceptor API.
- **`fork` without `exec`** — TSan's runtime isn't async-signal-safe; multi-process programs sometimes need `TSAN_OPTIONS=die_after_fork=0`.
- **Old kernels / unusual ASLR settings** — ASan reserves a fixed shadow region; if the kernel's ASLR places code there, ASan aborts at startup. Modern Linux is fine.
- **Hardware-assisted ASan (HWASan)** on AArch64 is a lower-overhead alternative (~15% slowdown) when targeting that architecture.

### Sanitizers are not a substitute for tests

Sanitizers only catch bugs **on code paths that actually execute**. A use-after-free on an error path nobody hits in CI is invisible. Pair sanitizer-enabled builds with high-coverage tests and fuzzing (libFuzzer + ASan is a particularly strong combination).

# References

- [AddressSanitizer wiki](https://github.com/google/sanitizers/wiki/AddressSanitizer)
- [ThreadSanitizer wiki](https://github.com/google/sanitizers/wiki/ThreadSanitizerCppManual)
- [UBSan in the Clang manual](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html)
- [MemorySanitizer wiki](https://github.com/google/sanitizers/wiki/MemorySanitizer)
- Related: [memory_leaking_valgrind.md](memory_leaking_valgrind.md), [align.md](align.md)
