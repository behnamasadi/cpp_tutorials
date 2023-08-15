#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>

typedef std::function<std::vector<double>(double, double)> CallbackFunction;

class robot {

public:
  CallbackFunction m_planner;
  // robot(const CallbackFunction &callback) : m_planner(callback) {}

  void executePlan(std::vector<double> trajectory) {
    std::cout << "robot is traversing the generated trajectory:" << std::endl;
    for (const auto &p : trajectory)
      std::cout << p << std::endl;
  }

  void move(double start, double goal) {

    std::vector<double> trajectory = m_planner(start, goal);
    executePlan(trajectory);
  }
};

std::vector<double> planer1(double start, double goal) {
  return {start, (start + goal) / 2, goal};
}

std::vector<double> planer2(double start, double goal) {
  double step = (goal - start) / 20;
  std::vector<double> values;
  for (double value = start; value < goal; value += step)
    values.push_back(value);
  return values;
}

int main() {

  std::function<std::vector<double>(double, double)> planer1_ptr =
      std::bind(&planer1, std::placeholders::_1, std::placeholders::_2);

  // robot myrobot(planer1_ptr);

  double start, goal;
  start = 1;
  goal = 5;
  robot myrobot;

  myrobot.m_planner = planer1;
  myrobot.move(start, goal);

  std::function<std::vector<double>(double, double)> planer2_ptr =
      std::bind(&planer2, std::placeholders::_1, std::placeholders::_2);

  myrobot.m_planner = planer2_ptr;
  myrobot.move(start, goal);

  auto planer_lambda = [](double start, double goal) {
    return std::vector<double>(10, 2);
  };

  myrobot.m_planner = planer_lambda;
  myrobot.move(start, goal);

  // robot::m_planner(std::bind(&robot::m_planner, planer2, start, goal));
  // myrobot.move(start, goal);
}
