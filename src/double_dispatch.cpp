/*
In most object-oriented systems, the concrete function that is called from a
function call in the code depends on the dynamic type of a single object and
therefore they are known as single dispatch calls, or simply virtual function
calls.
*/
#include <iostream>

namespace SingleDispatch {
class SpaceShip {};

class ApolloSpaceShip : public SpaceShip {};

class Astroid {
public:
  void virtual collidingWith(SpaceShip &) {
    std::cout << "Astroid Colliding With SpaceShip" << std::endl;
  }
  void virtual collidingWith(ApolloSpaceShip &) {
    std::cout << "Astroid Colliding With ApolloSpaceShip" << std::endl;
  }
};

class ExplodingAstroid : public Astroid {
public:
  void collidingWith(SpaceShip &) override {
    std::cout << "ExplodingAstroid Colliding With SpaceShip" << std::endl;
  }
  void collidingWith(ApolloSpaceShip &) override {
    std::cout << "ExplodingAstroid Colliding With ApolloSpaceShip" << std::endl;
  }
};

} // namespace SingleDispatch

namespace DoubleDispatch {

class SpaceShip;
class ApolloSpaceShip;
class Astroid {
public:
  void virtual collidingWith(SpaceShip &);

  void virtual collidingWith(ApolloSpaceShip &);
};

class ApolloSpaceShip;

class ExplodingAstroid : public Astroid {
public:
  void collidingWith(SpaceShip &) override;

  void collidingWith(ApolloSpaceShip &) override;
};

class SpaceShip {
public:
  void virtual collidingWith(Astroid *astroid_ptr) {
    astroid_ptr->collidingWith(*this);
  }
};

class ApolloSpaceShip : public SpaceShip {
public:
  void virtual collidingWith(Astroid *astroid_ptr) {
    astroid_ptr->collidingWith(*this);
  }
};

void Astroid::collidingWith(SpaceShip &) {
  std::cout << "Astroid Colliding With SpaceShip" << std::endl;
}

void Astroid::collidingWith(ApolloSpaceShip &) {
  std::cout << "Astroid Colliding With ApolloSpaceShip" << std::endl;
}

void ExplodingAstroid::collidingWith(SpaceShip &) {
  std::cout << "ExplodingAstroid Colliding With SpaceShip" << std::endl;
}

void ExplodingAstroid::collidingWith(ApolloSpaceShip &) {
  std::cout << "ExplodingAstroid Colliding With ApolloSpaceShip" << std::endl;
}

} // namespace DoubleDispatch

int main() {
  {
    using namespace SingleDispatch;
    ApolloSpaceShip apolloSpaceShip;
    SpaceShip spaceShip;
    Astroid astroid;
    ExplodingAstroid explodingAstroid;

    std::cout << "The output is correct, function overloading, without using "
                 "any dynamic dispatch"
              << std::endl;
    astroid.collidingWith(apolloSpaceShip);
    astroid.collidingWith(spaceShip);

    explodingAstroid.collidingWith(apolloSpaceShip);
    explodingAstroid.collidingWith(spaceShip);
    std::cout << "------------------------------------------------------"
              << std::endl;
  }

  {
    using namespace SingleDispatch;
    SpaceShip *apolloSpaceShip_ptr = new ApolloSpaceShip;
    SpaceShip *spaceShip_ptr = new SpaceShip;
    Astroid astroid;
    ExplodingAstroid explodingAstroid;

    std::cout << "The output is wrong, while virtual functions are dispatched "
                 "dynamically in C++, function overloading is done statically."
              << std::endl;
    astroid.collidingWith(*apolloSpaceShip_ptr);
    astroid.collidingWith(*spaceShip_ptr);
    explodingAstroid.collidingWith(*apolloSpaceShip_ptr);
    explodingAstroid.collidingWith(*spaceShip_ptr);
    std::cout << "------------------------------------------------------"
              << std::endl;
  }

  {
    /*
    1) In apolloSpaceShip_ptr, C++ looks up the correct method in the vtable. In
    this case, it will call ApolloSpaceShip::collidingWith(Asteroid*).

    2) ApolloSpaceShip::collidingWith(Asteroid*),
    astroid_ptr->collidingWith(*this); will result in another vtable lookup.
    */

    using namespace DoubleDispatch;

    SpaceShip *apolloSpaceShip_ptr = new ApolloSpaceShip;
    SpaceShip *spaceShip_ptr = new SpaceShip;
    Astroid *astroid_ptr = new Astroid;
    Astroid *explodingAstroid_ptr = new ExplodingAstroid;

    spaceShip_ptr->collidingWith(astroid_ptr);
    spaceShip_ptr->collidingWith(explodingAstroid_ptr);

    apolloSpaceShip_ptr->collidingWith(astroid_ptr);
    apolloSpaceShip_ptr->collidingWith(explodingAstroid_ptr);
  }
}
