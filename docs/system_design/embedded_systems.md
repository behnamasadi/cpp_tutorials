# Embedded Systems Design

Embedded C++ runs on systems where memory is bytes-to-megabytes, not gigabytes; where there's often no OS, no heap, no exceptions; and where the hardware datasheet matters more than any abstraction. The discipline is "modern C++ within tight constraints" — a subset of the language, but a powerful one.

- [1. The Embedded Spectrum](#1-the-embedded-spectrum)
- [2. Freestanding C++](#2-freestanding-c)
- [3. No Heap, No Exceptions, No RTTI](#3-no-heap-no-exceptions-no-rtti)
- [4. ROMable Code and `constinit`](#4-romable-code-and-constinit)
- [5. Memory-Mapped I/O](#5-memory-mapped-io)
- [6. Interrupt Service Routines](#6-interrupt-service-routines)
- [7. Power Management](#7-power-management)
- [8. Embedded Standard Library Replacements](#8-embedded-standard-library-replacements)
- [9. Build, Toolchains, and Sizing](#9-build-toolchains-and-sizing)
- [10. Patterns](#10-patterns)

---

# 1. The Embedded Spectrum

| Tier | Memory | OS | Examples | Modern C++ usable? |
|---|---|---|---|---|
| **Bare-metal MCU** | KBs of RAM | None | ARM Cortex-M0/M3, AVR | C++17 subset |
| **MCU with RTOS** | 100s KB | FreeRTOS, Zephyr | Cortex-M4/M7, ESP32 | C++17 broader |
| **MPU with embedded Linux** | 16 MB+ | Linux, QNX | Raspberry Pi, BeagleBone | Full C++20+ |
| **Edge AI / SoC** | GBs | Linux | NVIDIA Jetson, Apple silicon | Full + GPU |

This page focuses on the first two tiers — where you have to reason about every byte and every clock cycle. Linux-class embedded ("Yocto, BSP, but it's Linux") is mostly the same as server C++.

# 2. Freestanding C++

A *freestanding* C++ implementation (vs *hosted*) provides a smaller subset:
- No `<iostream>`, `<thread>`, `<filesystem>`, etc.
- No `main()` requirement.
- Limited library (`<cstddef>`, `<type_traits>`, `<array>` mostly).
- The build doesn't link a C runtime.

You bring up the system yourself: reset vector, stack pointer, `.bss` zero, `.data` copy from flash to RAM, then call `main`.

C++23's [P1642 / P0829](https://wg21.link/p1642) expanded what's portable in freestanding (more `<algorithm>`, `<ranges>`, etc.), but specific support varies by toolchain.

# 3. No Heap, No Exceptions, No RTTI

The embedded triumvirate:

**No heap (`-fno-rtti -fno-exceptions -nostdlib`):** static and stack only.
- Use `std::array<T, N>` instead of `std::vector`.
- Use [Embedded Template Library (etl)](https://www.etlcpp.com/) for fixed-capacity vectors, maps, queues, strings.
- Object pools for variable-lifetime objects.
- Stack arenas for transient buffers.

**No exceptions (`-fno-exceptions`):** every operation that "might throw" gets a non-throwing alternative.
- Use `std::expected<T, E>` or hand-rolled error returns.
- `new(std::nothrow)` if you must allocate.
- Exit on impossible conditions; assertions, not throws.

**No RTTI (`-fno-rtti`):** `dynamic_cast` and `typeid` go away.
- Use `static_cast` with hierarchy invariants you maintain.
- Tag types with an enum if you need to discriminate.

These flags **also save flash**. Disabling exceptions removes unwind tables; RTTI removes type info. Often 5–20% smaller.

# 4. ROMable Code and `constinit`

In bare-metal:
- Code lives in flash (read-only).
- `const` global data lives in flash (`.rodata`).
- Mutable globals live in RAM (`.data` initialized from flash, or `.bss` zero-init).
- Stack and heap are carved out of remaining RAM.

C++ static initializers run *before* `main`. If they throw or trap, you've crashed before getting to your code. To get cleaner control:

```cpp
constinit int g_counter = 0;          // C++20 — guaranteed compile-time init
constexpr int kBaud = 115200;
```

`constinit` enforces that the compiler use compile-time initialization (no runtime constructor), which keeps the value in `.data` or `.rodata` and guarantees no startup work.

Avoid `static` non-trivially-constructed objects in functions — the lazy-init guard adds atomics and a flag, both of which cost flash and may not work pre-`main`.

# 5. Memory-Mapped I/O

Hardware peripherals live at fixed addresses. The classic pattern:

```cpp
#include <cstdint>

struct GpioRegs {
  volatile uint32_t MODER;     // mode register
  volatile uint32_t OTYPER;    // output type
  volatile uint32_t OSPEEDR;
  volatile uint32_t PUPDR;
  volatile uint32_t IDR;       // input data
  volatile uint32_t ODR;       // output data
  volatile uint32_t BSRR;      // bit set/reset
};

GpioRegs* const GPIOA = reinterpret_cast<GpioRegs*>(0x40020000);

void setPA5High() {
  GPIOA->ODR |= (1 << 5);      // set pin PA5 high
}
```

Why `volatile`: the compiler must not optimize away or reorder reads/writes the way it would with normal memory. Hardware sees every access; sometimes reading a register has side effects (clearing a flag), and missing those reads breaks the device.

`volatile` is for **MMIO and signal handlers**, *not* for thread synchronization (use `std::atomic`). See [Volatile Keyword](../volatile.md).

Vendor headers (CMSIS, STM32 HAL) provide these structs typed correctly. Use them.

# 6. Interrupt Service Routines

ISRs interrupt the running code, do their work, return. Constraints:

- **Short.** Anything beyond microseconds belongs in the main loop, signaled from the ISR.
- **No blocking.** Locks held by main code can't be acquired in an ISR — except with specifically-designed RTOS primitives.
- **Re-entrant safety.** A higher-priority ISR can preempt this ISR. Nested or shared state must be protected.
- **No floating point** unless the hardware has FPU context save (most Cortex-M4F+ do, M0 doesn't).

```cpp
#include <cstdint>

struct ExtiRegs { volatile uint32_t PR; };
ExtiRegs* const EXTI = reinterpret_cast<ExtiRegs*>(0x40013C00);

volatile bool g_button_event = false;   // shared with main; volatile required

void handleButton() { /* ... */ }

extern "C" void EXTI0_IRQHandler() {
  EXTI->PR = (1u << 0);                 // clear pending bit (write-1-to-clear)
  g_button_event = true;
}

void mainLoopStep() {
  if (g_button_event) {
    g_button_event = false;
    handleButton();
  }
}
```

For richer ISR/main communication, use a lock-free SPSC ring buffer (see [Lock-Free Data Structures](lock_free_data_structures.md)).

# 7. Power Management

Battery-powered embedded means the deepest sleep mode the application can tolerate, woken by interrupts:

```cpp
void handlePending() { /* drain queues, update state */ }

void runForever() {
  while (true) {
    handlePending();
    __WFI();       // wait for interrupt — sleep until something happens
  }
}
```

Tactics:
- **Tickless RTOS** — no periodic timer; wake only when needed.
- **Peripheral clock gating** — turn off clocks to unused peripherals.
- **Voltage scaling** — drop CPU frequency when idle.
- **DMA over CPU polling** — DMA can run while CPU sleeps.

Power and timing are coupled: a faster CPU finishes earlier and sleeps longer ("race to sleep"). Profile both.

# 8. Embedded Standard Library Replacements

| Need | Replacement |
|---|---|
| `std::vector` | `etl::vector<T, N>`, `std::array<T, N>` |
| `std::string` | `etl::string<N>`, fixed `char[]` + `std::span` |
| `std::map` | `etl::map<K, V, N>` (sorted vector) |
| `std::function` | Function pointers; `etl::delegate`; `std::function_ref` (C++26) |
| `std::shared_ptr` | Avoid; use `unique_ptr` or static lifetimes |
| `std::optional` | Use as-is (header-only, no allocations) |
| `std::variant` | Use as-is |
| `<iostream>` | `printf` or [`<format>`](https://github.com/fmtlib/fmt) (fmt has compiled mode without iostream) |
| `<thread>` | RTOS task primitives |
| `<chrono>` | Use as-is (no allocations); pair with hardware timer |

# 9. Build, Toolchains, and Sizing

Toolchains: `arm-none-eabi-gcc`, `clang --target=thumbv7m-none-eabi`, vendor IDEs (Keil, IAR — proprietary).

Build flags worth knowing:
- `-Os` — optimize for size. Often what you want.
- `-flto` — link-time optimization; significant size and speed wins.
- `-ffunction-sections -fdata-sections -Wl,--gc-sections` — remove unused functions/data at link time.
- `-fno-rtti -fno-exceptions` — see §3.
- `-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard` — match the target.

Sizing your binary:
```bash
arm-none-eabi-size firmware.elf            # text/data/bss
arm-none-eabi-nm --size-sort firmware.elf  # what's biggest
```

Tools like [Bloaty McBloatface](https://github.com/google/bloaty) give a richer breakdown.

# 10. Patterns

**Static polymorphism over virtual.** CRTP gives compile-time dispatch with no vtable cost. Use it when you'd otherwise reach for inheritance:
```cpp
#include <cstdint>

template<class Derived>
struct Driver {
  void send(uint8_t b) { static_cast<Derived*>(this)->doSend(b); }
};

struct UartDriver : Driver<UartDriver> {
  void doSend(uint8_t b) { /* write byte to UART data register */ }
};

int main() {
  UartDriver uart;
  uart.send(0x42);    // dispatched at compile time, no vtable
}
```

**Constexpr everything.** Move work to compile time. Tables, lookup arrays, configuration — all `constexpr`.

**Type-safe units.** `std::chrono::duration` for time; or roll your own for length, voltage, etc. Catches "passed milliseconds where seconds expected" at compile time.

**Singleton hardware.** A peripheral has exactly one instance; modeling it as a class with a private static gives a clean API and prevents two paths from initializing the same hardware twice.

**State machine for protocols.** UART parsers, SPI sequences, USB enumeration — all state machines. See [Event-Driven and State Machines](event_driven_state_machines.md).

**Layered HAL.** Bottom: register access. Middle: peripheral driver (`Uart`, `Spi`). Top: application protocol (`Modbus`, `MQTT`). Each layer testable independently with substitute implementations.

# References

- [Volatile Keyword](../volatile.md)
- [Real-Time Systems Design](realtime_systems.md)
- [Lock-Free Data Structures](lock_free_data_structures.md)
- [Memory Management Strategies](memory_management.md)
- *Making Embedded Systems*, Elecia White.
- *Real-Time C++*, Christopher Kormanyos — modern C++ in MCU territory.
- [Embedded Template Library](https://www.etlcpp.com/)
- [CppCon talks: Modern C++ in Embedded Systems](https://www.youtube.com/results?search_query=cppcon+embedded)
