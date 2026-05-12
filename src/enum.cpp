// Enums in C++: unscoped enum, scoped enum class, underlying type, and
// iterating over an enum. Matches docs/enum.md.

#include <iostream>
#include <type_traits>

// Unscoped enum used as a bitmask. Enumerators leak into the surrounding scope
// and implicitly convert to int.
enum color {
  None = 0,
  red = 1,
  blue = 2,
  green = 4,
  All = red | blue | green,
};

// Another unscoped enum. Its enumerators are int too, which is why a stray
// comparison with `color` compiles even though it makes no sense.
enum fruit {
  apple = 1,
  melon = 2,
  orange = 3,
};

// Scoped enum (enum class) with an explicit underlying type.
enum class animals : unsigned char { chicken, aligator, turtle };

// Scoped enum with the default underlying type (int).
enum class mamals { cow, fox, dog };

// Generic iterator that walks every value of an enum from beginVal to endVal.
template <typename C, C beginVal, C endVal> class Iterator {
  typedef typename std::underlying_type<C>::type val_t;
  int val;

public:
  Iterator(const C &f) : val(static_cast<val_t>(f)) {}
  Iterator() : val(static_cast<val_t>(beginVal)) {}
  Iterator operator++() {
    ++val;
    return *this;
  }
  C operator*() { return static_cast<C>(val); }
  Iterator begin() { return *this; }
  Iterator end() {
    static const Iterator endIter = ++Iterator(endVal);
    return endIter;
  }
  bool operator!=(const Iterator &i) { return val != i.val; }
};

// A consumer that takes a color and prints it as an int (unscoped enum
// converts implicitly).
void foo(color c) { std::cout << c << std::endl; }

void unscopedEnumDemo() {
  std::cout << "--- unscoped enum ---\n";
  // Dangerous but compiles: both sides decay to int.
  std::cout << "color::red == fruit::apple ? "
            << (color::red == fruit::apple ? "equal" : "not equal") << "\n";

  // Enumerators are visible without qualification.
  color c = red;
  std::cout << "red as int: " << c << "\n";
}

void scopedEnumDemo() {
  std::cout << "--- scoped enum class ---\n";
  // This would NOT compile, scoped enums do not implicitly convert:
  //   animals::chicken == mamals::cow
  // You must explicitly cast to compare against an underlying value.
  std::cout << "animals::chicken == static_cast<animals>(0) ? "
            << (animals::chicken == static_cast<animals>(0) ? "equal"
                                                            : "not equal")
            << "\n";
}

void underlyingTypeDemo() {
  std::cout << "--- underlying type ---\n";
  std::cout << "sizeof(animals) = " << sizeof(animals) << " (unsigned char)\n";
  std::cout << "sizeof(mamals)  = " << sizeof(mamals) << " (default int)\n";

  // Scoped enums need an explicit cast to get their integer value.
  std::cout << "static_cast<int>(animals::turtle) = "
            << static_cast<int>(animals::turtle) << "\n";
}

void iterateEnumDemo() {
  std::cout << "--- iterate over color ---\n";
  typedef Iterator<color, color::None, color::All> colorIterator;
  for (color c : colorIterator()) {
    foo(c);
  }
}

int main() {
  unscopedEnumDemo();
  scopedEnumDemo();
  underlyingTypeDemo();
  iterateEnumDemo();
  return 0;
}
