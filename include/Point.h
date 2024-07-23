#ifndef POINT_H
#define POINT_H

#include "DataType.h"
#include <iostream>

template <std::floating_point T = float> class Point {
private:
  using NType = Safe<T>;

  NType x;
  NType y;

public:
  Point() : x(0), y(0) {}
  Point(NType _x, NType _y) : x(_x), y(_y) {}

  auto getX() const -> NType { return x; }
  auto getY() const -> NType { return y; }

  void setX(NType _x) { this->x = _x; }
  void setY(NType _y) { this->y = _y; }

  auto distance(const Point &p) const -> NType {
    return sqrt(pow(x - p.x, 2) + pow(y - p.y, 2));
  }

  auto operator==(const Point &p) const -> bool { return x == p.x && y == p.y; }

  auto operator!=(const Point &p) const -> bool { return !(*this == p); }

  auto operator-(const Point &p) const -> Point {
    return Point(x - p.x, y - p.y);
  }
  auto operator+(const Point &p) const -> Point {
    return Point(x + p.x, y + p.y);
  }

  // Imprimir
  friend auto operator<<(std::ostream &os, const Point &p) -> std::ostream & {
    os << "(" << p.x.getValue() << "," << p.y.getValue() << ")";
    return os;
  }
};

#endif // POINT_H
