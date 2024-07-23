#ifndef INCLUDE_DATATYPE_CPP_
#define INCLUDE_DATATYPE_CPP_

#include <cmath>
#include <stdexcept>

template <std::floating_point T> class Safe {

private:
  T value;

public:
  Safe() : value(static_cast<T>(0)) {}
  // NOLINTNEXTLINE (hicpp-explicit-constructor)
  Safe(T _value) : value(_value) {}

  // Comparison operators using <=> and == defaulting
  auto operator==(const Safe &other) const -> bool {
    return std::abs(value - other.value) < std::numeric_limits<T>::epsilon();
  }

  auto operator<=>(const Safe &other) const {
    if (std::abs(value - other.value) < std::numeric_limits<T>::epsilon()) {
      return std::strong_ordering::equal;
    }
    if (value < other.value) {
      return std::strong_ordering::less;
    }
    return std::strong_ordering::greater;
  }

  auto operator==(const T &scalar) const -> bool {
    return std::abs(value - scalar) < std::numeric_limits<T>::epsilon();
  }

  bool operator!=(const T &scalar) const { return !(*this == scalar); }

  // Arithmetic operators
  Safe operator+(const Safe &other) const { return Safe(value + other.value); }
  Safe operator-(const Safe &other) const { return Safe(value - other.value); }
  Safe operator*(const Safe &other) const { return Safe(value * other.value); }
  Safe operator/(const Safe &other) const {
    if (std::abs(other.value) < std::numeric_limits<T>::epsilon()) {
      throw std::runtime_error("Division by zero");
    }
    return Safe(value / other.value);
  }
  Safe operator-() const { return Safe(-value); }
  Safe &operator+=(const Safe &other) {
    value += other.value;
    return *this;
  }
  Safe &operator-=(const Safe &other) {
    value -= other.value;
    return *this;
  }

  // Methods for direct value manipulation
  T getValue() const { return value; }
  void setValue(T _value) { value = _value; }

  // Mathematical functions
  static Safe abs(const Safe &other) { return Safe(std::abs(other.value)); }
  static Safe sqrt(const Safe &other) {
    if (other.value < 0) {
      throw std::runtime_error(
          "Attempt to calculate the square root of a negative number");
    }
    return Safe(std::sqrt(other.value));
  }
  static Safe pow(const Safe &base, int exponent) {
    return Safe(static_cast<T>(std::pow(base.value, exponent)));
  }
  static Safe min(const Safe &a, const Safe &b) { return a < b ? a : b; }
  static Safe max(const Safe &a, const Safe &b) { return a > b ? a : b; }

  // Print
  friend std::ostream &operator<<(std::ostream &os, const Safe &other) {
    os << other.value;
    return os;
  }
};

template <std::floating_point T> Safe<T> abs(const Safe<T> &x) {
  return Safe<T>::abs(x);
}
template <std::floating_point T> Safe<T> sqrt(const Safe<T> &x) {
  return Safe<T>::sqrt(x);
}
template <std::floating_point T>
Safe<T> pow(const Safe<T> &base, int exponent) {
  return Safe<T>::pow(base, exponent);
}
template <std::floating_point T>
Safe<T> min(const Safe<T> &a, const Safe<T> &b) {
  return Safe<T>::min(a, b);
}
template <std::floating_point T>
Safe<T> max(const Safe<T> &a, const Safe<T> &b) {
  return Safe<T>::max(a, b);
}

using NType = Safe<float>;

#endif // INCLUDE_DATATYPE_CPP_
