#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct S {
  int m_id;
  S(int id) {
    m_id = id;
    std::cout << "ctor " << m_id << std::endl;
  }

  ~S() { std::cout << "dtor " << m_id << std::endl; }

  S(S &&rhs) { std::cout << "move ctor " << m_id << std::endl; }

  S &operator=(S &&rhs) {
    std::cout << "move assignment operator " << m_id << std::endl;
    return *this;
  }
};

template <typename T> void printArray(T array) {
  for (const auto &element : array)
    std::cout << element << std::endl;
}

std::unique_ptr<S> createResource() {
  // Dynamically allocate the resource
  std::unique_ptr<S> ptr = std::make_unique<S>(10);
  return ptr; // Safely return the unique_ptr
}

void takeOwnership(std::unique_ptr<S> ptr) {
  // Now owns the resource
}

void processShared(std::shared_ptr<S> s_ptr) {
  // Do something with ptr
  // Reference count is maintained automatically

  std::cout << "Reference count: " << s_ptr.use_count() << std::endl;
}

std::shared_ptr<S> createSharedResource() {

  std::shared_ptr<S> s_ptr = std::make_shared<S>(100);

  std::cout << "Reference count: " << s_ptr.use_count() << std::endl;

  return s_ptr;
}

std::shared_ptr<S> badIdea() {
  S local(10);
  return std::shared_ptr<S>(&local); // Very bad: points to a local variable
}

int main(int argc, char **argv) {
  // std::unique_ptr<S> s_ptr = std::make_unique<S>(1);
  // S s;

  // std::vector<S> vec_s;

  // vec_s.push_back(S(2));
  // vec_s.push_back(S(3));

  // std::unique_ptr<S> s_ptr = createResource();
  // takeOwnership(std::move(s_ptr));
  // std::cout << "----- " << std::endl;

  // std::shared_ptr<S> s_ptr = std::make_shared<S>(10);

  // std::cout << "Reference count: " << s_ptr.use_count() << std::endl;
  // processShared(s_ptr);
  // std::cout << "Reference count: " << s_ptr.use_count() << std::endl;

  // auto sharedPtr = createSharedResource();
  // std::cout << "Reference count: " << sharedPtr.use_count() << std::endl;

  auto sharedPtr = badIdea();
  std::cout << "sharedPtr->m_id: " << sharedPtr->m_id << std::endl;
}
