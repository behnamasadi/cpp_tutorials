# `-fPIC` and `-fPIE`

- [The one-sentence version](#the-one-sentence-version)
- [Why position-independent code exists](#why-position-independent-code-exists)
- [fPIC: for shared libraries](#fpic-for-shared-libraries)
- [fPIE: for executables (security/ASLR)](#fpie-for-executables-securityaslr)
- [The error you'll actually hit](#the-error-youll-actually-hit)
- [CMake: how to set it](#cmake-how-to-set-it)
- [Real-world scenarios](#real-world-scenarios)
- [Seeing the difference in assembly](#seeing-the-difference-in-assembly)

## The one-sentence version

- **`-fPIC`** — compile this code so it works no matter what address the loader puts
  it at. Required for shared libraries (`.so`).
- **`-fPIE`** — same idea, but for the main executable, so the OS can randomize its
  load address (ASLR). A hardening flag.

## Why position-independent code exists

A shared library on disk has no idea where in a process's address space it will end
up. Process A might map `libfoo.so` at `0x7f1234...`, process B at `0x7f5678...`. If
the compiler had baked absolute addresses into the code ("jump to function at
`0x401050`"), every load would need a fix-up pass to patch those addresses — and
worse, each process would need its *own* private copy of the patched code, defeating
the whole point of sharing.

Position-independent code sidesteps this by using **relative addressing**:
"jump to the function 0x40 bytes after wherever I am right now." No fix-ups, one
copy in physical memory, mapped into many processes.

## fPIC: for shared libraries

Rule of thumb: **if it ends up inside a `.so`, it must be built with `-fPIC`.** That
includes static libraries you intend to link *into* a shared library — they have to
be PIC too, otherwise the final `.so` link will fail.

```bash
# Compile object files as PIC
g++ -fPIC -c motor_driver.cpp -o motor_driver.o
g++ -fPIC -c encoder.cpp      -o encoder.o

# Link into a shared library
g++ -shared -o libmotors.so motor_driver.o encoder.o
```

CMake does this automatically for `SHARED` libraries, but **not** for `STATIC` ones
— see the [CMake section](#cmake-how-to-set-it) below.

## fPIE: for executables (security/ASLR)

`-fPIE` is `-fPIC` applied to the main program. The linker companion is `-pie`. With
both, the kernel can map the executable itself to a random base address every run, so
an attacker who knows offsets inside your binary still can't predict absolute
addresses. Most modern distros (Ubuntu, Debian, Fedora) default to PIE for everything.

```bash
g++ -fPIE -pie -o robot_control main.cpp
```

You generally don't write this by hand — your distro's compiler is already
configured this way. You only think about `-fPIE` when *disabling* it (e.g.
`-no-pie`) for tools that need a fixed load address, or when explicitly hardening a
build.

## The error you'll actually hit

The classic moment people learn what `-fPIC` is:

```text
/usr/bin/ld: libutils.a(crc.o): relocation R_X86_64_32 against `.rodata'
    can not be used when making a shared object; recompile with -fPIC
/usr/bin/ld: failed to set dynamic section sizes: bad value
collect2: error: ld returned 1 exit status
```

Translation: you tried to link a static library (`libutils.a`) into a shared library
(`libdriver.so`), but `libutils.a` was built without `-fPIC`. The fix is to rebuild
`libutils.a` with `-fPIC`. There is no way around it on x86-64.

## CMake: how to set it

```cmake
# Apply to every target in the project (simplest, recommended)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
```

```cmake
# Or apply it to a single target — useful for a static lib that
# gets pulled into a shared lib downstream.
add_library(robot_utils STATIC src/crc.cpp src/log.cpp)
set_target_properties(robot_utils PROPERTIES POSITION_INDEPENDENT_CODE ON)
```

```bash
# Or pass it on the command line at configure time
cmake -DCMAKE_POSITION_INDEPENDENT_CODE=ON ..
```

Notes:

- `add_library(foo SHARED ...)` enables PIC automatically.
- `add_library(foo STATIC ...)` does **not** — you must opt in if it'll be linked
  into a shared library.
- `add_executable(...)` does not enable PIE just from
  `CMAKE_POSITION_INDEPENDENT_CODE`. For PIE executables on older CMake, you need
  the linker flag too; modern CMake (≥3.14) propagates it correctly for executables
  when the variable is on.

## Real-world scenarios

**1. Plugin system in a robotics stack.** Your motion planner loads
`libtrajectory_rrt.so` and `libtrajectory_lattice.so` at runtime via `dlopen` so users
can swap planners without rebuilding. Every planner plugin must be `-fPIC` — they're
shared objects loaded into a process that doesn't know their layout in advance.

**2. Driver library that wraps a vendor static lib.** You're shipping
`libcan_driver.so`, and it depends on the vendor's `libpeak_can.a`. If the vendor
built their static lib without `-fPIC` (sadly common with proprietary drivers), you
can't link it into your `.so` on x86-64. You either get the vendor to ship a PIC
build, or you ship `libcan_driver` as a static library too and let the final
executable do the linking.

**3. ROS 2 nodes.** Each `rclcpp_components`-registered node is compiled into a
shared library so the component container can load it at runtime. CMake's
`ament_auto_add_library(... SHARED ...)` takes care of `-fPIC` for you — but the
moment you add a third-party static dep, the relocation error from
[above](#the-error-youll-actually-hit) shows up.

**4. Hardening a robot's onboard binary.** For a robot that exposes a network
interface (e.g. a ROS bridge or a teleop server), building the main executable with
`-fPIE -pie` and the kernel's ASLR enabled makes ROP-style exploits significantly
harder. The cost is one extra register pressure on x86-64 — negligible for a control
loop running at 1 kHz.

## Seeing the difference in assembly

A minimal program with a function-local static (so the compiler has to emit a data
reference) — save this as `example.cpp`:

```cpp
#include <iostream>

int counter() {
    static int count = 0;
    return ++count;
}

int main() {
    std::cout << "Counter: " << counter() << '\n';
    return 0;
}
```

Build both flavors and diff the disassembly:

```bash
g++ -O1 -fno-pie -no-pie -c example.cpp -o example_nopic.o
g++ -O1 -fPIC               -c example.cpp -o example_fpic.o

objdump -d example_nopic.o > nopic.asm
objdump -d example_fpic.o  > fpic.asm

diff -u nopic.asm fpic.asm   # or: meld nopic.asm fpic.asm
```

What to look for inside `counter()`:

- **Without `-fPIC`** — the access to `count` shows up as an absolute address that
  the linker will fill in, e.g.
  `mov 0x0(%rip),%eax  # R_X86_64_PC32 against count` followed by a relocation entry.
- **With `-fPIC`** — the same access goes through the GOT (Global Offset Table):
  `mov count@GOTPCREL(%rip),%rax` then `mov (%rax),%eax`. One extra indirection, no
  load-time fix-up of the code itself.

That extra indirection is the price of position independence. On modern x86-64 it's
typically 1–2% on data-heavy code and often unmeasurable on control-flow-heavy code
like a real-time loop.
