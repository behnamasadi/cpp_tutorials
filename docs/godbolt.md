# Compiler Explorer (godbolt.org)

## What is it?

[Compiler Explorer](https://godbolt.org), commonly called **godbolt** after its creator Matt Godbolt, is an online tool that compiles C++ (and many other languages) and shows you the **generated assembly** side-by-side with your source. No install, no `objdump`, no `-S` flag juggling — paste code on the left, read assembly on the right, with the lines color-matched.

It runs every mainstream compiler — GCC, Clang, MSVC, ICC, NVCC, ARM, and dozens of cross-compilers — at every version going back roughly a decade.

## Why C++ developers actually use it

- **See what the optimizer did.** Did that `constexpr` actually fold? Did the loop vectorize? Did the function get inlined into a single `mov`? Assembly is the ground truth.
- **Compare compilers.** Open two panes: GCC on the left, Clang on the right, same source. You'll find that one inlines what the other doesn't, or vectorizes a loop the other unrolls.
- **Compare standard library implementations.** libstdc++ vs libc++ vs MSVC's STL — the assembly from `std::sort`, `std::variant`, or `std::shared_ptr` differs noticeably.
- **Share a self-contained reproducer.** Click `Share`, paste the URL into a bug report or Slack. The recipient sees exactly what you saw, with the same compiler, flags, and code.
- **Decode template error messages.** Hover, expand, copy. Easier than scrolling through 80 lines of `<...>::type::value_type::iterator_category` in your terminal.

## The UI in one minute

| Pane | What it does |
|---|---|
| **Source** (left)   | Your C++ code. |
| **Compiler output** (right) | The generated assembly, line-matched to source by color. |
| **Compiler dropdown** | Pick GCC 13.2, Clang 17, MSVC 19.38, etc. |
| **Compiler options box** | Free-text flags: `-O2 -std=c++20 -march=native ...` |
| **Add new...** menu | Add an Execution pane (run the binary), a Diff view, an LLVM IR pane, an opt-pipeline view, or a second Compiler pane. |

The colored stripes on the left of each pane match source lines to the assembly they produced. Click an assembly line to see which source line generated it.

## Useful workflows

### "Did the compiler optimize this away?"

Compile with `-O2` and look at the function body in the right pane. If you see your function reduced to a single `mov eax, 42` followed by `ret`, the constant folded. If the function symbol isn't in the output at all, it was inlined into its callers and the standalone version was eliminated.

A common trick to keep the function visible while still getting optimization: don't mark it `static`, and add a *consumer* like `int foo() { return ...; }` so the compiler must emit a definition.

### Comparing two implementations

Open a second source pane (`Add new... -> Source editor`) and a second compiler pane. Now you have two side-by-side compilations of two slightly different versions — `std::vector` vs raw array, `if/else` vs `std::min`, switch vs lookup table. Same compiler, same flags, just the source differs.

For a cleaner diff, use `Add new... -> Diff view` to see the assembly differences highlighted directly.

### Sharing a permalink

Click **Share** in the top bar. You get a short URL (`https://godbolt.org/z/...`) that captures source, compiler, flags, all open panes, *and* layout. This is the standard way to attach a reproducer to bug reports on the libstdc++, libc++, GCC, and Clang trackers.

Alternatives in the Share menu: full URL (no shortener dependency), embeddable iframe, "open in a new window."

### Including third-party libraries

Click the **Libraries** button (under the compiler options) and pick from the list: Eigen, fmt, Boost, range-v3, Catch2, Google Benchmark, abseil, and so on. Compiler Explorer fetches and includes them — no `-I` paths needed.

```cpp
#include <fmt/format.h>
auto greet() { return fmt::format("hello {}", 42); }
```

Hit compile and the resulting assembly shows fmt's actual codegen at your chosen optimization level.

## Useful flags

| Flag | What it does |
|---|---|
| `-O2`               | Standard release-grade optimization. The default to compare against. |
| `-O3`               | More aggressive: vectorization, loop unrolling, function cloning. |
| `-Os`               | Optimize for size. Sometimes faster than `-O2` on instruction-cache-bound code. |
| `-march=native`     | Use every instruction set the host CPU supports (AVX2, AVX-512, BMI2, ...). On godbolt the "host" is their server, not yours — pick `-march=skylake`, `-march=znver3`, etc. for a specific target. |
| `-std=c++20`        | Pick the language standard. `c++17`, `c++20`, `c++23` are the common ones. |
| `-Wall -Wextra`     | Enable warnings. Warnings show up in the **Output** dropdown at the bottom of the compiler pane. |
| `-stdlib=libc++`    | (Clang only) Use libc++ instead of libstdc++. Useful for comparing STL implementations. |

## Reducing assembly noise

Even a tiny function emits a wall of `.cfi_*` directives, exception-handling tables, and unwind metadata. They're irrelevant if you only want to see what the function *computes*. Add these flags to strip them:

```
-fno-asynchronous-unwind-tables -fno-exceptions
```

Optionally also `-fno-rtti` and `-fno-stack-protector`. Toggle the **Output > Filter** menu (the funnel icon above the assembly pane) and turn on at minimum:

- **Unused labels** off
- **Library functions** off
- **Directives** off
- **Comments** off
- **Demangle identifiers** on
- **Intel syntax** on (subjective — easier to read than AT&T for most)

The result is dramatically more readable assembly that fits on one screen.

## Cousins worth knowing

| Tool | What it shows |
|---|---|
| [cppinsights.io](https://cppinsights.io)   | Rewrites your C++ to show **what the compiler sees** after template instantiation, lambda lowering, structured-binding desugaring, range-for expansion, etc. The complement to godbolt: godbolt shows the back end, cppinsights shows the front end. |
| [quick-bench.com](https://quick-bench.com) | Online Google Benchmark runner. Paste two implementations, get a chart of nanoseconds-per-op. Great for settling "is `std::find` faster than a manual loop?" arguments without building locally. |
| [build-bench.com](https://build-bench.com) | Same idea for **compile-time** comparisons — useful when picking between heavy template libraries. |

## References

- [Compiler Explorer](https://godbolt.org)
- [Matt Godbolt's CppCon talk: "What Has My Compiler Done For Me Lately?"](https://www.youtube.com/watch?v=bSkpMdDe4g4)
- [cppinsights.io](https://cppinsights.io)
- [quick-bench.com](https://quick-bench.com)
