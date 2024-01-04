#include <iostream>

struct S {
  size_t m_size = 0;
  int *m_data;

  S(size_t size = 0) : m_size(size) {
    std::cout << "ctor: " << m_size << std::endl;
    m_data = new int[m_size];
  }

  ~S() { delete[] m_data; }

  // copy constructor
  S(const S &rhs) : m_size(rhs.m_size), m_data(new int[m_size]) {
    for (std::size_t i = 0; i < rhs.m_size; i++) {
      m_data[i] = rhs.m_data[i];
    }
  }

  //   Copy assignment operator
  S &operator=(S rhs) // Note: pass by value to handle self-assignment and for
                      // exception safety

  {
    std::swap(m_size, rhs.m_size);
    std::swap(m_data, rhs.m_data);
    return *this;
  }

  // Traditional Copy Assignment Operator
  S &operator=(const S &rhs) {
    std::cout << "Copy assignment operator" << std::endl;
    if (this != &rhs) { // Check for self-assignment
      delete[] m_data;  // Free existing resource

      m_size = rhs.m_size;
      m_data = new int[m_size]; // Allocate new resource
      for (std::size_t i = 0; i < m_size; i++) {
        m_data[i] = rhs.m_data[i]; // Copy the resource data
      }
    }
    return *this; // Return the current object
  }
};

int main() {}