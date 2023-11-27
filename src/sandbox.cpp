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

struct ResourceHolder {

  // Constructor
  ResourceHolder(size_t size) : data(new int[size]), size(size) {
    std::cout << "ctor " << size << std::endl;
  }

  // Destructor
  ~ResourceHolder() { delete[] data; }

  // Copy Constructor
  ResourceHolder(const ResourceHolder &other)
      : data(new int[other.size]), size(other.size) {
    std::cout << "copy constructor operator " << size << std::endl;
    std::copy(other.data, other.data + size, data);
  }

  // Copy Assignment Operator
  ResourceHolder &operator=(const ResourceHolder &other) {
    if (this != &other) {
      std::cout << "copy assignment operator " << size << std::endl;
      delete[] data;
      size = other.size;
      data = new int[size];
      std::copy(other.data, other.data + size, data);
    }
    return *this;
  }

  // Move Constructor
  ResourceHolder(ResourceHolder &&other) noexcept
      : data(other.data), size(other.size) {
    std::cout << "move constructor operator " << size << std::endl;
    other.data = nullptr;
    other.size = 0;
  }

  // Move Assignment Operator
  ResourceHolder &operator=(ResourceHolder &&other) noexcept {
    if (this != &other) {
      std::cout << "move assignment operator " << size << std::endl;
      delete[] data;
      data = other.data;
      size = other.size;
      other.data = nullptr;
      other.size = 0;
    }
    return *this;
  }

  int *data;
  size_t size;
};

struct S {
  std::shared_ptr<ResourceHolder> m_resourceHolder_ptr;

  // Constructor to initialize m_resourceHolder_ptr
  S(size_t size)
      : m_resourceHolder_ptr(std::make_shared<ResourceHolder>(size)) {}

  // The default destructor, copy constructor, move constructor,
  // copy assignment operator, and move assignment operator
  // are automatically provided and are suitable for managing the shared_ptr.
};

int main(int argc, char **argv) {
  S s1(1);

  S s2(2);

  s2 = s1;

  std::cout << "s2.m_resourceHolder_ptr->size " << s2.m_resourceHolder_ptr->size
            << std::endl;
}
