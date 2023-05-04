#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>

typedef std::function<std::vector<double>(double, double)> CallbackFunction;

class robot {

public:
  CallbackFunction m_callback;
  // robot(const CallbackFunction &callback) : m_callback(callback) {}

  void makeMotion(std::vector<double> trajectory) {
    std::cout << "robot is traversing the generated trajectory:" << std::endl;
    for (const auto &p : trajectory)
      std::cout << p << std::endl;
  }

  void move(double start, double goal) {

    std::vector<double> trajectory = m_callback(start, goal);
    makeMotion(trajectory);
  }
};

std::vector<double> planer1(double start, double goal) {
  return {start, (start + goal) / 2, goal};
}
std::vector<double> planer2(double start, double goal) {
  return {start, start + (start + goal) / 3, 2 * (start + goal) / 3, goal};
}
int main() {

  std::function<std::vector<double>(double, double)> planer1_ptr =
      std::bind(&planer1, std::placeholders::_1, std::placeholders::_2);

  // robot myrobot1(planer1_ptr);

  double start, goal;
  start = 1;
  goal = 5;
  robot myrobot1;

  myrobot1.m_callback = planer1;
  myrobot1.move(start, goal);

  std::function<std::vector<double>(double, double)> planer2_ptr =
      std::bind(&planer2, std::placeholders::_1, std::placeholders::_2);

  myrobot1.m_callback = planer2_ptr;
  myrobot1.move(start, goal);

  auto planer_lambda = [](double start, double goal) {
    return std::vector<double>(10, 2);
  };

  myrobot1.m_callback = planer_lambda;
  myrobot1.move(start, goal);

}
