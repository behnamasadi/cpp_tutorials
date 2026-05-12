// Companion to docs/any_variant_optional.md §3 — std::variant.
//
// Covers the doc's full progression:
//   - holds_alternative / get / get_if
//   - the minimal generic-lambda visit
//   - hand-written visitor struct (Step A in the doc)
//   - overloaded{Fs...} helper (Step B)
//   - returning a value from visit
//   - multi-variant visit (cross-product dispatch)
//   - PlanResult: the canonical planner example with three outcomes

#include <iostream>
#include <string>
#include <variant>
#include <vector>

// --- The overloaded helper (doc Step B, would normally live in a util header) ---
template <class... Fs> struct overloaded : Fs... {
  using Fs::operator()...;
};
template <class... Fs> overloaded(Fs...) -> overloaded<Fs...>;

// --- Planner result: three outcomes, each with its own data ---
struct Pose {
  double x, y;
};
struct PathFound {
  std::vector<Pose> waypoints;
  double cost;
};
struct NoPath {
  std::string reason;
};
struct Partial {
  std::vector<Pose> waypoints;
  double coverage;
};
using PlanResult = std::variant<PathFound, NoPath, Partial>;

// --- Sensor message bus (doc's other canonical example) ---
struct LidarScan {
  std::vector<float> ranges;
};
struct ImuSample {
  float ax, ay, az;
};
struct GpsFix {
  double lat, lon;
};
using SensorMsg = std::variant<LidarScan, ImuSample, GpsFix>;

// Step A: hand-written visitor struct (one operator() per alternative).
// Forget one alternative and this won't compile.
struct SensorVisitor {
  void operator()(const LidarScan &s) const {
    std::cout << "  LidarScan: " << s.ranges.size() << " ranges\n";
  }
  void operator()(const ImuSample &s) const {
    std::cout << "  ImuSample: a=(" << s.ax << "," << s.ay << "," << s.az << ")\n";
  }
  void operator()(const GpsFix &s) const {
    std::cout << "  GpsFix: " << s.lat << ", " << s.lon << '\n';
  }
};

void demo_get_and_get_if() {
  std::cout << "--- holds_alternative / get / get_if ---\n";

  PlanResult r = PathFound{{{0, 0}, {1, 1}, {2, 2}}, /*cost=*/12.4};
  std::cout << "  active index: " << r.index() << " (0=PathFound, 1=NoPath, 2=Partial)\n";

  if (std::holds_alternative<PathFound>(r)) {
    const auto &p = std::get<PathFound>(r);
    std::cout << "  PathFound: " << p.waypoints.size() << " waypoints, cost=" << p.cost << '\n';
  }

  // get_if returns nullptr instead of throwing — preferred for "maybe this type."
  r = NoPath{"goal in obstacle"};
  if (auto *p = std::get_if<NoPath>(&r))
    std::cout << "  NoPath: " << p->reason << '\n';
  if (auto *p = std::get_if<PathFound>(&r))
    std::cout << "  PathFound (won't print): " << p->cost << '\n';
}

void demo_minimal_visit() {
  std::cout << "--- minimal visit with one generic lambda ---\n";

  std::variant<int, double, std::string> v = 3.14;
  std::visit([](const auto &x) { std::cout << "  " << x << '\n'; }, v);

  v = std::string{"hi"};
  std::visit([](const auto &x) { std::cout << "  " << x << '\n'; }, v);
}

void demo_visitor_struct() {
  std::cout << "--- Step A: hand-written visitor struct ---\n";

  std::vector<SensorMsg> bus = {
      LidarScan{{1.0f, 2.0f, 3.0f}},
      ImuSample{0.1f, 0.0f, 9.81f},
      GpsFix{37.7749, -122.4194},
  };
  for (const auto &m : bus)
    std::visit(SensorVisitor{}, m);
}

void demo_overloaded() {
  std::cout << "--- Step B: overloaded{...} helper, inline ---\n";

  PlanResult outcomes[] = {
      PathFound{{{0, 0}, {1, 0}}, 7.0},
      NoPath{"unreachable"},
      Partial{{{0, 0}}, 0.42},
  };

  // The PlanResult dispatcher the doc walks through — each branch handles only
  // the data that actually makes sense for that outcome. Add a fourth
  // alternative to PlanResult and this won't compile until a handler is added.
  for (const auto &r : outcomes) {
    std::visit(overloaded{
                   [](const PathFound &p) {
                     std::cout << "  PathFound: " << p.waypoints.size()
                               << " waypoints, cost=" << p.cost << '\n';
                   },
                   [](const NoPath &n) {
                     std::cout << "  NoPath: " << n.reason << '\n';
                   },
                   [](const Partial &p) {
                     std::cout << "  Partial: coverage=" << p.coverage << '\n';
                   },
               },
               r);
  }
}

void demo_return_value() {
  std::cout << "--- returning a value from visit ---\n";

  auto estimateLatencyMs = [](const SensorMsg &m) {
    return std::visit(overloaded{
                          [](const LidarScan &) { return 30.0; },
                          [](const ImuSample &) { return 1.0; },
                          [](const GpsFix &) { return 50.0; },
                      },
                      m);
  };

  SensorMsg msgs[] = {LidarScan{}, ImuSample{}, GpsFix{}};
  for (const auto &m : msgs)
    std::cout << "  latency = " << estimateLatencyMs(m) << " ms\n";
}

void demo_multi_visit() {
  std::cout << "--- multi-variant visit (cross-product dispatch) ---\n";

  std::variant<int, double> a = 1;
  std::variant<int, double> b = 2.0;

  // visit passes one argument per variant to the callable.
  auto sum = std::visit([](auto x, auto y) -> double { return x + y; }, a, b);
  std::cout << "  sum = " << sum << " (expected 3)\n";
}

int main() {
  demo_get_and_get_if();
  demo_minimal_visit();
  demo_visitor_struct();
  demo_overloaded();
  demo_return_value();
  demo_multi_visit();
}
