// Iterators and loops: categories, range-for, for_each, std::next/advance/distance.
// Robotics flavor: a small buffer of sensor samples (e.g. joint angles, ticks).

#include <algorithm>
#include <forward_list>
#include <iostream>
#include <iterator>
#include <list>
#include <type_traits>
#include <vector>

// ---------- 1.1 input iterator ----------
// std::istream_iterator reads from a stream. We feed it from a stringstream
// instead of std::cin so the demo runs unattended.
#include <sstream>
void inputIteratorDemo() {
  std::cout << "-- input iterator --\n";
  std::istringstream samples("12 34 56 78");
  std::istream_iterator<int> it(samples), end;
  std::vector<int> ticks(it, end);
  std::cout << "read: ";
  for (int t : ticks)
    std::cout << t << " ";
  std::cout << "\n";
}

// ---------- 1.2 output iterator ----------
void outputIteratorDemo() {
  std::cout << "-- output iterator --\n";
  std::vector<int> ticks{10, 20, 30, 40, 50};
  std::ostream_iterator<int> out(std::cout, " ");
  std::cout << "write: ";
  std::copy(ticks.begin(), ticks.end(), out);
  std::cout << "\n";
}

// ---------- 1.3 forward iterator ----------
void forwardIteratorDemo() {
  std::cout << "-- forward iterator (std::forward_list) --\n";
  std::forward_list<int> ticks{10, 20, 30, 40, 50};
  for (auto it = ticks.begin(); it != ticks.end(); ++it)
    *it += 5; // forward iterators allow read + write
  std::cout << "ticks: ";
  for (int t : ticks)
    std::cout << t << " ";
  std::cout << "\n";
}

// ---------- 1.4 bidirectional iterator ----------
void bidirectionalIteratorDemo() {
  std::cout << "-- bidirectional iterator (std::list) --\n";
  std::list<int> ticks{10, 20, 30, 40, 50};
  std::cout << "forward:  ";
  for (auto it = ticks.begin(); it != ticks.end(); ++it)
    std::cout << *it << " ";
  std::cout << "\nbackward: ";
  for (auto it = ticks.end(); it != ticks.begin();) {
    --it;
    std::cout << *it << " ";
  }
  std::cout << "\n";
}

// ---------- 1.5 random access iterator ----------
void randomAccessIteratorDemo() {
  std::cout << "-- random access iterator (std::vector) --\n";
  std::vector<int> ticks{10, 20, 30, 40, 50};
  std::cout << "third element: " << *(ticks.begin() + 2) << "\n";
  std::cout << "backward: ";
  for (auto it = ticks.end() - 1; it >= ticks.begin(); --it)
    std::cout << *it << " ";
  std::cout << "\n";
}

// ---------- 2. determine iterator category ----------
template <typename Iterator> void printCategory(Iterator) {
  using cat = typename std::iterator_traits<Iterator>::iterator_category;
  if (std::is_same<cat, std::input_iterator_tag>::value)
    std::cout << "input\n";
  else if (std::is_same<cat, std::output_iterator_tag>::value)
    std::cout << "output\n";
  else if (std::is_same<cat, std::forward_iterator_tag>::value)
    std::cout << "forward\n";
  else if (std::is_same<cat, std::bidirectional_iterator_tag>::value)
    std::cout << "bidirectional\n";
  else if (std::is_same<cat, std::random_access_iterator_tag>::value)
    std::cout << "random access\n";
  else
    std::cout << "unknown\n";
}

void iteratorCategoryDemo() {
  std::cout << "-- iterator categories via iterator_traits --\n";
  std::vector<int> v{1, 2, 3};
  std::list<int> l{1, 2, 3};
  std::forward_list<int> f{1, 2, 3};
  std::cout << "vector::iterator        -> ";
  printCategory(v.begin());
  std::cout << "list::iterator          -> ";
  printCategory(l.begin());
  std::cout << "forward_list::iterator  -> ";
  printCategory(f.begin());
}

// ---------- 3. custom container with custom iterator ----------
template <typename T> class SampleBuffer {
public:
  SampleBuffer(std::size_t n) : m_size(n), m_data(new T[n]) {}
  ~SampleBuffer() { delete[] m_data; }
  T &operator[](std::size_t i) { return m_data[i]; }

  class Iterator {
  public:
    Iterator(T *p) : ptr(p) {}
    Iterator &operator++() {
      ++ptr;
      return *this;
    }
    bool operator!=(const Iterator &o) const { return ptr != o.ptr; }
    T &operator*() const { return *ptr; }

  private:
    T *ptr;
  };

  Iterator begin() { return Iterator(m_data); }
  Iterator end() { return Iterator(m_data + m_size); }

private:
  std::size_t m_size;
  T *m_data;
};

void customIteratorDemo() {
  std::cout << "-- custom container + iterator --\n";
  SampleBuffer<int> buf(5);
  for (std::size_t i = 0; i < 5; ++i)
    buf[i] = static_cast<int>(i + 1);
  std::cout << "buf: ";
  for (auto it = buf.begin(); it != buf.end(); ++it)
    std::cout << *it << " ";
  std::cout << "\n";
}

// ---------- 4. range based for ----------
void rangeBasedForDemo() {
  std::cout << "-- range based for --\n";
  std::vector<int> ticks{1, 2, 3};

  std::cout << "auto const &: ";
  for (auto const &x : ticks)
    std::cout << x << " ";
  std::cout << "\n";

  std::cout << "auto &     : ";
  for (auto &x : ticks)
    x += 10;
  for (int x : ticks)
    std::cout << x << " ";
  std::cout << "\n";

  std::cout << "auto       : ";
  for (auto x : ticks) // copy
    std::cout << x << " ";
  std::cout << "\n";
}

// ---------- begin/cbegin ----------
void beginVsCbeginDemo() {
  std::cout << "-- begin vs cbegin --\n";
  std::vector<int> ticks{1, 2, 3};
  // begin() yields iterator (mutable); cbegin() yields const_iterator (read-only).
  for (auto it = ticks.cbegin(); it != ticks.cend(); ++it)
    std::cout << *it << " ";
  std::cout << "\n";
}

// ---------- for_each ----------
struct Sum {
  void operator()(int n) { sum += n; }
  int sum{0};
};

void forEachDemo() {
  std::cout << "-- std::for_each --\n";
  std::vector<int> nums{3, 4, 2, 8, 15};
  auto print = [](int n) { std::cout << n << " "; };

  std::cout << "before: ";
  std::for_each(nums.cbegin(), nums.cend(), print);
  std::cout << "\n";

  std::for_each(nums.begin(), nums.end(), [](int &n) { ++n; });

  Sum s = std::for_each(nums.begin(), nums.end(), Sum());

  std::cout << "after:  ";
  std::for_each(nums.cbegin(), nums.cend(), print);
  std::cout << "\nsum: " << s.sum << "\n";
}

// ---------- std::next / advance / distance ----------
void nextAdvanceDistanceDemo() {
  std::cout << "-- std::next / advance / distance --\n";
  std::vector<int> v{4, 5, 6, 1, 3};

  auto it = v.begin();
  auto nx = std::next(it, 3);
  std::cout << "*it=" << *it << ", *next(it,3)=" << *nx << "\n";
  std::cout << "distance(it,nx)=" << std::distance(it, nx) << "\n";

  std::advance(it, 3);
  std::cout << "after advance(it,3): *it=" << *it << "\n";

  it = v.end();
  nx = std::next(it, -2);
  std::cout << "*next(end,-2)=" << *nx << "\n";
}

int main() {
  inputIteratorDemo();
  outputIteratorDemo();
  forwardIteratorDemo();
  bidirectionalIteratorDemo();
  randomAccessIteratorDemo();
  iteratorCategoryDemo();
  customIteratorDemo();
  rangeBasedForDemo();
  beginVsCbeginDemo();
  forEachDemo();
  nextAdvanceDistanceDemo();
  return 0;
}
