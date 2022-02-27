#ifndef UTIL_H
#define UTIL_H

#include "data.h"

float sqdist(float ax, float ay, float bx, float by);
float dist(Vec2 a, Vec2 b);
Vec2 operator+(const Vec2& lhs, const Vec2& rhs);
Vec2 operator-(const Vec2& lhs, const Vec2& rhs);
Vec2i operator-(const Vec2i& lhs, const Vec2i& rhs);
Vec2 operator/(const Vec2& lhs, const float rhs);
Vec2i operator/(const Vec2i& lhs, const int rhs);
Vec2i& operator/=(Vec2i& lhs, const int rhs);
bool operator==(const Vec2& lhs, const Vec2& rhs);
bool operator==(const Hex3& lhs, const Hex3& rhs);

Vec2 hex2point(const Hex3 hex, const float size);
Hex3 point2hex(Vec2i point, const float size);

Vec2i vec2i(const Vec2 vec);

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

#endif  // UTIL_H
