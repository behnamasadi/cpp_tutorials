#include <iostream>

namespace non_virtual_des {

class Animal {
public:
  Animal() { std::cout << "con Animal" << std::endl; }

  ~Animal() { std::cout << "des Animal" << std::endl; }
};

class Mamal : public Animal {
public:
  Mamal() { std::cout << "con Mamal" << std::endl; }

  ~Mamal() { std::cout << "des Mamal" << std::endl; }
};

} // namespace non_virtual_des

namespace virtual_des {
class Animal {
public:
  Animal() { std::cout << "con Animal" << std::endl; }

  virtual ~Animal() { std::cout << "des Animal" << std::endl; }
};

class Mamal : public Animal {
public:
  Mamal() { std::cout << "con Mamal" << std::endl; }

  virtual ~Mamal() { std::cout << "des Mamal" << std::endl; }
};
} // namespace virtual_des

int main() {
  {
    non_virtual_des::Animal *obj = new non_virtual_des::Mamal;
    delete obj;
  }

  {
    virtual_des::Animal *obj = new virtual_des::Mamal;
    delete obj;
  }
}
