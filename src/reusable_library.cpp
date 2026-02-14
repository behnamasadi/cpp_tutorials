#include <cmath>

template <typename T> struct Vec2 {

  T x;
  T y;
  Vec2(T x, T y) : x(x), y(y) {}

  Vec2<T> operator+(const Vec2<T> &rhs) {
    return Vec2<T>(rhs.x + x, rhs.y + y);
  }

  T dot(const Vec2<T> &rhs) { return (rhs.x * x + rhs.y * y); }

  T norm() { return pow(x * x + y * y, 0.5); }
};

// dot, cross (3D), norm, normalize, distance, angle, transform, compose

template <typename T> struct Vec3 {

  T x;
  T y;
  T z;
  Vec3(T x, T y) : x(x), y(y), z(z) {}
};

template <typename T> struct Rot2 {
  T m[2][2];

  Rot2() : m{{T(1), T(0)}, {T(0), T(1)}} {}

  Rot2(T m00, T m01, T m10, T m11) : m{{m00, m01}, {m10, m11}} {}

  Rot2<T> operator*(const Rot2<T> &rhs) const {
    return Rot2<T>(m[0][0] * rhs.m[0][0] + m[0][1] * rhs.m[1][0],
                   m[0][0] * rhs.m[0][1] + m[0][1] * rhs.m[1][1],
                   m[1][0] * rhs.m[0][0] + m[1][1] * rhs.m[1][0],
                   m[1][0] * rhs.m[0][1] + m[1][1] * rhs.m[1][1]);
  }

  const Rot2<T> transpose() {
    return Rot2<T>(m[0][0], m[1][0], m[0][1], m[1][1]);
  }

  const Rot2<T> inverse() {
    return Rot2<T>(m[0][0], m[1][0], m[0][1], m[1][1]);
  }

  const Vec2<T> apply(const Vec2<T> &vec) {

    return Vec2<T>(m[0][0] * vec[0] + m[0][1] * vec[1],
                   m[1][0] * vec[0] + m[1][1] * vec[1]);
  }
};

template <typename T> struct Rot3 {};

template <typename T> struct SE2 {
  Rot2<T> rotation;
  Vec2<T> translation;

  SE2(Rot2<T> rotation, Vec2<T> translation)
      : rotation(rotation), translation(translation) {}

  SE2 inverse() {}

  SE2 operator*(const SE2<T> &rhs) { return SE2<T>(); }
};

template <typename T> struct SE3 {};
template <typename T> struct Point2 {

  // Point + Vec -> Point
  // Point - Point -> Vec
  // Point - Vec -> Point
};
template <typename T> struct Point3 {};

struct shape {

  virtual ~shape() = default;
  virtual double area() const = 0;
};

struct rectangle : public shape {
  /* data */
};

// Circle, AABB, OBB, Polygon, Mesh, Capsule, Box Polygon..

/*

Where to use templates vs inheritance in this library
Use templates for:

    dimension: 2D vs 3D
    scalar type: float/double
    generic algorithms: dot, norm, transform, etc.

Use inheritance for:

    heterogeneous collections of conceptually different things:
    shapes
    constraints
    sensors
    optimization residuals
    collision objects

*/

int main() {}

// double matrix2x2[2][2] = {0, 0, 0, 0};