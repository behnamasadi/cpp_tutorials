// Demos for nested_unnamed_anonymous_namespaces.md
//
// The doc covers two topics:
//   1) Anonymous (unnamed) namespaces -> translation-unit-local (internal linkage).
//   2) Nested namespaces -> classic form vs. the C++17 A::B::C shorthand.

#include <iostream>

// ---------------------------------------------------------------------------
// 1) Anonymous namespace: identifiers are local to this translation unit.
//    The doc also notes that you may extend an unnamed namespace within the
//    same translation unit, so we split it into two blocks like the doc does.
// ---------------------------------------------------------------------------
namespace {
int variable = 42;
void funct(int);  // declared here
}  // namespace

namespace {
void funct(int i) { std::cout << "anonymous funct, i = " << i << "\n"; }
}  // namespace

// ---------------------------------------------------------------------------
// 2a) Nested namespaces: classic (pre-C++17) form.
//     Imagine a small robot stack: Robot -> Sensors -> Lidar.
// ---------------------------------------------------------------------------
namespace Robot {
namespace Sensors {
namespace Lidar {
void scan() { std::cout << "Robot::Sensors::Lidar::scan (classic form)\n"; }
}  // namespace Lidar
}  // namespace Sensors
}  // namespace Robot

// ---------------------------------------------------------------------------
// 2b) Nested namespaces: C++17 shorthand A::B::C.
// ---------------------------------------------------------------------------
namespace Robot::Actuators::Motor {
void spin() { std::cout << "Robot::Actuators::Motor::spin (C++17 shorthand)\n"; }
}  // namespace Robot::Actuators::Motor

int main() {
    std::cout << "--- anonymous namespace ---\n";
    funct(variable);  // file-local function and variable

    std::cout << "--- nested namespace (classic) ---\n";
    Robot::Sensors::Lidar::scan();

    std::cout << "--- nested namespace (C++17 shorthand) ---\n";
    Robot::Actuators::Motor::spin();

    return 0;
}
