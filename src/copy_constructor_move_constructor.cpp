// Companion to docs/copy_constructor_move_constructor.md.
//
// The smallest class that owns a heap resource, so the four SMFs that the
// doc covers — copy ctor, copy assign, move ctor, move assign — each have
// something real to do. Every SMF prints which one ran, so the labelled
// output in main() shows when each fires.
//
// Rule-of-5 sibling pieces:
//   - copy-and-swap idiom    -> see src/copy_and_swap.cpp
//   - all six SMFs in detail -> see src/class/special_member_functions.cpp
// This file stays narrowly on copy vs. move.
//
// Build: cmake --build build --target copy_constructor_move_constructor -j 4
// Run:   ./build/Debug/copy_constructor_move_constructor

#include <iostream>
#include <utility>
#include <vector>

// Minimal resource-owning class. Holds one heap int. That is enough to make
// copy (allocate + duplicate) visibly different from move (steal pointer).
class Resource {
public:
  // Normal ctor — allocates.
  explicit Resource(int v) : data_(new int(v)) {
    std::cout << "  ctor(" << v << ")\n";
  }

  // Copy constructor — deep copy: new heap int, copy the value.
  Resource(const Resource &other) : data_(new int(*other.data_)) {
    std::cout << "  copy ctor\n";
  }

  // Copy assignment — release old, deep copy new. Self-assignment guard.
  Resource &operator=(const Resource &other) {
    std::cout << "  copy assign\n";
    if (this == &other)
      return *this;
    delete data_;
    data_ = new int(*other.data_);
    return *this;
  }

  // Move constructor — steal the pointer, null the source. noexcept so
  // std::vector<Resource> moves (not copies) on reallocation.
  Resource(Resource &&other) noexcept : data_(other.data_) {
    std::cout << "  move ctor\n";
    other.data_ = nullptr;
  }

  // Move assignment — release old, steal new, null the source.
  Resource &operator=(Resource &&other) noexcept {
    std::cout << "  move assign\n";
    if (this == &other)
      return *this;
    delete data_;
    data_ = other.data_;
    other.data_ = nullptr;
    return *this;
  }

  // Destructor — delete is fine on nullptr (moved-from objects).
  ~Resource() {
    std::cout << "  dtor\n";
    delete data_;
  }

private:
  int *data_ = nullptr;
};

// Helpers used by main() to demonstrate the situations the doc lists.
static void take_by_value(Resource r) { (void)r; }
static Resource make_temp(int v) { return Resource(v); }

int main() {
  std::cout << "\n== copy constructor ==\n";
  Resource a(1);
  std::cout << "Resource b = a;\n";
  Resource b = a; // copy ctor — copy-initialization
  std::cout << "Resource c(a);\n";
  Resource c(a); // copy ctor — direct-initialization
  std::cout << "take_by_value(a);\n";
  take_by_value(a); // copy ctor into parameter, then dtor on return

  std::cout << "\n== copy assignment ==\n";
  Resource x(10), y(20);
  std::cout << "y = x;\n";
  y = x; // copy assign
  std::cout << "y = y;  // self-assign\n";
  y = y; // self-assignment guard kicks in

  std::cout << "\n== move constructor ==\n";
  std::cout << "Resource m1 = std::move(a);\n";
  Resource m1 = std::move(a); // move ctor — a is now empty
  std::cout << "Resource m2 = make_temp(2);  // prvalue, mandatory elision\n";
  Resource m2 = make_temp(2); // C++17: no copy, no move — direct ctor
  std::cout << "Resource m3 = Resource(3);   // prvalue, mandatory elision\n";
  Resource m3 = Resource(3);

  std::cout << "\n== move assignment ==\n";
  Resource p(30), q(40);
  std::cout << "q = std::move(p);\n";
  q = std::move(p); // move assign — q frees its old int, takes p's

  std::cout << "\n== vector with noexcept move ==\n";
  // Because move ctor is noexcept, push_back's reallocation moves elements
  // instead of copying them.
  std::vector<Resource> v;
  v.reserve(2);
  std::cout << "v.emplace_back(100);\n";
  v.emplace_back(100);
  std::cout << "v.push_back(Resource(101));  // move into container\n";
  v.push_back(Resource(101));

  std::cout << "\n== end of main, destructors run in reverse ==\n";
  return 0;
}
