# Arrays and Spans

- [The one-minute version](#the-one-minute-version)
- [C-style arrays](#c-style-arrays)
  - [Size and the decay trap](#size-and-the-decay-trap)
  - [Multidimensional arrays](#multidimensional-arrays)
- [`std::array`](#stdarray)
- [`std::span` (C++20)](#stdspan-c20)
- [Robotics scenarios](#robotics-scenarios)
- [Tricky questions and common mistakes](#tricky-questions-and-common-mistakes)
- [Reference](#reference)

## The one-minute version

| Type               | Owns memory? | Knows its size?     | Size fixed at... | When to use                                              |
|--------------------|--------------|---------------------|------------------|----------------------------------------------------------|
| `int arr[N]`       | Yes (stack)  | Sometimes (decays!) | Compile time     | Legacy code, embedded, talking to a C API                |
| `std::array<T, N>` | Yes (stack)  | Yes (constexpr)     | Compile time     | Fixed-size buffer with a type-safe size (CAN frame, IMU) |
| `std::vector<T>`   | Yes (heap)   | Yes                 | Runtime          | Variable size, owning                                    |
| `std::span<T>`     | **No**       | Yes                 | Either           | A non-owning view over any contiguous storage            |

**Rule of thumb:** functions that *read or write* a contiguous range without taking
ownership should accept `std::span<T>` or `std::span<const T>`. Storage that lives in
the function or object should be `std::array` or `std::vector`.

## C-style arrays

```cpp
int joint_targets[6] = { 0, 0, 0, 0, 0, 0 };  // 6 joints, all zero
joint_targets[2] = 45;                         // OK
// joint_targets[6] = 0;                       // UB: out of bounds, no diagnostic
```

A C array is a contiguous block of `N` objects. There's no built-in size method, no
bounds check, and no way to know — once it's been passed to a function — how big it
originally was.

### Size and the decay trap

```cpp
int arr[] = { 1, 2, 3, 4, 5 };
std::cout << sizeof(arr) << '\n';                 // 20 (5 * sizeof(int))
constexpr auto n = sizeof(arr) / sizeof(arr[0]);  // 5

for (int* p = arr; p != arr + n; ++p) {
    std::cout << *p << '\n';
}
```

The moment you pass `arr` to a function, it **decays** to `int*` and forgets its size:

```cpp
void print(int arr[]) {                  // identical to: void print(int* arr)
    std::cout << sizeof(arr) << '\n';    // 8 on x86-64 — that's sizeof(int*), not the array!
}
```

This is the single most common C-array bug. Fixes, in order of how modern they are:

```cpp
void print(int* arr, std::size_t n);                       // pass size explicitly
template <std::size_t N> void print(int (&arr)[N]);        // reference-to-array keeps N
void print(std::span<const int> arr);                      // C++20: just use a span
```

See [decay.md](decay.md) for the full picture on array-to-pointer decay.

### Multidimensional arrays

C++ doesn't have true multidimensional arrays — it has arrays of arrays.

```cpp
int grid[3][5] = {
    {  1,  2,  3,  4,  5 },
    {  6,  7,  8,  9, 10 },
    { 11, 12, 13, 14, 15 },
};
```

When passing one to a function, every dimension except the leftmost is part of the
type:

```cpp
void process(int (&grid)[3][5]);    // takes exactly 3x5
void process(int grid[][5], int rows);   // any number of rows, columns must be 5
```

For runtime-sized 2-D data, prefer `std::vector<std::vector<T>>` (simple but two
allocations and not contiguous), a flat `std::vector<T>` with manual row indexing
(one allocation, cache-friendly), or `std::mdspan` (C++23). Avoid the
`new int*[rows]` pattern from older tutorials — it's a manual-memory minefield.

## `std::array`

`std::array<T, N>` is a thin wrapper over `T[N]` that behaves like a real value
type — it knows its size, can be copied and returned, supports iterators and
algorithms, and never decays to a pointer.

```cpp
#include <array>
#include <algorithm>
#include <numeric>

std::array<double, 7> joint_pos = {};        // value-initialized to zero
joint_pos.fill(0.0);                         // explicit fill
auto avg = std::accumulate(joint_pos.begin(), joint_pos.end(), 0.0) / joint_pos.size();

constexpr auto size = joint_pos.size();      // constexpr! known at compile time
auto& first  = joint_pos.front();
auto& last   = joint_pos.back();
auto& fourth = joint_pos.at(3);              // bounds-checked; throws on bad index
auto& fifth  = joint_pos[4];                 // not bounds-checked
```

You can return a `std::array` from a function (impossible with a C array) and pass it
by value cheaply for small sizes. Initialization deduces nicely with `std::to_array`
(C++20):

```cpp
auto frame = std::to_array<uint8_t>({0xDE, 0xAD, 0xBE, 0xEF});
// → std::array<uint8_t, 4>
```

## `std::span` (C++20)

`std::span<T>` is a **non-owning view** over a contiguous range. It's a pointer plus
a length — usually a 16-byte struct on 64-bit machines. It accepts a C array, a
`std::array`, a `std::vector`, a raw `(ptr, size)` pair, anything contiguous.

```cpp
#include <span>

void normalize(std::span<float> v) {
    float n = 0.0f;
    for (float x : v) n += x * x;
    n = std::sqrt(n);
    for (float& x : v) x /= n;
}

int main() {
    float       a[3] = {3, 4, 0};
    std::array  b    = {3.0f, 4.0f, 0.0f};
    std::vector c    = {3.0f, 4.0f, 0.0f};

    normalize(a);            // C array  → span
    normalize(b);            // std::array → span
    normalize(c);            // std::vector → span
    normalize({c.data() + 1, 2});  // a sub-range, no copy
}
```

`std::span<const T>` is read-only — use it for input parameters; `std::span<T>` for
output/in-out. Pass spans by value, not by reference.

Subviews are cheap:

```cpp
std::span<const uint8_t> packet = ...;
auto header  = packet.first(8);          // first 8 bytes
auto payload = packet.subspan(8);        // rest
auto crc     = packet.last(4);           // last 4 bytes
```

Spans also have an optional compile-time extent: `std::span<int, 6>` is exactly six
elements and the size is `constexpr`. Most of the time you want the dynamic-extent
form (`std::span<T>`), which is what the constructors above produce.

## Robotics scenarios

**CAN classic frame payload.** A classic CAN frame has exactly 8 data bytes — use
`std::array<uint8_t, 8>` because the size is a hard part of the protocol:

```cpp
struct CanFrame {
    uint32_t id;
    uint8_t  dlc;                       // 0..8
    std::array<uint8_t, 8> data;
};

void send(const CanFrame& f) {
    write_to_socket(f.id, std::span(f.data).first(f.dlc));   // only DLC bytes
}
```

**Sliding window of IMU samples.** A fixed-capacity ring buffer that lives on the
stack:

```cpp
constexpr std::size_t WINDOW = 100;
std::array<ImuSample, WINDOW> imu_window{};
```

**Lidar scan filtering.** The filter shouldn't care whether the scan came from a ROS
message, a recorded `.bag`, or a simulator — accept any contiguous range:

```cpp
void downsample(std::span<const float> ranges,
                std::span<float>       out);
```

**Image rows.** Iterating over rows of a frame buffer without copying:

```cpp
for (int y = 0; y < height; ++y) {
    std::span<uint8_t> row{frame.data() + y * stride, width};
    sobel(row);
}
```

## Tricky questions and common mistakes

**1. "Why is `sizeof(arr)` wrong inside my function?"**
Because the array decayed to a pointer at the call site. `sizeof(arr)` inside the
function gives the size of a pointer (usually 8 bytes), not the array. Fix: pass a
`std::span`, or use a `T (&)[N]` reference-to-array parameter.

**2. Returning a span to a local.**

```cpp
std::span<int> bad() {
    int local[4] = {1, 2, 3, 4};
    return local;                  // DANGLING — local dies at return
}
```

`std::span` owns nothing. The same trap exists with temporaries:

```cpp
std::span<const int> view = std::vector{1, 2, 3};   // DANGLING
```

The `vector` is destroyed at the end of the full expression. `std::span` does
**not** extend its lifetime (unlike a `const T&` to a single object).

**3. `sizeof` on a `char*` vs a string literal.**

```cpp
const char  s1[] = "robot";       // array of 6 chars (incl. '\0')
const char* s2   = "robot";       // pointer

sizeof(s1);                       // 6
sizeof(s2);                       // 8 (size of pointer on x86-64)
strlen(s1);                       // 5 in both cases
```

**4. The classic `int` index bug.**

```cpp
for (int i = 0; i < sizeof(arr); ++i) { ... }   // BUG
```

`sizeof(arr)` is bytes, not elements. Use `std::size(arr)` (C++17) or a range-`for`.

**5. `std::array<T, 0>`.** Legal and useful in generic code — it has zero elements,
`empty()` returns true, but `front()`/`back()` are undefined behavior. Don't call
them without checking.

**6. `std::span<int>` vs `std::span<const int>` for a `const` argument.**

```cpp
void read(const std::span<int> s);    // `s` itself is const, but the elements are mutable
void read(std::span<const int> s);    // the elements are read-only — what you usually want
```

For an input parameter, **`std::span<const T>`** is almost always the right choice.

**7. Spans don't own — and don't deep-copy.** Copying a span copies the
pointer-and-length pair. Two spans aliasing the same buffer are fine for reads, a
data race for writes from different threads.

**8. Initializer-list pitfall.**

```cpp
void foo(std::span<const int> s);
foo({1, 2, 3});                  // OK — braced init constructs a temporary array... that dies at ;
```

Works as a function argument because the temporary lives until the end of the call
expression. Storing the span across statements is a bug; see point 2.

**9. Iterating with the wrong index type.**

```cpp
std::array<int, 5> a{};
for (int i = 0; i < a.size(); ++i) {}   // signed/unsigned warning: a.size() is size_t
```

Use `std::size_t`, or — better — `for (auto& x : a)`.

**10. Variable-length arrays (VLAs) are not standard C++.**

```cpp
void f(int n) {
    int buf[n];           // gcc extension only — NOT portable C++
}
```

Use `std::vector<int>(n)` if you really need a runtime size. Static analyzers will
flag VLAs because they let an attacker-controlled `n` blow the stack.

**11. Subspan past the end.**

```cpp
std::span<int> s = ...;     // size 10
s.subspan(20);              // UB — debug builds may assert, release won't
```

Always validate the offset/length when the source is external (a packet length, a
file header, a user input).

**12. `std::array` is *not* a `std::vector`.** Resizing, push_back, and clearing
don't exist — the size is baked into the type. If you need to grow, you need a
`vector` (or a small-buffer-optimized variant like `boost::container::small_vector`).

## Reference

- [`std::array` — cppreference](https://en.cppreference.com/w/cpp/container/array)
- [`std::span` — cppreference](https://en.cppreference.com/w/cpp/container/span)
- [`std::to_array` — cppreference](https://en.cppreference.com/w/cpp/container/array/to_array)
- [`std::mdspan` (C++23) — cppreference](https://en.cppreference.com/w/cpp/container/mdspan)
- Source code: [src/arrays.cpp](../src/arrays.cpp)
