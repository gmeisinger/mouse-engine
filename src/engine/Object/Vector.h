/**
 * @file Vector.h
 * @author George Meisinger (gmeisinger@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-08-21
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef MOUSE_VECTOR_H
#define MOUSE_VECTOR_H

#include <cmath>

namespace mouse {

// All math operators are component-wise.
struct Vector2 {
  float x, y;

  Vector2() : x(0.0f), y(0.0f) {}

  Vector2(float x, float y) : x(x), y(y) {}

  Vector2(const Vector2 &v) : x(v.x), y(v.y) {}

  float getLength() const;
  float getLengthSquare() const;

  /**
   * Normalizes the Vector.
   * @param length Desired length of the vector.
   **/
  void normalize(float length = 1.0f);

  /**
   * Gets a vector perpendicular to the Vector.
   * To get the true (normalized) normal, use v.getNormal(1.0f / v.getLength())
   **/
  Vector2 getNormal() const;

  /**
   * Gets a vector perpendicular to the Vector.
   * To get the true (normalized) normal, use v.getNormal(1.0f / v.getLength())
   **/
  Vector2 getNormal(float scale) const;

  static inline float dot(const Vector2 &a, const Vector2 &b);
  static inline float cross(const Vector2 &a, const Vector2 &b);

  Vector2 operator+(const Vector2 &v) const;
  Vector2 operator-(const Vector2 &v) const;

  Vector2 operator*(float s) const;
  Vector2 operator/(float s) const;

  Vector2 operator-() const;

  void operator+=(const Vector2 &v);
  void operator-=(const Vector2 &v);

  void operator*=(float s);
  void operator/=(float s);

  bool operator==(const Vector2 &v) const;
  bool operator!=(const Vector2 &v) const;

}; // Vector2

inline float Vector2::getLength() const { return sqrtf(x * x + y * y); }

inline float Vector2::getLengthSquare() const { return x * x + y * y; }

inline Vector2 Vector2::getNormal() const { return Vector2(-y, x); }

inline Vector2 Vector2::getNormal(float scale) const {
  return Vector2(-y * scale, x * scale);
}

inline float Vector2::dot(const Vector2 &a, const Vector2 &b) {
  return a.x * b.x + a.y * b.y;
}

inline float Vector2::cross(const Vector2 &a, const Vector2 &b) {
  return a.x * b.y - a.y * b.x;
}

inline void Vector2::normalize(float length) {
  float length_current = getLength();
  if (length_current > 0) {
    float m = length / length_current;
    x *= m;
    y *= m;
  }
}

inline Vector2 Vector2::operator+(const Vector2 &v) const {
  return Vector2(x + v.x, y + v.y);
}

inline Vector2 Vector2::operator-(const Vector2 &v) const {
  return Vector2(x - v.x, y - v.y);
}

inline Vector2 Vector2::operator*(float s) const {
  return Vector2(x * s, y * s);
}

inline Vector2 Vector2::operator/(float s) const {
  float invs = 1.0f / s;
  return Vector2(x * invs, y * invs);
}

inline Vector2 Vector2::operator-() const { return Vector2(-x, -y); }

inline void Vector2::operator+=(const Vector2 &v) {
  x += v.x;
  y += v.y;
}

inline void Vector2::operator-=(const Vector2 &v) {
  x -= v.x;
  y -= v.y;
}

inline void Vector2::operator*=(float s) {
  x *= s;
  y *= s;
}

inline void Vector2::operator/=(float s) {
  float invs = 1.0f / s;
  x *= invs;
  y *= invs;
}

inline bool Vector2::operator==(const Vector2 &v) const {
  return x == v.x && y == v.y;
}

inline bool Vector2::operator!=(const Vector2 &v) const {
  return x != v.x || y != v.y;
}

} // namespace mouse

#endif // MOUSE_VECTOR_H