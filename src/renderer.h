#ifndef RENDERER_H
#define RENDERER_H

#include <allegro5/allegro_font.h>
#include <allegro5/bitmap.h>

#include <array>
#include <cstdint>
#include <map>
#include <random>

#include "controller.h"
#include "game.h"

struct Pixel {
  int16_t x, y;
};

class Renderer {
 public:
  Renderer(int width, int height, int scale);
  void init();
  void reset(int width, int height, int scale);

  void draw(const Game& game, const Controller& controller) const;

 private:
  void drawBackground() const;
  void drawGrid(const Game& game) const;
  void drawCards(const Game& game) const;
  void drawCursor(const Game& game, const Vec2i mousePos) const;

 private:
  int width_;
  int height_;
  int scale_;
  ALLEGRO_DISPLAY* display_;

  Vec2i grid_origin;

  mutable std::default_random_engine random_generator_;
  std::map<Texture, ALLEGRO_BITMAP*> textures_;
  std::map<Texture, Vec2> texture_dimensions_;
  std::unique_ptr<ALLEGRO_FONT, void (*)(ALLEGRO_FONT*)> font_;
  std::unique_ptr<ALLEGRO_BITMAP, void (*)(ALLEGRO_BITMAP*)> bitmap_;
  int dialog_font_line_height;
};

#endif  // RENDERER_H
