#ifndef MBB_H
#define MBB_H

#include "Point.h"

template <std::floating_point T = float> class MBB {
public:
  Point<T> lowerLeft;
  Point<T> upperRight;

  MBB() : lowerLeft(Point<T>(0, 0)), upperRight(Point<T>(0, 0)) {}

  MBB(const Point<T> &_lowerLeft, const Point<T> &_upperRight)
      : lowerLeft(_lowerLeft), upperRight(_upperRight) {}

  [[nodiscard]] auto intersects(const MBB &other) const -> bool;
  [[nodiscard]] auto intersectionArea(const MBB &other) const -> NType;
  void expand(const MBB &other);
  [[nodiscard]] auto calculateExpansionCost(const MBB &other) const -> NType;
  [[nodiscard]] auto contains(const Point<T> &point) const -> bool;
  [[nodiscard]] auto perimeter() const -> NType;
  [[nodiscard]] auto area() const -> NType;
};

template <std::floating_point T = float> class QueryBox {
private:
  MBB<T> mbb;

public:
  QueryBox(const Point<T> &lowerLeft, const Point<T> &upperRight)
      : mbb(lowerLeft, upperRight) {}

  auto intersects(const MBB<T> &other) const -> bool {
    return mbb.intersects(other);
  }
  auto contains(const Point<T> &point) const -> bool {
    return mbb.contains(point);
  }
  auto getMBB() const -> const MBB<T> & { return mbb; }
};

extern template class MBB<float>;

#endif // MBB_H
