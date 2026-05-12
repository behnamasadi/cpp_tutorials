#include <algorithm>
#include <iostream>
#include <random>
#include <sstream>

class money {
private:
  int m_size = 10;

public:
  int m_value;
  int *data;

  explicit money(int value = 10) : m_value(value) {
    std::cout << "Constructor money with m_value: " << m_value << std::endl;

    data = new int[m_size];

    int upperBound = 20;
    int lowerBound = 0;
    int numberOfRandomNumbers = m_size;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(lowerBound, upperBound);

    for (int n = 0; n < numberOfRandomNumbers; ++n)
      data[n] = dis(gen);
  }

  ~money() {
    std::cout << "Destructor money with value: " << m_value << std::endl;
    delete[] data;
  }

  // () operator: returns m_value
  double operator()() { return m_value; }

  // friend free-function operators
  friend money operator-(const money &t1, int value);
  friend std::ostream &operator<<(std::ostream &os, const money &t);
  friend std::istream &operator>>(std::istream &is, money &t);

  // < operator as a member
  bool operator<(const money &other) const { return m_value < other.m_value; }

  // - operator as a member (money - money)
  money operator-(const money &other) {
    return money(m_value - other.m_value);
  }

  // Copy constructor for deep copy
  money(const money &other) : m_size(other.m_size), m_value(other.m_value) {
    std::cout << "Copy constructor money with m_value: " << m_value
              << std::endl;
    data = new int[m_size];
    std::copy(other.data, other.data + m_size, data);
  }

  // Copy assignment operator for deep copy
  money &operator=(const money &other) {
    std::cout << "Copy assignment money with m_value: " << other.m_value
              << std::endl;
    if (this != &other) {
      delete[] data;

      m_value = other.m_value;
      m_size = other.m_size;
      data = new int[m_size];
      std::copy(other.data, other.data + m_size, data);
    }
    return *this;
  }
};

// - operator as a non-member friend (money - int)
money operator-(const money &t1, int value) {
  return money(t1.m_value - value);
}

// << operator as a non-member friend: prints the data array
std::ostream &operator<<(std::ostream &os, const money &t) {
  for (int i = 0; i < t.m_size; i++)
    os << t.data[i] << " ";
  return os;
}

// >> operator as a non-member friend: reads m_value
std::istream &operator>>(std::istream &is, money &t) {
  is >> t.m_value;
  return is;
}

// Demonstrates < implemented outside the class as a free function
struct cell {
  int index;
  float cost;
};

bool operator<(const cell &lhs, const cell &rhs) { return lhs.cost < rhs.cost; }

int main() {
  std::cout << "--- () operator ---" << std::endl;
  money m1(7);
  std::cout << "m1() = " << m1() << std::endl;

  std::cout << "\n--- << operator (prints data array) ---" << std::endl;
  std::cout << "m1: " << m1 << std::endl;

  std::cout << "\n--- >> operator (reads m_value) ---" << std::endl;
  std::istringstream input("42");
  money m2;
  input >> m2;
  std::cout << "m2.m_value after >>: " << m2.m_value << std::endl;

  std::cout << "\n--- - operator (member: money - money) ---" << std::endl;
  money a(10), b(3);
  money c = a - b;
  std::cout << "a - b -> m_value: " << c.m_value << std::endl;

  std::cout << "\n--- - operator (friend: money - int) ---" << std::endl;
  money d = a - 4;
  std::cout << "a - 4 -> m_value: " << d.m_value << std::endl;

  std::cout << "\n--- < operator (member) ---" << std::endl;
  std::cout << "b < a ? " << (b < a) << std::endl;

  std::cout << "\n--- < operator (free function on cell) ---" << std::endl;
  cell c1{0, 2.5f}, c2{1, 1.0f};
  std::cout << "c1 < c2 ? " << (c1 < c2) << std::endl;

  std::cout << "\n--- copy constructor (deep copy) ---" << std::endl;
  money e = a;
  std::cout << "e.m_value: " << e.m_value << std::endl;

  std::cout << "\n--- copy assignment (deep copy) ---" << std::endl;
  money f;
  f = a;
  std::cout << "f.m_value: " << f.m_value << std::endl;

  std::cout << "\n--- end of main, destructors follow ---" << std::endl;
  return 0;
}
