#include <cmath>
#include <exception>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>

int sub(int a, int b) { return a - b; }

void inc(int a, int &b, const int &c) {
  std::cout << "In function: " << a << ' ' << b << ' ' << c << '\n';

  a++;
  b++;
  return;
}

template <typename T> struct array {
  int m_length = 0;
  T *data;
  array(int length = 0) : m_length(length), data(new T[length]) {}
  ~array() { delete[] data; }
  T &operator[](std::size_t index) { return data[index]; }

  class Iterator {
  public:
    Iterator(T *ptr) : m_ptr(ptr) {}

    Iterator &operator++() {
      ++m_ptr;
      return *this;
    }

    // Iterator & ++operator() {
    //   m_ptr++;
    //   return *this;
    // }

    Iterator &operator--() {
      --m_ptr;
      return *this;
    }

    // bool operator!=(const Iterator &rhs) const { return m_ptr != rhs.m_ptr; }

    bool operator!=(const Iterator &other) const {
      return m_ptr != other.m_ptr;
    }

    T &operator*() { return *m_ptr; }

  private:
    T *m_ptr;
  };

public:
  Iterator begin() { return Iterator(data); }
  Iterator end() { return Iterator(data + m_length); }
};

int main() {
  // double number = 3.1914;

  // std::cout << "default std::cout.precision(): " << std::cout.precision()
  //           << "\n";

  // std::cout << number << "\n";
  // std::cout << "std::setprecision(1): " << std::setprecision(1) << number
  //           << "\n";

  // std::cout << "std::fixed <<std::setprecision(7): " << std::fixed
  //           << std::setprecision(7) << number << "\n";

  // int n = 10;

  // std::cout << "Using pow : " << pow(10, n) << std::endl;
  // std::cout << "std::fixed <<" << std::fixed << pow(10, n) << std::endl;

  // std::cout << std::invoke(sub, 2, 5) << std::endl;
  // std::cout << std::invoke(sub, 5, 2) << std::endl;

  // auto subFunc1 = std::bind(sub, std::placeholders::_1,
  // std::placeholders::_2); auto subFunc2 = std::bind(sub,
  // std::placeholders::_2, std::placeholders::_1);

  // std::cout << subFunc1(2, 5) << std::endl;
  // std::cout << subFunc2(2, 5) << std::endl;

  // auto f = std::bind(foo, std::ref(x));
  // int a, b, c;

  // a = 1;
  // b = 10;
  // c = 100;
  // std::cout << "before bind: " << a << ' ' << b << ' ' << c << '\n';
  // auto f = std::bind(inc, a, std::ref(b), std::cref(c));
  // a = 5;
  // b = 50;
  // c = 500;
  // std::cout << "before call: " << a << ' ' << b << ' ' << c << '\n';
  // f();
  // std::cout << "after call: " << a << ' ' << b << ' ' << c << '\n';

  std::vector<int> foo(3);
  array<int> my_arr(4);

  my_arr[0] = 3;
  my_arr[1] = 7;
  my_arr[2] = 5;
  my_arr[3] = 6;

  // std::sort(my_arr.begin(), my_arr.end());

  // std::cout << *(my_arr++) << std::endl;
  for (array<int>::Iterator it = my_arr.begin(); it != my_arr.end(); ++it) {

    std::cout << *it << std::endl;
  }
}