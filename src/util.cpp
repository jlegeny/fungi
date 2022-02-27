#include "util.h"

#include <cmath>

namespace {
Hex3 cube_round(Vec3 floatHex) {
  int q = round(floatHex.x);
  int r = round(floatHex.y);
  int s = round(floatHex.z);

  float q_diff = fabs(q - floatHex.x);
  float r_diff = fabs(r - floatHex.y);
  float s_diff = fabs(s - floatHex.z);

  if (q_diff > r_diff && q_diff > s_diff) {
    q = -r - s;
  } else if (r_diff > s_diff) {
    r = -q - s;
  } else {
    s = -q - r;
  }
  return {q, r, s};
}

Vec2i cube_to_axial(Hex3 hex) {
  return {hex.q, hex.r};
}

Hex3 axial_to_cube(Vec2i hex) {
  return {hex.x, hex.y, -hex.x - hex.y};
}

Vec3 axial_to_cube_float(Vec2 hex) {
  return {hex.x, hex.y, -hex.x - hex.y};
}
}  // namespace

float sqdist(float ax, float ay, float bx, float by) {
  return (ax - bx) * (ax - bx) + (ay - by) * (ay - by);
}

float dist(Vec2 a, Vec2 b) {
  return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

Vec2 operator+(const Vec2& lhs, const Vec2& rhs) {
  return {lhs.x + rhs.x, lhs.y + rhs.y};
}

Vec2 operator-(const Vec2& lhs, const Vec2& rhs) {
  return {lhs.x - rhs.x, lhs.y - rhs.y};
}

Vec2i operator-(const Vec2i& lhs, const Vec2i& rhs) {
  return {lhs.x - rhs.x, lhs.y - rhs.y};
}

Vec2 operator/(const Vec2& lhs, const float rhs) {
  return {lhs.x / rhs, lhs.y / rhs};
}

Vec2i operator/(const Vec2i& lhs, const int rhs) {
  return {lhs.x / rhs, lhs.y / rhs};
}

Vec2i& operator/=(Vec2i& lhs, const int rhs) {
  lhs.x /= rhs;
  lhs.y /= rhs;
  return lhs;
}

bool operator==(const Vec2& lhs, const Vec2& rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool operator==(const Hex3& lhs, const Hex3& rhs) {
  return lhs.q == rhs.q && lhs.r == rhs.r;
}

Vec2 hex2point(const Hex3 hex, const float size) {
  return {.x = size * (3.f / 2.f * hex.q),
          .y = size * (sqrtf(3.f) / 2.f * hex.q + sqrtf(3.f) * hex.r)};
}

Hex3 point2hex(Vec2i point, const float size) {
  float q = (2. / 3. * point.x) / size;
  float r = (-1. / 3. * point.x + sqrtf(3.) / 3. * point.y) / size;

  Vec2 axialFloat = {q, r};

  return cube_round(axial_to_cube_float(axialFloat));
}

Vec2i vec2i(const Vec2 vec) {
  return {static_cast<int>(vec.x), static_cast<int>(vec.y)};
}
