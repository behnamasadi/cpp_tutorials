#include <algorithm>
#include <iostream>
#include <vector>

class Student {
public:
  Student() = default;
  Student(const Student &rhs) { std::cout << "Copy" << std::endl; }
};

struct Sum {
  void operator()(int n) { sum += n; }
  int sum{0};
};

void rangeBasedVsIteratorLoopVsForeach() {
  /////////////////////// range based for-loops ///////////////////////

  /*
  for ( for-range-declaration : expression ) statement

  Range-for is as fast as possible since it caches the end iterator, uses
  pre-increment and only dereferences the iterator once.
  */
  std::vector<Student> vecStudent(10);

  // Choose auto const &x when you want to work with original items and will not
  // modify them.

  for (auto const &x : vecStudent)
    ;

  // Choose auto &x when you want to work with original items and may modify
  // them.
  std::cout << "Range based for with &" << std::endl;
  for (auto &x : vecStudent)
    ;

  // Choose auto x when you want to work with copies.
  std::cout << "Range based for without &" << std::endl;

  for (auto x : vecStudent)
    ;

  /////////////////////// regular for loops with iterators
  //////////////////////////
  // Here you get a cheap copy of the iterator, not the actual object:
  for (auto a = vecStudent.cbegin(); a != vecStudent.cend(); ++a) {
  }

  /////////////////////// for_each ///////////////////////

  std::for_each(vecStudent.begin(), vecStudent.end(), [](Student &n) { n; });
}

void forEachExample() {
  /*
  begin, end:
  begin will return an iterator or a const_iterator depending on the
  const-qualification of the object it is called on.

  ebegin, cend:
  cbegin will return a const_iterator unconditionally.

  https://www.fluentcpp.com/2019/02/07/why-you-should-use-stdfor_each-over-range-based-for-loops/
  */
  std::vector<int> vec;
  const std::vector<int> const_vec;

  vec.begin();  // iterator
  vec.cbegin(); // const_iterator

  const_vec.begin();  // const_iterator
  const_vec.cbegin(); // const_iterator

  std::vector<int> nums{3, 4, 2, 8, 15, 267};

  auto print = [](const int &n) { std::cout << " " << n; };

  std::cout << "before:";
  std::for_each(nums.cbegin(), nums.cend(), print);
  std::cout << '\n';

  std::for_each(nums.begin(), nums.end(), [](int &n) { n++; });

  // calls Sum::operator() for each number
  Sum s = std::for_each(nums.begin(), nums.end(), Sum());

  std::cout << "after: ";
  std::for_each(nums.cbegin(), nums.cend(), print);
  std::cout << '\n';
  std::cout << "sum: " << s.sum << '\n';
}

int main() { rangeBasedVsIteratorLoopVsForeach(); }
