#include <iostream>
#include <memory>
#include <string>

/*
A class that declares or inherits a virtual function is called a polymorphic
class. Because dynamic_cast is a run-time cast, the classes must be polymorphic
in order for the cast to be legal.



The casts execute at runtime, and work by querying the object, asking it if it
the type we're looking for. If it is, dynamic_cast<Type*> returns a pointer;
otherwise it returns NULL.

In order for this base-to-derived casting to work using dynamic_cast<>, Base,
Derived and anOtherClass must be what the Standard calls polymorphic types. In
order to be a polymorphic type, your class must have at least one virtual
function. If your classes are not polymorphic types, the base-to-derived use of
dynamic_cast will not compile.


*/

namespace dynamiccast {
class Base {
  virtual void foo() {}

public:
  void whoAmI() { std::cout << "I'm Base" << std::endl; }
};
class Derived : public Base {
public:
  void whoAmI() { std::cout << "I'm Derived" << std::endl; }
};
class anOtherClass : public Base {
public:
  void whoAmI() { std::cout << "I'm anOtherClass" << std::endl; }
};
} // namespace dynamiccast

namespace staticcast {
class Base {
public:
  void whoAmI() { std::cout << "I'm Base" << std::endl; }
};
class Derived : public Base {
public:
  void whoAmI() { std::cout << "I'm Derived" << std::endl; }
};
class anOtherClass : public Base {};
} // namespace staticcast

void staticcastD2B() {
  staticcast::Derived *d = new staticcast::Derived;
  d->whoAmI();

  // this works
  staticcast::Base *b = static_cast<staticcast::Base *>(d);
  b->whoAmI();

  // this won't compile
  // static::anOtherClass* x = static_cast<static::anOtherClass*>(d);
}

void dynamiccastD2B() {
  dynamiccast::Derived *d = new dynamiccast::Derived;
  d->whoAmI();
  // this works
  dynamiccast::Base *b = dynamic_cast<dynamiccast::Base *>(d);
  b->whoAmI();
  // this will compile but the result is nullptr
  dynamiccast::anOtherClass *x = dynamic_cast<dynamiccast::anOtherClass *>(d);

  std::cout << (x == nullptr ? "x is nullptr" : "x has been casted to Derived")
            << std::endl;
}

dynamiccast::Base *CreateRandom() {
  if ((rand() % 2) == 0)
    return new dynamiccast::Derived;
  else
    return new dynamiccast::anOtherClass;
}

void dynamiccastB2D() {
  for (int n = 0; n < 10; ++n) {
    dynamiccast::Base *base = CreateRandom();

    dynamiccast::anOtherClass *anOtherClassObject =
        dynamic_cast<dynamiccast::anOtherClass *>(base);
    dynamiccast::Derived *derivedObject =
        dynamic_cast<dynamiccast::Derived *>(base);

    std::cout << "==================n is:" << n
              << "=================" << std::endl;

    std::cout << (anOtherClassObject == nullptr
                      ? "anOtherClassObject is nullptr"
                      : "base has been casted to anOtherClassObject")
              << std::endl;

    std::cout << (derivedObject == nullptr
                      ? "derivedObject is nullptr"
                      : "base has been casted to derivedObject")
              << std::endl;

    delete base;
  }
}

struct Base {
  int a;
  virtual void whoAmI() const { std::cout << "I am base!\n"; }
  virtual ~Base() {}
};

struct Derived : Base {
  void whoAmI() const override { std::cout << "I am derived!\n"; }
  ~Derived() {}
};

void smartPointerBaseToDerive() {
  std::shared_ptr<Base> ptr_to_base(std::make_shared<Derived>());

  // ptr_to_base->whoAmI(); // Error won't compile: Base has no member named
  // 'whoAmI'

  std::static_pointer_cast<Derived>(ptr_to_base)->whoAmI(); // OK
  // (constructs a temporary shared_ptr, then calls operator->)

  static_cast<Derived *>(ptr_to_base.get())->whoAmI(); // also OK
  // (direct cast, does not construct a temporary shared_ptr)
}

int main() {
  // staticcastD2B();
  // dynamiccastD2B();
  dynamiccastB2D();
}
