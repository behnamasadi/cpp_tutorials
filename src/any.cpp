// Demo for std::any — companion to docs/any_variant_optional.md.
//
// std::any stores "some value, type known only at runtime." It is the
// last-resort tool in the optional / variant / any trilogy: reach for it only
// when the type set genuinely is not known at compile time (so variant won't
// fit). The canonical robotics use case is a parameter server / behavior-tree
// blackboard: heterogeneous keyed storage that crosses a stable API boundary.
//
// Rule of thumb (from the doc): if you think you need std::any, first ask
// whether std::variant would do. If yes, use variant. Confine any to the
// boundaries of one subsystem.

#include <any>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>

// -----------------------------------------------------------------------------
// A tiny parameter blackboard — matches the example in section 4 of the doc.
//
// Notice the API shape: std::any is the *storage* type, but every read funnels
// through a typed get<T>() that returns std::optional<T>. Callers never touch
// raw any. This is the pattern you want when wrapping any in a real system
// (e.g. a BT.CPP-style blackboard).
// -----------------------------------------------------------------------------
class Blackboard {
 public:
  template <class T>
  void set(std::string key, T value) {
    static_assert(std::is_copy_constructible_v<T>,
                  "Blackboard values must be copy-constructible (std::any "
                  "requirement)");
    data_[std::move(key)] = std::move(value);
  }

  template <class T>
  std::optional<T> get(const std::string& key) const {
    auto it = data_.find(key);
    if (it == data_.end())
      return std::nullopt;  // key missing
    if (auto* p = std::any_cast<T>(&it->second))
      return *p;
    return std::nullopt;  // wrong type — caller asked for the wrong T
  }

  bool contains(const std::string& key) const {
    return data_.find(key) != data_.end();
  }

  // For demo purposes: what type does the stored any hold?
  const std::type_info& type_of(const std::string& key) const {
    auto it = data_.find(key);
    return it == data_.end() ? typeid(void) : it->second.type();
  }

 private:
  std::unordered_map<std::string, std::any> data_;
};

// -----------------------------------------------------------------------------
// A move-only sensor handle — demonstrates the CopyConstructible requirement.
// -----------------------------------------------------------------------------
class SensorHandle {
 public:
  SensorHandle() = default;
  SensorHandle(const SensorHandle&) = delete;
  SensorHandle& operator=(const SensorHandle&) = delete;
  SensorHandle(SensorHandle&&) = default;
  SensorHandle& operator=(SensorHandle&&) = default;

  int fd() const { return 42; }
};

int main() {
  // ---- Section 4 (doc): basic any usage -----------------------------------
  std::cout << "[1] Basic any\n";
  std::any a = 1;
  std::cout << "    holds int = " << std::any_cast<int>(a) << '\n';
  a = std::string{"base_link"};
  std::cout << "    now holds string = " << std::any_cast<std::string>(a)
            << '\n';

  // ---- Section 4 (doc): any_cast — throwing vs pointer form ---------------
  std::cout << "\n[2] any_cast: throwing vs pointer form\n";
  // Pointer form returns nullptr on mismatch — preferred on hot paths.
  if (auto* p = std::any_cast<int>(&a)) {
    std::cout << "    (unexpected) int = " << *p << '\n';
  } else {
    std::cout << "    any_cast<int>(&a) returned nullptr (a holds string)\n";
  }
  // Value form throws std::bad_any_cast on mismatch — fine in init code.
  try {
    [[maybe_unused]] int v = std::any_cast<int>(a);
  } catch (const std::bad_any_cast& e) {
    std::cout << "    any_cast<int>(a) threw: " << e.what() << '\n';
  }

  // ---- Section 4 (doc): the parameter blackboard --------------------------
  std::cout << "\n[3] Parameter blackboard\n";
  Blackboard bb;
  bb.set("max_speed", 1.5);
  bb.set("base_frame", std::string{"base_link"});
  bb.set("num_lidars", 2);

  double max_speed = bb.get<double>("max_speed").value_or(0.0);
  std::string frame = bb.get<std::string>("base_frame").value_or("<unset>");
  int lidars = bb.get<int>("num_lidars").value_or(0);

  std::cout << "    max_speed   = " << max_speed << '\n';
  std::cout << "    base_frame  = " << frame << '\n';
  std::cout << "    num_lidars  = " << lidars << '\n';

  // Wrong type — get<T> returns nullopt instead of throwing.
  auto wrong = bb.get<int>("max_speed");
  std::cout << "    get<int>(\"max_speed\") has_value? " << std::boolalpha
            << wrong.has_value() << "  (stored as double)\n";

  // Missing key.
  auto missing = bb.get<double>("never_set");
  std::cout << "    get<double>(\"never_set\") has_value? " << missing.has_value()
            << '\n';

  // ---- Section 4 (doc): CopyConstructible requirement ---------------------
  std::cout << "\n[4] CopyConstructible requirement\n";
  static_assert(!std::is_copy_constructible_v<SensorHandle>,
                "SensorHandle is intentionally move-only for this demo");
  // std::any a_handle = SensorHandle{};   // would fail to compile
  SensorHandle h;
  std::any a_ptr = &h;  // pointers are copyable
  std::cout << "    stored SensorHandle* fd=" << std::any_cast<SensorHandle*>(a_ptr)->fd()
            << "  (pointer form: any holds the address, not the object)\n";

  std::any a_shared = std::make_shared<SensorHandle>();
  std::cout << "    stored shared_ptr<SensorHandle>, type = "
            << a_shared.type().name() << '\n';

  // ---- Reset / empty check -----------------------------------------------
  std::cout << "\n[5] reset() and has_value()\n";
  std::any e = 3.14;
  std::cout << "    before reset: has_value=" << e.has_value() << '\n';
  e.reset();
  std::cout << "    after reset:  has_value=" << e.has_value() << '\n';

  return 0;
}
