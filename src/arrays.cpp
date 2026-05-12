// Arrays and Spans — companion code for docs/array_span.md
//
// Demonstrates C arrays, std::array, and std::span (C++20) with robotics-flavored
// examples: joint targets, CAN frames, IMU windows, lidar scans, image rows.

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numeric>
#include <span>
#include <type_traits>
#include <vector>

// ---------------------------------------------------------------------------
// 1. C-style arrays: size, decay, and bounds
// ---------------------------------------------------------------------------

// Decay trap: the parameter is really `int*` — sizeof gives the pointer size.
static void print_decayed(int arr[]) {
  std::cout << "  inside print_decayed, sizeof(arr) = " << sizeof(arr)
            << " (this is sizeof(int*), NOT the array!)\n";
}

// Fix #1 — explicit size.
static void print_with_size(const int *arr, std::size_t n) {
  std::cout << "  print_with_size:";
  for (std::size_t i = 0; i < n; ++i)
    std::cout << ' ' << arr[i];
  std::cout << '\n';
}

// Fix #2 — reference to array preserves N at compile time.
template <std::size_t N>
static void print_by_ref(int (&arr)[N]) {
  std::cout << "  print_by_ref: N=" << N << " elements:";
  for (auto v : arr)
    std::cout << ' ' << v;
  std::cout << '\n';
}

// (Fix #3 — std::span — shown in a later section.)

static void c_array_demo() {
  std::cout << "--- C-style array: joint targets ---\n";
  int joint_targets[6] = {0, 0, 0, 0, 0, 0};  // 6 joints, all zero
  joint_targets[2] = 45;
  // joint_targets[6] = 0;                    // UB: out of bounds, no diagnostic

  int arr[] = {1, 2, 3, 4, 5};
  std::cout << "  sizeof(arr) = " << sizeof(arr) << " bytes (5 * sizeof(int))\n";
  constexpr auto n = sizeof(arr) / sizeof(arr[0]);
  std::cout << "  element count = " << n << '\n';

  std::cout << "  pointer walk:";
  for (int *p = arr; p != arr + n; ++p)
    std::cout << ' ' << *p;
  std::cout << '\n';

  print_decayed(arr);   // shows the decay bug
  print_with_size(arr, n);
  print_by_ref(arr);    // template preserves the size

  // String literal vs char*: sizeof differs, strlen does not.
  const char s1[] = "robot";       // array of 6 chars (incl. '\0')
  const char *s2 = "robot";        // pointer
  std::cout << "  sizeof(\"robot\" array) = " << sizeof(s1)
            << ", sizeof(const char*) = " << sizeof(s2)
            << ", strlen = " << std::strlen(s1) << '\n';
}

// ---------------------------------------------------------------------------
// 2. Multidimensional arrays
// ---------------------------------------------------------------------------

// Takes exactly 3x5 — every dimension except the leftmost is part of the type.
static void process_3x5(int (&grid)[3][5]) {
  std::cout << "  3x5 grid sum =";
  int sum = 0;
  for (auto &row : grid)
    for (auto v : row)
      sum += v;
  std::cout << ' ' << sum << '\n';
}

// Any number of rows, columns must be 5.
static void process_rows(int grid[][5], int rows) {
  std::cout << "  process_rows row[0][0] = " << grid[0][0]
            << ", row[" << rows - 1 << "][4] = " << grid[rows - 1][4] << '\n';
}

static void multidim_demo() {
  std::cout << "--- Multidimensional arrays ---\n";
  int grid[3][5] = {
      {1, 2, 3, 4, 5},
      {6, 7, 8, 9, 10},
      {11, 12, 13, 14, 15},
  };
  process_3x5(grid);
  process_rows(grid, 3);
}

// ---------------------------------------------------------------------------
// 3. std::array
// ---------------------------------------------------------------------------

static void std_array_demo() {
  std::cout << "--- std::array: 7-DOF joint positions ---\n";

  std::array<double, 7> joint_pos = {};  // value-initialized to zero
  joint_pos.fill(0.0);                   // explicit fill
  joint_pos = {0.1, -0.2, 0.3, 0.0, 1.2, -0.5, 0.7};

  auto avg = std::accumulate(joint_pos.begin(), joint_pos.end(), 0.0) /
             joint_pos.size();

  constexpr auto size = std::array<double, 7>{}.size();  // constexpr!
  auto &first = joint_pos.front();
  auto &last = joint_pos.back();
  auto &fourth = joint_pos.at(3);   // bounds-checked
  auto &fifth = joint_pos[4];       // not bounds-checked

  std::cout << std::fixed << std::setprecision(3);
  std::cout << "  size (constexpr) = " << size << ", avg = " << avg
            << ", front = " << first << ", back = " << last
            << ", at(3) = " << fourth << ", [4] = " << fifth << '\n';
  std::cout.unsetf(std::ios::fixed);

  // std::to_array (C++20) — deduces N from the braced list.
  auto frame = std::to_array<std::uint8_t>({0xDE, 0xAD, 0xBE, 0xEF});
  static_assert(frame.size() == 4);
  std::cout << "  to_array<uint8_t>({0xDE,0xAD,0xBE,0xEF}) -> "
            << "std::array<uint8_t, " << frame.size() << ">: " << std::hex
            << std::setfill('0');
  for (auto b : frame)
    std::cout << " 0x" << std::setw(2) << static_cast<int>(b);
  std::cout << std::dec << std::setfill(' ') << '\n';

  // Container operations work on std::array.
  std::array<int, 3> a2 = {3, 1, 2};
  std::sort(a2.begin(), a2.end());
  std::cout << "  sorted+reversed: ";
  std::reverse_copy(a2.begin(), a2.end(),
                    std::ostream_iterator<int>(std::cout, " "));
  std::cout << '\n';
}

// ---------------------------------------------------------------------------
// 4. std::span (C++20)
// ---------------------------------------------------------------------------

// Mutating span: normalize a 3-vector in place.
static void normalize(std::span<float> v) {
  float n = 0.0f;
  for (float x : v)
    n += x * x;
  n = std::sqrt(n);
  if (n == 0.0f)
    return;
  for (float &x : v)
    x /= n;
}

// Read-only span: prints any contiguous range of floats.
static void print_floats(const char *label, std::span<const float> v) {
  std::cout << "  " << label << " [" << v.size() << "]:";
  std::cout << std::fixed << std::setprecision(3);
  for (float x : v)
    std::cout << ' ' << x;
  std::cout.unsetf(std::ios::fixed);
  std::cout << '\n';
}

static void span_demo() {
  std::cout << "--- std::span: one function, many sources ---\n";

  float a[3] = {3, 4, 0};
  std::array<float, 3> b = {3.0f, 4.0f, 0.0f};
  std::vector<float> c = {3.0f, 4.0f, 0.0f};

  normalize(a);                      // C array       -> span
  normalize(b);                      // std::array    -> span
  normalize(c);                      // std::vector   -> span

  print_floats("from C array  ", a);
  print_floats("from std::array", b);
  print_floats("from std::vector", c);

  // Sub-range: normalize only the tail of a buffer, no copy.
  std::vector<float> tail = {99.0f, 3.0f, 4.0f};
  normalize({tail.data() + 1, 2});
  print_floats("subspan(1,2)  ", tail);

  // first / last / subspan on a packet of bytes.
  std::array<std::uint8_t, 16> raw = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0A, 0x0B, 0x0C, 0xDE, 0xAD,
                                      0xBE, 0xEF};
  std::span<const std::uint8_t> packet{raw};
  auto header = packet.first(8);     // first 8 bytes
  auto payload = packet.subspan(8);  // rest
  auto crc = packet.last(4);         // last 4 bytes
  std::cout << "  packet sizes: header=" << header.size()
            << " payload=" << payload.size() << " crc=" << crc.size() << '\n';

  // Compile-time extent: size is constexpr.
  std::span<float, 3> fixed_view{b};
  static_assert(fixed_view.size() == 3);
  std::cout << "  fixed-extent span<float, 3> size (constexpr) = "
            << fixed_view.size() << '\n';
}

// ---------------------------------------------------------------------------
// 5. Robotics scenarios — CAN frame, IMU window, lidar scan, image rows
// ---------------------------------------------------------------------------

struct CanFrame {
  std::uint32_t id;
  std::uint8_t dlc;                       // 0..8
  std::array<std::uint8_t, 8> data;
};

// Pretends to send `dlc` bytes; we just print them.
static void send_can(const CanFrame &f) {
  std::cout << "  CAN id=0x" << std::hex << f.id << std::dec
            << " dlc=" << static_cast<int>(f.dlc) << " bytes:" << std::hex
            << std::setfill('0');
  for (auto b : std::span(f.data).first(f.dlc))
    std::cout << " 0x" << std::setw(2) << static_cast<int>(b);
  std::cout << std::dec << std::setfill(' ') << '\n';
}

struct ImuSample {
  float ax, ay, az;
};

// Filter accepts any contiguous range — works on ROS arrays, .bag replays,
// simulator output. Downsamples by averaging pairs.
static void downsample(std::span<const float> ranges, std::span<float> out) {
  const std::size_t pairs = ranges.size() / 2;
  const std::size_t n = std::min(pairs, out.size());
  for (std::size_t i = 0; i < n; ++i)
    out[i] = 0.5f * (ranges[2 * i] + ranges[2 * i + 1]);
}

// Iterating over rows of a frame buffer without copying.
static std::uint64_t row_sum(std::span<const std::uint8_t> row) {
  std::uint64_t s = 0;
  for (auto p : row)
    s += p;
  return s;
}

static void robotics_demo() {
  std::cout << "--- Robotics scenarios ---\n";

  // CAN: only the DLC bytes are actually on the wire.
  CanFrame f{0x123, 5, {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0, 0, 0}};
  send_can(f);

  // Sliding window of IMU samples — fixed capacity, on the stack.
  constexpr std::size_t WINDOW = 100;
  std::array<ImuSample, WINDOW> imu_window{};
  imu_window[0] = {0.01f, -0.02f, 9.81f};
  imu_window[WINDOW - 1] = {0.0f, 0.0f, 9.80f};
  std::cout << "  IMU window capacity = " << imu_window.size()
            << ", front.az = " << imu_window.front().az
            << ", back.az = " << imu_window.back().az << '\n';

  // Lidar scan downsampling — span hides the storage choice.
  std::vector<float> scan(8);
  std::iota(scan.begin(), scan.end(), 1.0f);    // 1..8
  std::array<float, 4> reduced{};
  downsample(scan, reduced);
  print_floats("lidar downsampled", reduced);

  // Image rows over a flat frame buffer.
  constexpr int width = 4, height = 3, stride = 4;
  std::array<std::uint8_t, stride * height> frame = {1, 1, 1, 1,
                                                     2, 2, 2, 2,
                                                     3, 3, 3, 3};
  for (int y = 0; y < height; ++y) {
    std::span<const std::uint8_t> row{frame.data() + y * stride,
                                      static_cast<std::size_t>(width)};
    std::cout << "  row " << y << " sum = " << row_sum(row) << '\n';
  }
}

// ---------------------------------------------------------------------------
// 6. is_array trait — quick reference
// ---------------------------------------------------------------------------

struct Foo {};

static void is_array_demo() {
  std::cout << "--- std::is_array<T> ---\n";
  std::cout << std::boolalpha;
  std::cout << "  Foo                  -> " << std::is_array<Foo>::value << '\n';
  std::cout << "  Foo[3]               -> " << std::is_array<Foo[3]>::value << '\n';
  std::cout << "  int                  -> " << std::is_array<int>::value << '\n';
  std::cout << "  int[]                -> " << std::is_array<int[]>::value << '\n';
  std::cout << "  int[3]               -> " << std::is_array<int[3]>::value << '\n';
  std::cout << "  std::array<int, 3>   -> "
            << std::is_array<std::array<int, 3>>::value
            << "  (false! std::array is a class, not a C array)\n";
}

// ---------------------------------------------------------------------------

int main() {
  c_array_demo();
  multidim_demo();
  std_array_demo();
  span_demo();
  robotics_demo();
  is_array_demo();
  return 0;
}
