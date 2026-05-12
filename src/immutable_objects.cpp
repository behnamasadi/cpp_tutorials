// Immutable objects in C++.
//
// Pattern:
//   - const data members
//   - all state set in the constructor initializer list
//   - getters are const-qualified
//   - assignment operator is deleted
//   - no setters; "mutation" returns a new object instead

#include <iostream>
#include <string>

class ImmutablePoint {
private:
    const int x;
    const int y;

public:
    ImmutablePoint(int x_val, int y_val) : x(x_val), y(y_val) {}

    int getX() const { return x; }
    int getY() const { return y; }

    // "Mutation" returns a new immutable copy instead of changing this one.
    ImmutablePoint movedBy(int dx, int dy) const {
        return ImmutablePoint(x + dx, y + dy);
    }

    // Disallow assignment so an existing instance can't be overwritten.
    ImmutablePoint& operator=(const ImmutablePoint&) = delete;
};

int main() {
    ImmutablePoint p(10, 20);
    std::cout << "[original]   x=" << p.getX() << " y=" << p.getY() << '\n';

    // "Move" the point: original is untouched, we get a new object.
    ImmutablePoint q = p.movedBy(3, 4);
    std::cout << "[movedBy 3,4] x=" << q.getX() << " y=" << q.getY() << '\n';
    std::cout << "[original still] x=" << p.getX() << " y=" << p.getY() << '\n';

    // The following lines would not compile:
    //   p = q;             // operator= is deleted
    //   p.x = 99;          // x is const
    return 0;
}
