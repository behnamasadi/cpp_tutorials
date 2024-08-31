// count min max compare linear_searh attribute
#include <algorithm>
#include <array>
#include <cctype>
#include <forward_list>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <queue>
#include <random>

/*
std::unique
std::rotate
std::count
std::min_max
std::distance
std::find
std::search
std::adjacent_find
std::equal
std::mismatch
std::partition
std::transform
std::swap
std::exchange
std::fill
std::generate
std::replace
std::remove
std::next_permutation
std::lexicographical_compare
std::shuffel
std::sort
std::heap
std::binary_search
std::includes
std::priority_queue
std::lower_upper_equal_bound
std::upper_bound
std::advance
std::merge
std::accumulate
*/

template <typename T> void print_queue(T &q) {
  while (!q.empty()) {
    std::cout << q.top() << " ";
    q.pop();
  }
  std::cout << '\n';
}

template <typename T> void printArray(T array) {
  for (const auto &element : array)
    std::cout << element << " ";
}

bool IsOdd(int i) { return (i % 2) == 1; }

void unique() {
  // std::unique
  // will eliminate every consecutive group of equivalent elements
  std::vector<int> vec1 = {2, 1, 5, 2, 2, 3, 1, 1, 7};
  std::vector<int>::iterator last_it =
      std::unique(vec1.begin(), vec1.end()); //==> 2,1,5,2,3,1,7

  for (std::vector<int>::iterator it = vec1.begin(); it != last_it;
       it++) // 1,5,3,1,7
  {
    std::cout << *it << ',';
  }
  std::cout << '\n';

  std::sort(vec1.begin(), vec1.end());
  last_it = std::unique(vec1.begin(), vec1.end());

  for (std::vector<int>::iterator it = vec1.begin(); it != vec1.end(); it++) {
    std::cout << *it << ',';
  }
  std::cout << '\n';

  vec1.erase(last_it, vec1.end());
  for (int i : vec1)
    std::cout << i << " ";
  std::cout << "\n";

  // std::unique_copy(vec1.begin(),vec1.end(),vec2.being());

  // remove consecutive spaces
  std::string s = "wanna go    to      space?";
  auto end = std::unique(s.begin(), s.end(), [](char l, char r) {
    return std::isspace(l) && std::isspace(r) && l == r;
  });
  // s now holds "wanna go to space?xxxxxxxx", where 'x' is indeterminate
  std::cout << std::string(s.begin(), end) << '\n';
}

void rotate() {
  std::vector<int> vec1 = {1, 2, 3, 4};

  std::rotate(vec1.begin(), vec1.begin() + 1, vec1.end());
  std::cout << "simple rotate left:\n";
  for (int i : vec1)
    std::cout << i << " ";

  std::cout << "\n";

  std::cout << "simple rotate right:\n";
  std::rotate(vec1.rbegin(), vec1.rbegin() + 1, vec1.rend());
  for (int i : vec1)
    std::cout << i << " ";

  std::cout << "\n";

  std::vector<int>::iterator element_to_became_first_element = vec1.begin() + 2;
  std::rotate(vec1.begin(), element_to_became_first_element, vec1.end());
  for (int i : vec1)
    std::cout << i << " ";
}

void count() {
  std::vector<int> numbers = {2, 3, 1, 0, -4, 7, 5, 3};
  int larger_than_five = std::count_if(numbers.begin(), numbers.end(),
                                       [](int x) { return x > 5; });
  std::cout << "there is (are) " << larger_than_five << " number larger than 5"
            << std::endl;

  int number_three = 3;
  int how_many_three = std::count(numbers.begin(), numbers.end(), number_three);
  std::cout << "there is (are) " << how_many_three << " three(s)" << std::endl;
}

void min_max() {
  std::vector<int> numbers = {20, 30, 10, 0, -40, 7, 5, 3};
  std::vector<int>::iterator abs_max =
      std::max_element(numbers.begin(), numbers.end());
  std::cout << "the absolute max is: " << *abs_max << std::endl;

  std::vector<int>::iterator first_digit_max =
      std::max_element(numbers.begin(), numbers.end(),
                       [](int x, int y) { return (x % 10) < (y % 10); });
  std::cout << "the number with first digit max is : " << *first_digit_max
            << std::endl;

  std::pair<std::vector<int>::iterator, std::vector<int>::iterator> min_max =
      std::minmax_element(numbers.begin(), numbers.end());

  std::cout << "min is : " << *min_max.first << std::endl;
  std::cout << "max is : " << *min_max.second << std::endl;
}

void distance() {
  std::vector<int> numbers = {20, 30, 10, 0, -40, 7, 5, 3};
  int index = 2;
  std::vector<int>::iterator nth = numbers.begin() + index;
  std::cout << std::distance(numbers.begin(), nth) << std::endl;
  std::vector<int>::iterator it = numbers.begin();
  std::advance(it, index);
  std::cout << std::distance(numbers.begin(), nth) << std::endl;
}

void find() {
  // use it wehn data is not sorted
  std::vector<int> numbers = {2, 3, 1, 0, -4, 7, 5, 3};
  int number_three = 3;
  std::vector<int>::iterator find_it =
      std::find(numbers.begin(), numbers.end(), number_three);
  if (find_it != std::end(numbers)) {
    std::cout << "numbers vector contains " << number_three << '\n';
    size_t i = find_it - numbers.begin();
    std::cout << "and its index in the vector is:" << i << std::endl;

    std::ptrdiff_t pos = std::distance(
        numbers.begin(), find(numbers.begin(), numbers.end(), number_three));
    std::cout << "and its index in the vector is:" << pos << std::endl;
  } else {
    std::cout << "numbers vector does not contain " << number_three << '\n';
  }

  int number_to_find = 3;
  find_it =
      std::find_if(numbers.begin(), numbers.end(),
                   [number_to_find](int x) { return x > number_to_find; });
  std::ptrdiff_t pos = std::distance(numbers.begin(), find_it);
  std::cout << "the first element larger than 3 located at " << pos << '\n';
}

void search() {
  // search for n consecutive occurrence of numbers
  std::vector<int> numbers = {2, 3, 3, 0, -4, 7, 5, 3};
  int how_many_three = 2;
  int number_three = 3;
  std::vector<int>::iterator find_it = std::search_n(
      numbers.begin(), numbers.end(), how_many_three, number_three);
  std::cout << "the first consecutive occurrence of three number two times "
               "located at "
            << find_it - numbers.begin() << '\n';

  std::vector<int> sub_range = {0, -4, 7};
  find_it = std::search(numbers.begin(), numbers.end(), sub_range.begin(),
                        sub_range.end());
  std::cout << "the first consecutive occurrence of 0,-4,7 located at "
            << find_it - numbers.begin() << '\n';

  std::vector<int> items = {87, 5, 43};
  find_it = std::find_first_of(numbers.begin(), numbers.end(), items.begin(),
                               items.end());
  std::cout << "the first occurrence of any 87,5,43 located at "
            << find_it - numbers.begin() << '\n';
}

void adjacent_find() {
  // Searches the range [first, last) for two consecutive identical elements.
  std::vector<int> numbers = {-1, -2, 3, 4, 6, -4, 7, 5, 3};
  std::vector<int>::iterator find_it =
      std::adjacent_find(numbers.begin(), numbers.end(),
                         [](int x, int y) { return (y == x + 2); });
  std::cout << "the first occurrence n,n+1 happens at: "
            << find_it - numbers.begin() << std::endl;
}

void equal() {
  // std::equal
  std::vector<int> numbers_replicate = {-1, -2, 3, 4, 0, -4, 7, 5, 3};
  std::vector<int> numbers = {-1, -2, 3, 4, 0, -4, 7, 5, 3};
  if (std::equal(numbers.begin(), numbers.end(), numbers_replicate.begin())) {
    std::cout << "both vectors are equal " << std::endl;
  }

  numbers_replicate = {-2, -1, 3, 4, 0, -4, 7, 5, 3};
  if (std::is_permutation(numbers.begin(), numbers.end(),
                          numbers_replicate.begin())) {
    std::cout << "vectors are permutation of each other " << std::endl;
  }
}

void mismatch() {
  // std::mismatch
  std::vector<int> numbers = {-1, -2, 3, 4, 0, -4, 7, 5, 3};
  std::vector<int> numbers_changed = {-1, -2, -3, 4, 0, -4, 7, 5, 3};
  std::pair<std::vector<int>::iterator, std::vector<int>::iterator>
      mis_matches = std::mismatch(numbers.begin(), numbers.end(),
                                  numbers_changed.begin());
  std::cout << "Mismatch at " << (mis_matches.first - numbers.begin())
            << ". Because: " << *mis_matches.first
            << " != " << *mis_matches.second << std::endl;
}

void partition() {
  // std::partition
  std::vector<int> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::cout << "Original vector:\n    ";
  for (int elem : v)
    std::cout << elem << ' ';

  auto partition_it =
      std::partition(v.begin(), v.end(), [](int i) { return i % 2 == 0; });

  std::cout << "\nPartitioned vector:\n    ";
  std::copy(std::begin(v), partition_it,
            std::ostream_iterator<int>(std::cout, " "));
  std::cout << " * ";
  std::copy(partition_it, std::end(v),
            std::ostream_iterator<int>(std::cout, " "));
  std::cout << '\n';

  std::vector<int> myvector;

  // set some values:
  for (int i = 1; i < 10; ++i)
    myvector.push_back(i); // 1 2 3 4 5 6 7 8 9

  std::vector<int>::iterator bound;
  bound = std::partition(myvector.begin(), myvector.end(), IsOdd);

  // print out content:
  std::cout << "odd elements:";
  for (std::vector<int>::iterator it = myvector.begin(); it != bound; ++it)
    std::cout << ' ' << *it;
  std::cout << '\n';

  std::cout << "even elements:";
  for (std::vector<int>::iterator it = bound; it != myvector.end(); ++it)
    std::cout << ' ' << *it;
  std::cout << '\n';
}

void copy() {
  std::vector<int> vec1 = {1, 2, 3};
  std::vector<int> vec2 = {0, 0, 0, 0};
  std::vector<int> vec3 = {0, 0, 0, 0};
  std::copy(vec1.begin(), vec1.end(), vec2.begin());
  std::copy_if(vec1.begin(), vec1.end(), vec2.begin(),
               [](int x) { return x % 2 == 0; });
  std::copy_n(vec1.begin(), 2, vec2.begin());
  std::copy_backward(vec1.begin(), vec1.end(), vec2.end());
}

void move() {
  // std::move
  // std::move_backward
}

void transform() {
  /*
std::transform applies the given function to a range and stores the result in
another range, keeping the original elements order and beginning at d_first.

OutputIt transform( InputIt first1,
                  InputIt last1,
                  OutputIt d_first,
                  UnaryOperation unary_op );

first1, last1	-	the first range of elements to transform
first2	-	the beginning of the second range of elements to transform
d_first	-	the beginning of the destination range, may be equal to first1
or first2
policy	-	the execution policy to use. See execution policy for details.
unary_op	-	unary operation function object that will be applied.
The signature of the function should be equivalent to the following:


*/

  std::vector<int> vec1 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::vector<int> vec2(10, 0);
  std::vector<int> vec3(10, 0);
  std::transform(vec1.begin(), vec1.end(), vec2.begin(),
                 [](int x) { return x + 1; }); // vec2=vec1+1
  std::transform(vec1.begin(), vec1.end(), vec2.begin(), vec3.begin(),
                 [](int x, int y) { return x + y; }); // vec3=vec1+vec2
}

void swap() {
  std::vector<int> vec1 = {
      4,
      12,
      3,
      6,
  };
  std::vector<int> vec2 = {-7, -1, 2, 3, 1, 8, 9};
  std::swap(vec1, vec2);
}

// provide a swap function for a class
// https://stackoverflow.com/questions/6380862/how-to-provide-a-swap-function-for-my-class

//  is a setter returning the old value
void exchange() {
  int x, y, z;
  x = 2;
  y = 4;
  //    x is assigned the value of y,
  //    z is assigned the value that x had initially.
  z = std::exchange(x, y);
}

// The std::exchange can be used when implementing
// move assignment operators and move constructors:
struct S {
  int n;

  S(S &&other) noexcept : n{std::exchange(other.n, 0)} {}

  S &operator=(S &&other) noexcept {
    if (this != &other)
      n = std::exchange(other.n, 0); // move n, while leaving zero in other.n
    return *this;
  }
};

void fill() {
  std::vector<int> vec2(10, 0);
  std::fill(vec2.begin(), vec2.end(), 1);
  std::fill_n(vec2.begin(), 2, 0);
}

void generate() {
  // Assigns each element in range [first, last) a value generated by the given
  // function object g.
  std::random_device rd;
  std::mt19937 g(rd());

  // int upperbound=100;
  // std::srand(std::time(0)); //use current time as seed for random generator
  // //Will return an integer between [0,upperbound)
  // std::cout<<(rand() % upperbound) <<std::endl;

  std::vector<int> vec1;
  vec1.resize(5);
  std::generate(vec1.begin(), vec1.end(), g);
  printArray(vec1);
}

void replace() {
  std::vector<int> vec1 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::replace(vec1.begin(), vec1.end(), 0, 1); // replace 0 with 1 in vec1
  std::replace_if(
      vec1.begin(), vec1.end(), [](int x) { return x > 2; },
      -1); // if values are bigger 2, they will be replaced by -1
}

void remove() {
  std::vector<int> vec1 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::remove(vec1.begin(), vec1.end(), 3); // removes 3 from vec1
  std::remove_if(vec1.begin(), vec1.end(), [](int x) {
    return x % 2 == 0;
  }); // removes even number from vec1
}

void permutation() {
  // std::prev_permutation
  // std::next_permutation

  int arr[] = {1, 2, 3};
  std::sort(arr, arr + 3);

  std::cout << "The 3! possible permutations with 3 elements:\n";
  do {
    std::cout << arr[0] << " " << arr[1] << " " << arr[2] << "\n";
  } while (std::next_permutation(arr, arr + 3));

  std::cout << "After loop: " << arr[0] << ' ' << arr[1] << ' ' << arr[2]
            << '\n';
}

void lexicographical_compare() {

  // std::lexicographical_compare
  std::vector<int> number_set1, number_set2;
  number_set1 = {1, 2, 3};
  number_set2 = {1, 2, 4};
  // {1,2,4} > {1,2,3}

  // Is number_set1 less than number_set2?
  bool number_set1_less_than_number_set1 =
      std::lexicographical_compare(number_set1.begin(), number_set1.end(),
                                   number_set2.begin(), number_set2.end());

  if (number_set1_less_than_number_set1) {

    for (auto c : number_set1)
      std::cout << c << ' ';
    std::cout << "< ";
    for (auto c : number_set2)
      std::cout << c << ' ';
    std::cout << '\n';
  } else {
    for (auto c : number_set1)
      std::cout << c << ' ';
    std::cout << ">= ";
    for (auto c : number_set2)
      std::cout << c << ' ';
    std::cout << '\n';
  }

  //{1,3} > {1,2,3}
  number_set1 = {1, 2, 3};
  number_set2 = {1, 3};

  number_set1_less_than_number_set1 =
      std::lexicographical_compare(number_set1.begin(), number_set1.end(),
                                   number_set2.begin(), number_set2.end());

  if (number_set1_less_than_number_set1) {

    for (auto c : number_set1)
      std::cout << c << ' ';
    std::cout << "< ";
    for (auto c : number_set2)
      std::cout << c << ' ';
    std::cout << '\n';
  } else {
    for (auto c : number_set1)
      std::cout << c << ' ';
    std::cout << ">= ";
    for (auto c : number_set2)
      std::cout << c << ' ';
    std::cout << '\n';
  }
}

void shuffel() {
  std::vector<int> vec1 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::random_device rd;
  std::mt19937 g(rd());

  std::shuffle(vec1.begin(), vec1.end(), g);
  for (int i : vec1)
    std::cout << i << " ";
  std::cout << "\n";
}

bool cmp_sort_algorithm(int x, int y) { return x < y; }

void sort() {
  // only works on vector,deque,container array, native array

  std::random_device rd;
  std::mt19937 g(rd());

  /////////////////////normal sort///////////////////
  std::vector<int> vec1 = {0, 1, 3, 6, 9, 8, 7};
  std::sort(vec1.begin(), vec1.end(), cmp_sort_algorithm);

  for (int i : vec1)
    std::cout << i << " ";
  std::cout << "\n";

  /////////////////////is_sorted///////////////////
  std::vector<int> number_set1 = {3, 2, 1};
  bool is_number_set1_sorted_descendingly =
      std::is_sorted(number_set1.begin(), number_set1.end(),
                     [](int x, int y) { return x >= y; });
  if (is_number_set1_sorted_descendingly)
    std::cout << "number_set1 is sorted descendingly" << std::endl;

  /////////////////////is_sorted_until///////////////////
  number_set1 = {10, 20, 50, 30, 40};
  std::vector<int>::iterator the_vector_is_sorted_unitil =
      std::is_sorted_until(number_set1.begin(), number_set1.end());
  std::cout << *the_vector_is_sorted_unitil << std::endl;
  std::cout << "The array is sorted until element at: "
            << std::distance(number_set1.begin(), the_vector_is_sorted_unitil)
            << std::endl;

  std::shuffle(vec1.begin(), vec1.end(), g);

  ///////////////////partial_sort, finding top 3 students,
  /// sorted///////////////////
  std::partial_sort(number_set1.begin(), number_set1.begin() + 3,
                    number_set1.end());

  for (int i : number_set1)
    std::cout << i << " ";
  std::cout << "\n";

  /////////////////////nth_element, finding top 3 students, not necessarily
  /// sorted/////////////////////
  std::shuffle(vec1.begin(), vec1.end(), g);
  std::nth_element(number_set1.begin(), number_set1.begin() + 3,
                   number_set1.end());
  for (int i : number_set1)
    std::cout << i << " ";
  std::cout << "\n";

  ///////////////////////partion data into two groups, neither of the groups are
  /// sorted/////////////////////
  std::shuffle(number_set1.begin(), number_set1.end(), g);
  std::vector<int>::iterator iterator_to_the_first_element_of_the_second_group =
      std::partition(number_set1.begin(), number_set1.end(),
                     [](int x) { return x < 30; });

  std::cout << "Element of the first group"
            << "\n";
  for (std::vector<int>::iterator it = number_set1.begin();
       it != iterator_to_the_first_element_of_the_second_group; it++) {
    std::cout << *it << " ";
  }
  std::cout << "\n";

  std::cout << "Element of the second group"
            << "\n";
  for (std::vector<int>::iterator it =
           iterator_to_the_first_element_of_the_second_group;
       it != number_set1.end(); it++) {
    std::cout << *it << " ";
  }
  std::cout << "\n";

  /////////////////////////stable_partition,partion data into two groups, but it
  /// preserve the original order///////////////////////
  std::shuffle(number_set1.begin(), number_set1.end(), g);
  std::stable_partition(number_set1.begin(), number_set1.end(),
                        [](int x) { return x < 30; });
}

void heap() {
  /*

  Examples of Min Heap:

              10                      10
           /      \               /       \
         20        100          15         30
        /                      /  \        /  \
      30                     40    50    100   40

  Examples of Max Heap:

                                     17
                                  /       \
                                15         10
                               /  \        /
                              6    10     7

  Below table shows indexes of other nodes for the ith node, i.e., Arr[i]:
  Arr[i/2]	Returns the parent node
  Arr[(2*i)+1]	Returns the left child node
  Arr[(2*i)+2]	Returns the right child node

  1) getMini(): It returns the root element of Min Heap. Time Complexity of this
  operation is O(1).

  2) extractMin(): Removes the minimum element from Min Heap. Time Complexity of
  this Operation is O(Logn) as this operation needs to maintain the heap
  property (by calling heapify()) after removing root.

  3) decreaseKey(): Decreases value of key. Time complexity of this operation is
  O(Logn). If the decreases key value of a node is greater than parent of the
  node, then we don’t need to do anything. Otherwise, we need to traverse up to
  fix the violated heap property.

  4) insert(): Inserting a new key takes O(Logn) time.
  We add a new key at the end of the tree. IF new key is greater than its
  parent, then we don’t need to do anything. Otherwise, we need to traverse up
  to fix the violated heap property.

  5) delete(): Deleting a key also takes O(Logn) time. We replace the key to be
  deleted with minum infinite by calling decreaseKey(). After decreaseKey(), the
  minus infinite value must reach root, so we call extractMin() to remove key.




  Applications of Heaps:
  1) Heap Sort: Heap Sort uses Binary Heap to sort an array in O(nLogn) time.

  2) Priority Queue: Priority queues can be efficiently implemented using Binary
  Heap because it supports insert(), delete() and extractmax(), decreaseKey()
  operations in O(logn) time. Binomoial Heap and Fibonacci Heap are variations
  of Binary Heap. These variations perform union also efficiently.

  3) Graph Algorithms: The priority queues are especially used in Graph
  Algorithms like Dijkstra’s Shortest Path and Prim’s Minimum Spanning Tree.

  4) Many problems can be efficiently solved using Heaps. See following for
  example. a) K’th Largest Element in an array. b) Sort an almost sorted array/
  c) Merge K Sorted Arrays.

  */

  std::vector<int> v = {6, 10, 7, 17, 10, 15};

  std::cout << "initially, v: ";
  for (auto i : v)
    std::cout << i << ' ';
  std::cout << '\n';

  if (!std::is_heap(v.begin(), v.end())) {
    std::cout << "making heap...\n";
    std::make_heap(v.begin(), v.end(), std::greater_equal());
  }

  std::cout << "after make_heap, v: ";
  for (auto i : v)
    std::cout << i << ' ';
  std::cout << '\n';

  std::cout << "The maximum element of heap is : ";
  std::cout << v.front() << std::endl;

  // remove the largest element
  std::pop_heap(v.begin(), v.end());
  v.pop_back();

  // Adding a new element
  v.push_back(100);
  std::cout << (std::is_heap(v.begin(), v.end()) ? "it is a heap"
                                                 : "it is not a heap")
            << std::endl;
  std::push_heap(v.begin(), v.end());
  std::cout << (std::is_heap(v.begin(), v.end()) ? "it is a heap"
                                                 : "it is not a heap")
            << std::endl;

  // sorting a heap -> it only works on a heap, so you should first turn your
  // vetor into a heap
  std::sort_heap(v.begin(), v.end());
}

void binary_search() {
  // your data shoudl be always sorted
  std::vector<int> vec1 = {3, 6, 7, 9, 12, 5, -1};
  std::sort(vec1.begin(), vec1.end());
  int number_to_search_for = 7;
  bool number_exist =
      std::binary_search(vec1.begin(), vec1.end(), number_to_search_for);
}

void includes() {
  std::vector<int> vec1 = {3, 6, 7, 9, 12, 5, -1};
  std::sort(vec1.begin(), vec1.end());

  std::vector<int> elements_to_search = {9, 3};
  std::sort(elements_to_search.begin(), elements_to_search.end());

  bool found =
      std::includes(vec1.begin(), vec1.end(), elements_to_search.begin(),
                    elements_to_search.end());

  (found ? std::cout << "found" : std::cout << "not found") << std::endl;
}

void priority_queue() {
  // use heap for finding max/min value,
  std::priority_queue<int> q;

  for (int n : {1, 8, 5, 6, 3, 4, 0, 9, 7, 2})
    q.push(n);

  print_queue(q);

  std::priority_queue<int, std::vector<int>, std::greater<int>> q2;

  for (int n : {1, 8, 5, 6, 3, 4, 0, 9, 7, 2})
    q2.push(n);

  print_queue(q2);

  // Using lambda to compare elements.
  auto comparator = [](int left, int right) {
    return (left ^ 1) < (right ^ 1);
  };

  std::priority_queue<int, std::vector<int>, decltype(comparator)>
      q_custom_comparator(comparator);
  for (int n : {1, 8, 5, 6, 3, 4, 0, 9, 7, 2})
    q_custom_comparator.push(n);

  print_queue(q_custom_comparator);
}

void customStructPQ() {

  // Define the cell struct
  struct cell {
    int index;
    float cost;
  };

  // Define a custom comparison functor
  struct CompareCell {
    bool operator()(const cell &a, const cell &b) {
      return a.cost > b.cost; // Min-heap: smallest cost has highest priority
    }
  };

  cell c1{1, 5.0};
  cell c2{2, 3.0};
  cell c3{3, 7.0};
  cell c4{4, 4.0};

  // Initialize a vector of cells
  std::vector<cell> cells = {c1, c2, c3, c4};

  // Initialize a priority_queue with custom comparison
  std::priority_queue<cell, std::vector<cell>, CompareCell> pq_vec;

  // Initialize a priority_queue with std::list as the underlying container
  std::priority_queue<cell, std::list<cell>> pq_list;

  // Push all cells into the priority queue
  for (const auto &c : cells) {
    pq_vec.push(c);
  }

  // Display the elements in priority_queue (will be in ascending order of cost)
  while (!pq_vec.empty()) {
    cell top = pq_vec.top();
    std::cout << "Index: " << top.index << ", Cost: " << top.cost << std::endl;
    pq_vec.pop();
  }
}

void customStructOverloadedLessOperatorPQ() {
  struct cell {
    int index;
    float cost;

    bool operator<(const cell &other) const { return cost < other.cost; }
  };
  // Max-heap by default, largest `cost` is at the top
  std::priority_queue<cell> pq;
}

void lower_upper_equal_bound() {
  // find the first position where an item can be iserted while still keep the
  // sorting
  std::vector<int> vec1 = {12, 3, 4, 7, 7, 0};
  //    std::sort(vec1.begin(),vec1.end(),std::greater<int>() );
  std::sort(vec1.begin(), vec1.end(), std::less<int>());
  for (auto i : vec1)
    std::cout << i << " ";
  std::cout << "\n";
  int number_to_be_inserted = 7;
  std::vector<int>::iterator first_location_of_number_to_be_inserted,
      last_location_of_number_to_be_inserted;
  first_location_of_number_to_be_inserted =
      std::lower_bound(vec1.begin(), vec1.end(), number_to_be_inserted);
  last_location_of_number_to_be_inserted =
      std::upper_bound(vec1.begin(), vec1.end(), number_to_be_inserted);

  std::cout << "First location that " << number_to_be_inserted
            << " can be inserted" << std::endl;

  std::cout << std::distance(vec1.begin(),
                             first_location_of_number_to_be_inserted)
            << std::endl;

  std::cout << "Last location that " << number_to_be_inserted
            << " can be inserted" << std::endl;
  std::cout << std::distance(vec1.begin(),
                             last_location_of_number_to_be_inserted)
            << std::endl;

  // std::equal_range() ->return both first and last index
}

void upper_bound() {}

void advance() {
  std::vector<int> numbers = {20, 30, 10, 0, -40, 7, 5, 3};
  int index = 2;
  std::vector<int>::iterator nth = numbers.begin() + index;
  std::cout << std::distance(numbers.begin(), nth) << std::endl;
  std::vector<int>::iterator it = numbers.begin();
  std::advance(it, index);
  std::cout << std::distance(numbers.begin(), nth) << std::endl;
}

void merge() {
  std::vector<int> vec1 = {1, 2, 3, 4};
  std::vector<int> vec2 = {4, 8, 9, 12};
  std::vector<int> vec_out(10, 0);
  std::merge(vec1.begin(), vec1.end(), vec2.begin(), vec2.end(),
             vec_out.begin()); // keep duplicate items
}

void accumulateExample() {
  int array[] = {2, 3, 4};
  int sum = std::accumulate(std::begin(array), std::end(array), 0);
  std::cout << sum << std::endl;
}

/*
you should send the object by ref to mak any changes and return parameters
doesn't override the original object
*/
void for_eachExample() {
  std::vector<int> my_vec = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  // this only returns so the original object is unchanged
  std::for_each(std::begin(my_vec), std::end(my_vec),
                [](int a) { return a * 3; });
  printArray(my_vec);
  std::for_each(my_vec.begin(), my_vec.end(), [](int &a) { a++; });
  printArray(my_vec);
}

// https://stackoverflow.com/questions/28644245/string-validation-in-stdvector
// std::none_of checks if no element in a range satisfies a condition
bool validate(const std::vector<std::string> &in) {
  return std::none_of(std::begin(in), std::end(in), [](std::string const &s) {
    return s.find('c') == std::string::npos;
  });
}

void partial_sort() {
  int Kth = 4;
  std::vector<int> numbers = {64, 25, 12, 22, 11, 90};
  std::vector<int> result(Kth); // We want the 4 smallest elements

  std::partial_sort_copy(numbers.begin(), numbers.end(), result.begin(),
                         result.end());

  std::cout << "Original numbers: ";
  for (int num : numbers) {
    std::cout << num << " ";
  }
  std::cout << std::endl;

  std::cout << "Smallest" << Kth << "numbers: ";
  for (int num : result) {
    std::cout << num << " ";
  }
  std::cout << std::endl;
}
int main(int argc, char **argv) {

  // for_eachExample();
  //  accumulateExample();
  /*
      unique();
      count();
      min_max();
      distance();
      find();
      search();
      adjacent_find();
      equal();
      mismatch();
      lexicographical_compare();
      partition();
      transform();
      copy();
      move();//--> not done yet, should be done for complicated data structures
  and classes swap(); fill(); generate(); replace(); remove(); shuffel();
      unique();
      rotate();
      permutation();
      sort();
      heap();
      priority_queue();

  //    Sorted Data Algorithm, only works on sorted data
  //    1)Binary Search
  //    2)Merge
  //    3)Set Operation

      binary_search();
      includes();
      lower_upper_equal_bound();
      merge();

  //    https://www.youtube.com/watch?v=s6_meQVkwgc
  //    std::set_union()
  //    std::set_intersection();
  //    std::set_difference();
  //    std::set_symmetric_difference();

      //accumalte
      //std::accumulate
      //std::inner_product()


      //std::partial_sum()
      //std::adjacent_difference();


      */

  ////Iterator vector, deque, array

  //    //std::queue
  //    std::array<int, 3> a1{ {1, 2, 3} };
  //    std::array<int, 3>::iterator array_it;
  //    array_it++;
  //    array_it=array_it+5;
  //    //if(array_it>array2_it)

  ////1) Random Access -> Vector, Deque, Array

  //    //double-ended queue
  //    //As opposed to std::vector, the elements of a deque are not stored
  //    contiguously std::deque<int>::iterator deque_it;

  ////2) Biderectional Iterator -> List, Set, Multi set, Multimap
  //    std::list<int>::iterator list_it;
  //    list_it++;
  //    list_it--;
  //    //list_it=list_it+5; -> error

  ////3) Forward Iterator

  //    std::forward_list<int>::iterator forward_list_iterator;

  //    //std::ostream_iterator
  //    //std::back_insert_iterator
  //    //std::back_inserter

  //    //std::tie
  //    //std::begin(digits), std::end(digits)
  //    //std::align();

  //     permutation();
  // https://leetcode.com/problems/path-sum/description/
  // https://leetcode.com/problems/word-search/description/
  partial_sort();
}
