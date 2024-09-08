#include <cmath>
#include <exception>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <queue>
#include <set>
#include <stack>

int sub(int a, int b) { return a - b; }

void inc(int a, int &b, const int &c) {
  std::cout << "In function: " << a << ' ' << b << ' ' << c << '\n';

  a++;
  b++;
  return;
}

template <typename T> struct Node {
  Node *l_child = nullptr;
  Node *r_child = nullptr;
  T val;
};
// dfs
template <typename T> void recursive_dfs(Node<T> *node) {
  // pre order
  std::cout << node->val << std::endl;
  if (node->l_child != nullptr)
    recursive_dfs(node->l_child);
  // mid order
  // std::cout << node->val << std::endl;

  if (node->r_child != nullptr)
    recursive_dfs(node->r_child);
  // post order
  // std::cout << node->val << std::endl;
}

template <typename T> void bfs(Node<T> *node) {
  std::queue<Node<T> *> q;
  q.push(node);
  while (!q.empty()) {
    auto node = q.front();
    q.pop();

    std::cout << node->val << std::endl;
    if (node->l_child != nullptr)
      q.push(node->l_child);

    if (node->r_child != nullptr)
      q.push(node->r_child);
  }
}

template <typename T> void dfs(Node<T> *node) {
  std::stack<Node<T> *> stack;
  stack.push(node);
  while (!stack.empty()) {
    auto node = stack.top();
    stack.pop();

    std::cout << node->val << std::endl;

    if (node->r_child != nullptr)
      stack.push(node->r_child);

    if (node->l_child != nullptr)
      stack.push(node->l_child);
  }
}

void tree() {
  Node<std::string> *a = new Node<std::string>;
  Node<std::string> *b = new Node<std::string>;
  Node<std::string> *c = new Node<std::string>;
  Node<std::string> *d = new Node<std::string>;
  Node<std::string> *e = new Node<std::string>;
  Node<std::string> *f = new Node<std::string>;
  Node<std::string> *g = new Node<std::string>;
  Node<std::string> *h = new Node<std::string>;
  Node<std::string> *i = new Node<std::string>;
  Node<std::string> *j = new Node<std::string>;

  a->val = "a";
  a->l_child = b;
  a->r_child = c;

  b->val = "b";
  b->l_child = d;
  b->r_child = e;

  c->val = "c";
  c->l_child = f;
  c->r_child = g;

  d->val = "d";
  d->l_child = h;
  d->r_child = i;

  e->val = "e";
  e->l_child = j;

  h->val = "h";
  i->val = "i";
  f->val = "f";
  g->val = "g";
  j->val = "j";
  std::cout << "recursive_dfs" << std::endl;
  recursive_dfs(a);

  std::cout << "dfs" << std::endl;
  dfs(a);

  std::cout << "bfs" << std::endl;

  bfs(a);
}

struct Person {

  std::string name;
  int age;
};

bool operator<(const Person &lhs, const Person &rhs) {
  return lhs.age < rhs.age;
}

bool operator>(const Person &lhs, const Person &rhs) {
  return lhs.age > rhs.age;
}
template <typename T> void printArray(T array) {
  for (const auto &element : array)
    std::cout << element << " ";
}

class Solution {
public:
  int removeDuplicates(std::vector<int> &nums) {
    std::unique(nums.begin(), nums.end());

    printArray(nums);
    std::cout << '\n';
    std::cout << nums.size() << std::endl;
    return nums.size();
  }
};

struct cell {
  int index;
  float cost;
  // bool operator<(const cell &other) { return cost < other.cost; }
};

bool operator<(const cell &lhs, const cell &rhs) { return lhs.cost < rhs.cost; }

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

  /*


                      a
                    /   \
                  b       c
                /  \    /   \
               d    e  f     g
              / \  /
             h   i j

  */
  // std::cmp_equal

  // std::vector<int> numbers = {3, 7, 4, 9, 2, 5, 8, 1, 6};
  // std::sort(numbers.begin(), numbers.end(), std::less<int>());

  std::vector<Person> people = {{"Alice", 30}, {"Bob", 25}, {"Charlie", 35}};

  std::sort(people.begin(), people.end(), std::less<Person>());

  for (const auto &person : people) {
    std::cout << person.name << ": " << person.age << std::endl;
  }

  std::vector<int> nums = {0, 0, 1, 1, 1, 2, 2, 3, 3, 4};
  Solution s;
  s.removeDuplicates(nums);

  // std::transform()

  cell c1{1, 5};
  cell c2{2, 3};
  cell c3{3, 7};
  cell c4{4, 4};
  // std::boolalpha <<
  std::cout << std::boolalpha << (c1 < c2) << std::endl;

  std::vector<cell> cells = {c1, c2, c3, c4};
  std::priority_queue<cell> pq;

  for (const auto &c : cells) {
    pq.push(c);
  }

  // Displaying the elements in priority_queue (will be in descending order of
  // cost)
  while (!pq.empty()) {
    cell top = pq.top();
    std::cout << "Index: " << top.index << ", Cost: " << top.cost << std::endl;
    pq.pop();
  }

  // std::vector<int> numbers = {13, 7, 41, 9, 2, 15, 8, 1, 6};
  // printArray(numbers);

  // std::cout << "\n";

  // // Find the median
  // auto middle = numbers.begin() + numbers.size() / 2;
  // std::nth_element(numbers.begin(), middle, numbers.end(),
  //                  std::greater_equal());

  // std::cout << "\n";

  // std::cout << "Median: " << *middle << std::endl;
  // printArray(numbers);
  // std::cout << "\n";

  std::vector<int> v = {6, 10, 7, 17, 10, 15};

  std::cout << "initially, v: ";
  for (auto i : v)
    std::cout << i << ' ';
  std::cout << '\n';

  if (!std::is_heap(v.begin(), v.end())) {
    std::cout << "making min heap...\n";
    std::make_heap(v.begin(), v.end(), std::greater_equal());
  }

  std::cout << "the min heap: ";
  for (auto i : v)
    std::cout << i << ' ';
  std::cout << '\n';
  std::cout << "The minimum element of heap is: ";
  std::cout << v.front() << std::endl;

  // std::make_heap(v.begin(), v.end(), std::less_equal());
  std::make_heap(v.begin(), v.end());

  std::cout << "the max heap: ";
  for (auto i : v)
    std::cout << i << ' ';
  std::cout << '\n';

  std::cout << "The maximum element of heap is: ";
  std::cout << v.front() << std::endl;

  // remove the largest element
  std::pop_heap(v.begin(), v.end());
  std::cout << "the max is at the bottom: ";
  for (auto i : v)
    std::cout << i << ' ';
  std::cout << '\n';
  v.pop_back();
  std::cout << "the max element has been removed: ";
  for (auto i : v)
    std::cout << i << ' ';
  std::cout << '\n';

  // Adding a new element
  v.push_back(100);
  std::cout << (std::is_heap(v.begin(), v.end()) ? "it is a heap"
                                                 : "it is not a heap")
            << std::endl;
  std::push_heap(v.begin(), v.end());
  std::cout << (std::is_heap(v.begin(), v.end()) ? "it is a heap"
                                                 : "it is not a heap")
            << std::endl;

  std::set<std::string> usernames;
  usernames.insert("info");
  usernames.insert("alice");
  usernames.insert("bob");

  for (const auto &user : usernames)
    std::cout << user << std::endl;

  // if (usernames.find("who") == usernames.end())

  unsigned char c = 'A';
  char lower_c = static_cast<char>(std::tolower(c));

  std::string a(1, lower_c); // Create a string with one character

  std::cout << std::boolalpha << static_cast<bool>(std::isalnum(c)) << '\n';

  return 0;
}
