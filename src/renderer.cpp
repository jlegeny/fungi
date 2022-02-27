#include "renderer.h"

#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/bitmap.h>

#include <array>
#include <cmath>
#include <iostream>

#include "util.h"

constexpr int FONT_SIZE = 10;

constexpr ALLEGRO_COLOR BLACK = {0.0, 0.0, 0.0, 1};
constexpr ALLEGRO_COLOR MAGENTA = {1.0, 0.28, 0.76, 1};
constexpr ALLEGRO_COLOR CYAN = {0, 1, 1, 1};
constexpr ALLEGRO_COLOR OBSIDIAN = {0, 0, 0, 0.9};

constexpr ALLEGRO_COLOR EARTH7 = {244. / 255, 204. / 255, 161. / 255, 1};

constexpr std::pair<Texture, const char*> TEXTURE_FILES[] = {
    {Texture::CURSOR, "assets/cursors/cursor.png"},
    {Texture::BACKGROUND, "assets/textures/background.png"},
    {Texture::TILE_OUTLINE, "assets/textures/tile-outline.png"},
    {Texture::TILE_GRASS_01, "assets/textures/tile-grass-01.png"},
    {Texture::TILE_LUSH_GRASS_01, "assets/textures/tile-lush-grass-01.png"},
    {Texture::TILE_CONTROL_01, "assets/textures/tile-control-01.png"},
    {Texture::TILE_CONTROL_02, "assets/textures/tile-control-02.png"},
    {Texture::TILE_CONTROL_03, "assets/textures/tile-control-03.png"},
    {Texture::TILE_CONTROL_04, "assets/textures/tile-control-04.png"},
    {Texture::OBJECT_SHROOM_01, "assets/textures/object-shroom-01.png"},
    {Texture::OBJECT_SHROOM_02, "assets/textures/object-shroom-02.png"},
    {Texture::OBJECT_SHROOM_03, "assets/textures/object-shroom-03.png"},
    {Texture::OBJECT_SPORES_01, "assets/textures/object-spores-01.png"},
    {Texture::OBJECT_SPORES_02, "assets/textures/object-spores-02.png"},
    {Texture::OBJECT_SPORES_03, "assets/textures/object-spores-03.png"},
    {Texture::CARD_RAIN_M, "assets/textures/card-rain-m.png"},
    {Texture::CARD_SPORES_M, "assets/textures/card-spores-m.png"},
    {Texture::CARD_WIND_M, "assets/textures/card-wind-m.png"},
};

namespace {
Vec2 flat_hex_corner(Vec2 center, int size, int i) {
  float angle_rad = M_PI / 3 * i;
  return {center.x + size * cos(angle_rad), center.y + size * sin(angle_rad)};
}

Texture animation_frame_object(const Tile& tile) {
  switch (tile.obj) {
    case Object::SHROOM:
      if (tile.obj_frame == 0 || tile.obj_frame == 2) {
        return Texture::OBJECT_SHROOM_01;
      } else if (tile.obj_frame == 1) {
        return Texture::OBJECT_SHROOM_02;
      } else {
        return Texture::OBJECT_SHROOM_03;
      }
    case Object::SPORES:
      if (tile.obj_frame == 0 || tile.obj_frame == 4) {
        return Texture::OBJECT_SPORES_01;
      } else if (tile.obj_frame == 1 || tile.obj_frame == 3) {
        return Texture::OBJECT_SPORES_02;
      } else {
        return Texture::OBJECT_SPORES_03;
      }
    default:
      return Texture::INVALID;
  }
}

Texture animation_frame_tile(const Tile& tile) {
  switch (tile.type) {
    case TileType::GRASS:
      return Texture::TILE_GRASS_01;
    case TileType::LUSH_GRASS:
      return Texture::TILE_LUSH_GRASS_01;
    case TileType::CONTROL:
      if (tile.tile_frame == 0) {
        return Texture::TILE_CONTROL_01;
      } else if (tile.tile_frame == 1) {
        return Texture::TILE_CONTROL_02;
      } else if (tile.tile_frame == 2) {
        return Texture::TILE_CONTROL_03;
      } else {
        return Texture::TILE_CONTROL_04;
      }
    default:
      return Texture::TILE_OUTLINE;
  }
}
}  // namespace

Renderer::Renderer(int width, int height, int scale)
    : width_(width)
    , height_(height)
    , scale_(scale)
    , display_(al_get_current_display())
    , font_(al_load_ttf_font("assets/IBMPlexMono-Medium.ttf", FONT_SIZE, 0),
            al_destroy_font)
    , bitmap_(al_create_bitmap(400, 300), al_destroy_bitmap) {
  dialog_font_line_height = al_get_font_line_height(font_.get());
}

void Renderer::init() {
  for (auto [k, v] : TEXTURE_FILES) {
    ALLEGRO_PATH* path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
    al_join_paths(path, al_create_path(v));
    std::cout << "Loading [" << al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP)
              << "] ... ";
    auto* texture = al_load_bitmap(al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP));
    if (!texture) {
      std::cout << "ERROR" << std::endl;
      std::cerr << "Failed to load ["
                << al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP) << "] "
                << al_get_errno() << std::endl;
      exit(1);
    }
    textures_[k] = texture;
    texture_dimensions_[k] = {
        static_cast<float>(al_get_bitmap_width(texture)),
        static_cast<float>(al_get_bitmap_height(texture)),
    };
    std::cout << " OK" << std::endl;
  }
}

void Renderer::reset(int width, int height, int scale) {
  width_ = width;
  height_ = height;
  scale_ = scale;
}

void Renderer::draw(const Game& game, const Controller& controller) const {
  al_set_target_bitmap(bitmap_.get());
  al_clear_to_color(EARTH7);
  drawBackground();
  drawGrid(game);
  drawCards(game);
  drawCursor(game, controller.mousePos);
  al_set_target_bitmap(al_get_backbuffer(display_));
  al_draw_scaled_bitmap(bitmap_.get(), 0, 0, width_, height_, 0, 0,
                        width_ * scale_, height_ * scale_, 0);
}

void Renderer::drawBackground() const {
  int y = 0;
  int x = 0;
  while (y < height_) {
    while (x < width_) {
      al_draw_bitmap(textures_.at(Texture::BACKGROUND), x, y, 0);
      x += 24;
    }
    x = 0;
    y += 24;
  }
}

void Renderer::drawGrid(const Game& game) const {
  size_t q = 0;
  size_t r = 0;
  size_t qbeg = 0;
  size_t rbeg = 0;
  int order = 0;
  while (true) {
    if (q < game.map.size() && r < game.map.size()) {
      const Tile& tile = game.map[q][r];
      Vec2 c = hex2point(tile.coords, HEX_SIZE);
      Vec2 cr = c + GRID_ORIGIN;

      bool windControl =
          tile.type == TileType::CONTROL &&
          (game.selectedCard && game.peekActiveCard().type == CardType::WIND_M);

      if (tile.type != TileType::NONE &&
          (tile.type != TileType::CONTROL || windControl)) {
        Texture texture = animation_frame_tile(tile);
        bool shouldDraw = false;
        if (windControl) {
          if (game.peekActiveCard().selectingOrigin) {
            shouldDraw = true;
          } else if (game.selectedTile == tile.coords) {
            shouldDraw = true;
          }
        } else {
          shouldDraw = true;
        }
        if (shouldDraw) {
          al_draw_bitmap(textures_.at(texture), cr.x - HEX_SIZE - 2, cr.y - 35,
                         0);
        }
        if (game.selectedCard) {
          if (game.isAffected(tile.coords)) {
            al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ONE);
            ALLEGRO_COLOR tint = al_map_rgba_f(0.5, 0.5, 0.5, 1);
            if (game.deck[*game.selectedCard].type == CardType::SPORES_M) {
              tint = al_map_rgba_f(0.5, 0.0, 0.5, 1);
            } else if (game.deck[*game.selectedCard].type == CardType::RAIN_M) {
              tint = al_map_rgba_f(0.0, 0.3, 0.5, 1);
            }
            al_draw_tinted_bitmap(textures_.at(texture), tint,
                                  cr.x - HEX_SIZE - 2, cr.y - 35, 0);
            al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
          }
        }

        if (tile.obj != Object::NONE) {
          Texture obj_texture = animation_frame_object(tile);

          al_draw_bitmap(textures_.at(obj_texture), cr.x - HEX_SIZE - 2,
                         cr.y - 35, 0);
        }
        if (game.debug) {
          al_draw_textf(font_.get(), CYAN, cr.x, cr.y - 15, 0, "%d",
                        tile.coords.q);
          al_draw_textf(font_.get(), MAGENTA, cr.x + 5, cr.y - 5, 0, "%d",
                        tile.coords.r);
        }
      }
      if (game.debug) {
        al_draw_textf(font_.get(), BLACK, cr.x - 5, cr.y - 5, 0, "%d", order);
      }
    }
    if (q == game.map.size() - 1 && r == game.map.size() - 1) {
      break;
    }
    order += 1;
    if (q + 2 >= game.map.size() || r == 0) {
      qbeg += 1;
      if (qbeg == 2) {
        qbeg = 0;
        rbeg += 1;
      }
      q = qbeg;
      r = rbeg;
    } else {
      q += 2;
      r -= 1;
    }
  }
}

void Renderer::drawCards(const Game& game) const {
  int cardTypeIndex = 0;
  for (const Card& card : game.deck) {
    Texture texture;
    switch (card.type) {
      case CardType::RAIN_M:
        texture = Texture::CARD_RAIN_M;
        break;
      case CardType::SPORES_M:
        texture = Texture::CARD_SPORES_M;
        break;
      case CardType::WIND_M:
        texture = Texture::CARD_WIND_M;
        break;
      default:
        continue;
    }

    int cardX = DECK_ORIGIN.x;
    int cardY = DECK_ORIGIN.y + cardTypeIndex * 72;

    for (int j = 0; j < card.amount; ++j) {
      al_draw_bitmap(textures_.at(texture), cardX + j * 12, cardY + j * 8, 0);
    }
    Vec2 lastCardOffset;
    lastCardOffset.x = (card.amount - 1) * 12;
    lastCardOffset.y = (card.amount - 1) * 8;
    if (cardTypeIndex == game.selectedCard) {
      al_draw_rectangle(cardX + lastCardOffset.x, cardY + lastCardOffset.y,
                        cardX + 48 + lastCardOffset.x - 2,
                        cardY + 64 + lastCardOffset.y - 2, MAGENTA, 2);
    } else if (cardTypeIndex == game.hoveredCard) {
      al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ONE);
      ALLEGRO_COLOR tint = al_map_rgba_f(0.2, 0.2, 0.2, 1);
      al_draw_tinted_bitmap(textures_.at(texture), tint,
                            cardX + lastCardOffset.x, cardY + lastCardOffset.y,
                            0);
      al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
    }
    ++cardTypeIndex;
  }
}

void Renderer::drawCursor(const Game& game, const Vec2i mousePos) const {
  al_draw_bitmap(textures_.at(Texture::CURSOR), mousePos.x, mousePos.y, 0);
}
