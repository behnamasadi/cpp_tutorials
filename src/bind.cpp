#include <algorithm>
#include <cmath>
#include <functional> // std::bind, std::placeholders, std::function
#include <iostream>
#include <vector>

/*
std::bind is a functional adaptor: it takes a callable and returns a new
callable with some arguments pre-bound or reordered (partial application).

Real-world feel: bind a sensor-read function so a scheduler can call it with
no extra context, or bind a member function (e.g. Motor::set_torque) onto a
specific instance so it can be stored in a std::function<void(double)>.

See docs/callable_callbacks.md, sections 3 and 5.
*/

// A free function we will partially apply with std::bind.
void log_event(int actuator_id, int code) {
  std::cout << "[actuator " << actuator_id << "] event code: " << code << '\n';
}

// A function template to show that bind also works with instantiated
// templates.
template <typename T> void print_template(T value) {
  std::cout << value << '\n';
}

// A class with a member function — bind is the classic way to turn
// &Class::method into a free-standing callable bound to a specific instance.
class Motor {
public:
  explicit Motor(int id) : id_(id) {}

  void set_torque(double nm) const {
    std::cout << "Motor " << id_ << " torque -> " << nm << " Nm\n";
  }

private:
  int id_;
};

int main() {

  // 1. Bind by value vs std::ref: captured-by-value snapshots arguments at
  //    bind time; std::ref keeps a live reference, so later mutations show
  //    up when the bound callable is invoked.
  {
    std::cout << "--- bind by value vs std::ref ---\n";
    int actuator_id = 3;
    int code = 5;

    auto by_value = std::bind(&log_event, actuator_id, code);
    auto by_ref = std::bind(&log_event, std::ref(actuator_id), std::ref(code));

    actuator_id = 4;
    code = 6;

    by_value(); // prints 3, 5 (snapshot)
    by_ref();   // prints 4, 6 (live reference)
  }

  // 2. Reorder/forward arguments with std::placeholders.
  {
    std::cout << "--- placeholders reorder arguments ---\n";
    auto reversed =
        std::bind(&log_event, std::placeholders::_2, std::placeholders::_1);

    log_event(1, 99);  // original order
    reversed(1, 99);   // swapped: actuator 99, code 1
  }

  // 3. Wrap a bind expression in std::function for a stable, type-erased
  //    callable type that can be stored, copied, and passed around.
  {
    std::cout << "--- std::function holding a bind expression ---\n";
    auto bound = std::bind(&log_event, std::placeholders::_1, 42);
    std::function<void(int)> emit_with_fixed_code = bound;
    emit_with_fixed_code(7); // actuator 7, code 42
  }

  // 4. Bind to an instantiation of a function template.
  {
    std::cout << "--- bind to a template instantiation ---\n";
    auto printer = std::bind(&print_template<int>, std::placeholders::_1);
    std::function<void(int)> f(printer);
    f(123);
  }

  // 5. Use a bind expression as the projection in <algorithm>: cube every
  //    sample by partially applying std::pow with the exponent fixed.
  {
    std::cout << "--- bind in std::transform ---\n";
    std::vector<int> samples = {1, 2, 3};
    std::vector<int> cubed(samples.size(), 0);

    auto cube = std::bind(&std::pow<int, int>, std::placeholders::_1, 3);
    std::transform(samples.begin(), samples.end(), cubed.begin(), cube);

    for (auto v : cubed)
      std::cout << v << '\n';
  }

  // 6. Bind a member function onto an instance — the canonical "wrap a
  //    method as a callback" pattern. The first bound argument is the
  //    object (or pointer); placeholders take the remaining arguments.
  {
    std::cout << "--- bind a member function ---\n";
    Motor m{17};

    auto set_torque_on_m =
        std::bind(&Motor::set_torque, &m, std::placeholders::_1);

    std::function<void(double)> torque_cb = set_torque_on_m;
    torque_cb(12.5);
  }
}
