// Double dispatch: pick a function based on the runtime types of TWO objects.
// C++ virtual dispatch only resolves one type at a time, so we chain two
// virtual calls (the visitor pattern) to resolve both.
//
// Shape::collideWith does the first dispatch (on *this).
// Inside each override we call other.collideWithCircle / collideWithRectangle,
// which is the second dispatch (on the other shape).

#include <iostream>

class Circle;
class Rectangle;

class Shape {
public:
  virtual ~Shape() = default;
  virtual void collideWith(Shape &other) = 0;
  virtual void collideWithCircle(Circle &) = 0;
  virtual void collideWithRectangle(Rectangle &) = 0;
};

class Circle : public Shape {
public:
  void collideWith(Shape &other) override { other.collideWithCircle(*this); }
  void collideWithCircle(Circle &) override {
    std::cout << "Circle vs Circle\n";
  }
  void collideWithRectangle(Rectangle &) override {
    std::cout << "Rectangle vs Circle\n";
  }
};

class Rectangle : public Shape {
public:
  void collideWith(Shape &other) override {
    other.collideWithRectangle(*this);
  }
  void collideWithCircle(Circle &) override {
    std::cout << "Circle vs Rectangle\n";
  }
  void collideWithRectangle(Rectangle &) override {
    std::cout << "Rectangle vs Rectangle\n";
  }
};

int main() {
  std::cout << "=== double dispatch demo ===\n";

  Circle c;
  Rectangle r;

  Shape *a = &c;
  Shape *b = &r;

  // Each call resolves both runtime types via two virtual lookups.
  a->collideWith(*b); // Circle vs Rectangle
  b->collideWith(*a); // Rectangle vs Circle
  a->collideWith(*a); // Circle vs Circle
  b->collideWith(*b); // Rectangle vs Rectangle
}
