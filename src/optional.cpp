// Demo for std::optional — companion to docs/any_variant_optional.md.
//
// std::optional<T> encodes "a T, or nothing" in the type system. In robotics
// you reach for it whenever a value may legitimately not be there yet: a TF
// lookup before the chain connects, a sensor sample before the first frame
// arrives, an unset config field, etc. It replaces three older anti-patterns:
//   1. Returning a raw pointer and using nullptr as "absent".
//   2. Sentinel values (-1, NaN, "") that future readers will miss.
//   3. bool-return-plus-out-param, which mutates the out even on failure.

#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// --- Stand-in robotics types --------------------------------------------------

struct Transform {
  double tx{}, ty{}, tz{};
  double yaw{};
};

struct Pose {
  double x{}, y{}, yaw{};
};

struct GpsFix {
  double lat{}, lon{};
};

// --- 1. TF lookup: returns nullopt if the chain isn't connected yet ----------

// In a real TF buffer this would walk a tree of frames; here we fake a tiny
// table so the demo is self-contained.
std::optional<Transform> lookupTransform(std::string_view source,
                                         std::string_view target) {
  if (source == "odom" && target == "base_link") {
    return Transform{1.0, 2.0, 0.0, 0.1};
  }
  return std::nullopt;  // explicit absence — never return {}
}

// --- 2. Latest joint angle: encoder may not have published yet ---------------

std::optional<double> latestJointAngle(bool fresh) {
  if (!fresh)
    return std::nullopt;
  return 0.7854;  // ~45 deg
}

// --- 3. Latest pose buffer ---------------------------------------------------

class PoseBuffer {
 public:
  void push(Pose p) { last_ = p; }
  bool fresh() const { return last_.has_value(); }
  std::optional<Pose> read() const {
    if (!fresh())
      return std::nullopt;  // explicit, not "{}"
    return last_;
  }

 private:
  std::optional<Pose> last_;
};

// --- 4. GPS read used by the monadic chain demo ------------------------------

std::optional<GpsFix> readGpsFix(bool has_fix) {
  if (!has_fix)
    return std::nullopt;
  return GpsFix{37.7749, -122.4194};
}

Pose toLocalFrame(const GpsFix& f) {
  // Pretend we projected lat/lon into a local metric frame.
  return Pose{f.lat * 0.1, f.lon * 0.1, 0.0};
}

Pose deadReckon() { return Pose{99.0, 99.0, 0.0}; }

// =============================================================================

int main() {
  // ---- Section 2 (doc): TF lookup -----------------------------------------
  std::cout << "[1] TF lookup\n";
  if (auto tf = lookupTransform("odom", "base_link")) {
    std::cout << "    odom->base_link: (" << tf->tx << ", " << tf->ty
              << "), yaw=" << tf->yaw << '\n';
  } else {
    std::cout << "    odom->base_link not yet available\n";
  }

  if (auto tf = lookupTransform("map", "base_link"); !tf) {
    std::cout << "    map->base_link not yet available (expected)\n";
  }

  // ---- Section 2 (doc): three idioms for checking presence ----------------
  std::cout << "\n[2] Checking presence\n";
  // (a) Bind + test in one expression — preferred in realtime loops.
  if (auto angle = latestJointAngle(true)) {
    std::cout << "    bind+test: angle = " << *angle << '\n';
  }
  // (b) Explicit has_value().
  auto maybe_angle = latestJointAngle(false);
  std::cout << "    has_value(): " << std::boolalpha << maybe_angle.has_value()
            << '\n';
  // (c) operator bool — equivalent to has_value().
  if (!maybe_angle) {
    std::cout << "    operator bool: encoder hasn't published yet\n";
  }

  // ---- Section 2 (doc): three access modes, three failure modes -----------
  std::cout << "\n[3] Three ways to access\n";
  // *opt — UB if empty; safe here because we constructed it non-empty.
  auto a1 = latestJointAngle(true);
  std::cout << "    *opt          = " << *a1 << "  (only after a check!)\n";
  // .value_or(default) — safe fallback, ideal for realtime hot paths.
  double a2 = latestJointAngle(false).value_or(0.0);
  std::cout << "    value_or(0.0) = " << a2 << "  (safe fallback)\n";
  // .value() — throws std::bad_optional_access if empty.
  try {
    [[maybe_unused]] double a3 = latestJointAngle(false).value();
  } catch (const std::bad_optional_access& e) {
    std::cout << "    .value() on empty threw: " << e.what() << '\n';
  }

  // ---- Section 2 (doc): returning std::nullopt ----------------------------
  std::cout << "\n[4] Returning std::nullopt vs a value\n";
  PoseBuffer buf;
  std::cout << "    empty buffer:  read() has value? " << buf.read().has_value()
            << '\n';
  buf.push(Pose{1.0, 2.0, 0.5});
  if (auto p = buf.read()) {
    std::cout << "    after push:    pose = (" << p->x << ", " << p->y
              << ", yaw=" << p->yaw << ")\n";
  }

  // ---- Section 2 (doc): monadic ops (C++23) -------------------------------
  std::cout << "\n[5] Monadic chaining (C++23)\n";
#if defined(__cpp_lib_optional) && __cpp_lib_optional >= 202110L
  // Pose := transform(GPS fix) if available, else fall back to dead reckoning.
  auto pose_with_fix =
      readGpsFix(true)
          .transform([](const GpsFix& f) { return toLocalFrame(f); })
          .or_else([] { return std::optional<Pose>{deadReckon()}; });
  std::cout << "    with fix:    (" << pose_with_fix->x << ", "
            << pose_with_fix->y << ")\n";

  auto pose_no_fix =
      readGpsFix(false)
          .transform([](const GpsFix& f) { return toLocalFrame(f); })
          .or_else([] { return std::optional<Pose>{deadReckon()}; });
  std::cout << "    no fix:      (" << pose_no_fix->x << ", " << pose_no_fix->y
            << ")  <- dead reckoning fallback\n";
#else
  std::cout << "    (skipped — C++23 monadic ops not available in this build)\n";
#endif

  // ---- Section 2 (doc): anti-patterns -------------------------------------
  // Compile-time only; these are documented as bad ideas, no runtime demo.
  //   std::optional<int*>                  // pointless: int* already has nullptr
  //   std::optional<std::unique_ptr<T>>    // unique_ptr already encodes "maybe null"
  //   std::optional<T&>                    // illegal — references aren't reseatable
  std::cout << "\n[6] Anti-patterns: see docs/any_variant_optional.md\n";
  std::cout << "    Prefer T* for 'non-owning, may be absent'.\n";

  return 0;
}
