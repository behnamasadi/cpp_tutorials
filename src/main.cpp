#include <cmath>
#include <exception>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <queue>
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
}
