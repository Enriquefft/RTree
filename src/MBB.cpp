#include "MBB.h"

template <std::floating_point T>
auto MBB<T>::intersects(const MBB &other) const -> bool {
  return !(lowerLeft.getX() > other.upperRight.getX() ||
           upperRight.getX() < other.lowerLeft.getX() ||
           lowerLeft.getY() > other.upperRight.getY() ||
           upperRight.getY() < other.lowerLeft.getY());
}

template <std::floating_point T>
auto MBB<T>::intersectionArea(const MBB &other) const -> NType {
  if (!intersects(other)) {
    return {0};
  }
  NType x_overlap = std::min(upperRight.getX(), other.upperRight.getX()) -
                    std::max(lowerLeft.getX(), other.lowerLeft.getX());
  NType y_overlap = std::min(upperRight.getY(), other.upperRight.getY()) -
                    std::max(lowerLeft.getY(), other.lowerLeft.getY());
  return x_overlap * y_overlap;
}

template <std::floating_point T> void MBB<T>::expand(const MBB &other) {
  lowerLeft.setX(std::min(lowerLeft.getX(), other.lowerLeft.getX()));
  lowerLeft.setY(std::min(lowerLeft.getY(), other.lowerLeft.getY()));
  upperRight.setX(std::max(upperRight.getX(), other.upperRight.getX()));
  upperRight.setY(std::max(upperRight.getY(), other.upperRight.getY()));
}

template <std::floating_point T>
auto MBB<T>::calculateExpansionCost(const MBB &other) const -> NType {
  MBB expanded = *this;
  expanded.expand(other);
  return expanded.area() - this->area();
}

template <std::floating_point T>
auto MBB<T>::contains(const Point<T> &point) const -> bool {
  return (
      point.getX() >= lowerLeft.getX() && point.getX() <= upperRight.getX() &&
      point.getY() >= lowerLeft.getY() && point.getY() <= upperRight.getY());
}

template <std::floating_point T> auto MBB<T>::perimeter() const -> NType {
  return Safe<T>{2} * ((upperRight.getX() - lowerLeft.getX()) +
                       (upperRight.getY() - lowerLeft.getY()));
}

template <std::floating_point T> auto MBB<T>::area() const -> NType {
  return (upperRight.getX() - lowerLeft.getX()) *
         (upperRight.getY() - lowerLeft.getY());
}

template class MBB<float>;
