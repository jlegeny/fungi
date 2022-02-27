#ifndef DATA_H
#define DATA_H

#include <deque>
#include <vector>

enum class Texture {
  INVALID,
  CURSOR,
  BACKGROUND,
  TILE_OUTLINE,
  TILE_GRASS_01,
  TILE_LUSH_GRASS_01,
  TILE_CONTROL_01,
  TILE_CONTROL_02,
  TILE_CONTROL_03,
  TILE_CONTROL_04,
  OBJECT_SHROOM_01,
  OBJECT_SHROOM_02,
  OBJECT_SHROOM_03,
  OBJECT_SPORES_01,
  OBJECT_SPORES_02,
  OBJECT_SPORES_03,
  CARD_RAIN_M,
  CARD_SPORES_M,
  CARD_WIND_M,
};

struct Rect {
  float x, y;
  float w, h;
};

struct Vec2 {
  float x, y;
};

struct Vec2i {
  int x, y;
};

struct Vec3 {
  float x, y, z;
};

struct Hex3 {
  int q, r, s;
};

struct Quad {
  Vec3 a, b, c, d;
};

constexpr Vec2 GRID_ORIGIN = {.x = 20, .y = -40};
constexpr Vec2 DECK_ORIGIN = {.x = 300, .y = 20};
constexpr int HEX_SIZE = 15;

#endif  // DATA_H
