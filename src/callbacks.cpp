#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>

/*
Callbacks and callable objects — see docs/callable_callbacks.md.

A callback is just a callable handed to another component to be invoked
later. The robot/planner example below is the canonical illustration:
robot::move() doesn't know how the trajectory is generated, it just calls
whatever planner you wired in. The same wiring works for sensor data
callbacks, lifecycle events, and signal/slot patterns.

This file demonstrates each named technique in the doc that doesn't have
its own dedicated source file:
  1. Function pointer       (assigning a free function to a std::function)
  2. Functor                (operator() on a class)
  3. std::function + bind
  4. Lambda
  5. Member function pointer via std::bind
  6. Signals & slots        (portable, non-Qt, just std::function lists)
  7. Captureless lambda -> raw function pointer
*/

using CallbackFunction = std::function<std::vector<double>(double, double)>;

class Robot {
public:
  CallbackFunction m_planner;

  void executePlan(const std::vector<double> &trajectory) const {
    std::cout << "Robot is traversing the generated trajectory:\n";
    for (auto p : trajectory)
      std::cout << "  " << p << '\n';
  }

  void move(double start, double goal) {
    auto trajectory = m_planner(start, goal);
    executePlan(trajectory);
  }
};

// Free-function planner — works as a plain function pointer.
std::vector<double> planner_three_point(double start, double goal) {
  return {start, (start + goal) / 2, goal};
}

// Free-function planner used through std::bind.
std::vector<double> planner_uniform_20(double start, double goal) {
  double step = (goal - start) / 20.0;
  std::vector<double> values;
  for (double v = start; v < goal; v += step)
    values.push_back(v);
  return values;
}

// 2. Functor — a class with operator() that holds state (here: how many
// waypoints to emit). Maintaining state is the main reason to prefer a
// functor over a plain function pointer.
class LinearPlanner {
public:
  explicit LinearPlanner(int n_points) : n_points_(n_points) {}

  std::vector<double> operator()(double start, double goal) const {
    std::vector<double> values;
    values.reserve(n_points_);
    double step = (goal - start) / (n_points_ - 1);
    for (int i = 0; i < n_points_; ++i)
      values.push_back(start + i * step);
    return values;
  }

private:
  int n_points_;
};

// 5. A class whose method we want to expose as a callback. Real-world
// flavor: an IMU driver whose on_sample() method wants to be called by a
// CAN/serial frame dispatcher each time a new packet arrives.
class IMU {
public:
  void on_sample(double ax, double ay) {
    samples_.emplace_back(ax, ay);
    std::cout << "IMU got sample (" << ax << ", " << ay
              << "); total=" << samples_.size() << '\n';
  }

private:
  std::vector<std::pair<double, double>> samples_;
};

// 6. A minimal portable signal/slot: a list of std::function listeners
// invoked when the signal "fires". This is the same shape Qt exposes,
// without the moc dependency.
template <typename... Args> class Signal {
public:
  using Slot = std::function<void(Args...)>;

  void connect(Slot s) { slots_.push_back(std::move(s)); }

  void emit(Args... args) const {
    for (const auto &s : slots_)
      s(args...);
  }

private:
  std::vector<Slot> slots_;
};

// 7. A function that expects a raw C-style function pointer. A
// captureless lambda decays to one, which is the trick behind passing
// lambdas to legacy C APIs.
void run_with_raw_ptr(void (*fn)(int), int value) { fn(value); }

int main() {
  Robot myrobot;
  const double start = 1.0;
  const double goal = 5.0;

  // 1. Function pointer assigned directly to a std::function slot.
  std::cout << "=== plain function pointer ===\n";
  myrobot.m_planner = planner_three_point;
  myrobot.move(start, goal);

  // 2. Functor with internal state.
  std::cout << "=== functor (LinearPlanner with 5 points) ===\n";
  myrobot.m_planner = LinearPlanner{5};
  myrobot.move(start, goal);

  // 3. std::function holding a std::bind expression.
  std::cout << "=== std::function + std::bind ===\n";
  std::function<std::vector<double>(double, double)> bound_planner =
      std::bind(&planner_uniform_20, std::placeholders::_1,
                std::placeholders::_2);
  myrobot.m_planner = bound_planner;
  myrobot.move(start, goal);

  // 4. Lambda — most idiomatic modern form.
  std::cout << "=== lambda ===\n";
  myrobot.m_planner = [](double s, double g) {
    return std::vector<double>{s, g};
  };
  myrobot.move(start, goal);

  // 5. Member-function pointer wired through std::bind. Useful when a
  // generic dispatcher wants to call obj.method(args...) without knowing
  // the class.
  std::cout << "=== member function via std::bind ===\n";
  IMU imu;
  std::function<void(double, double)> imu_cb = std::bind(
      &IMU::on_sample, &imu, std::placeholders::_1, std::placeholders::_2);
  imu_cb(0.01, -0.02);
  imu_cb(0.03, 0.00);

  // 6. Signal/slot fan-out: one event, multiple listeners. Real-world
  // flavor: "estop pressed" notifying motor controller, logger, and UI.
  std::cout << "=== signal / slot fan-out ===\n";
  Signal<int> estop_pressed;
  estop_pressed.connect(
      [](int reason) { std::cout << "  motor: cutting power (reason=" << reason << ")\n"; });
  estop_pressed.connect(
      [](int reason) { std::cout << "  logger: estop logged, reason=" << reason << '\n'; });
  estop_pressed.connect(
      [](int reason) { std::cout << "  ui: estop banner shown (" << reason << ")\n"; });
  estop_pressed.emit(42);

  // 7. Captureless lambda implicitly converts to a raw function pointer.
  std::cout << "=== captureless lambda -> function pointer ===\n";
  run_with_raw_ptr([](int v) { std::cout << "  raw callback v=" << v << '\n'; },
                   7);
}
