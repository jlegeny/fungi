#include "game.h"

#include <iostream>

#include "util.h"

namespace {
uint32_t object_frame_duration(Object obj, int frame) {
  switch (obj) {
    case Object::SHROOM:
      return 192;
    case Object::SPORES:
      return 256;
    default:
      return 0;
  }
}
int object_next_frame(Object obj, int frame) {
  switch (obj) {
    case Object::SHROOM:
      return (frame + 1) % 4;
    case Object::SPORES:
      return (frame + 1) % 5;
    default:
      return 0;
  }
}
}  // namespace

Game::Game()
    : time_(0) {
  const int map_size = 11;
  map.resize(map_size);
  for (int q = 0; q < map_size; ++q) {
    map[q].resize(map_size);
    for (int r = 0; r < map_size; ++r) {
      map[q][r].coords = {.q = q, .r = r, .s = -q - r};
    }
  }

  int cutoff = (map_size - 1) / 2;

  std::uniform_int_distribution<> distrib(1, 2);

  for (auto& row : map) {
    for (Tile& tile : row) {
      if (tile.coords.q + tile.coords.r < cutoff ||
          tile.coords.q + tile.coords.r > 2 * (map_size - 1) - cutoff) {
        tile.type = TileType::NONE;
      } else if (tile.coords.q == 0 || tile.coords.q == map_size - 1 ||
                 tile.coords.r == 0 || tile.coords.r == map_size - 1 ||
                 tile.coords.s == -cutoff ||
                 tile.coords.s == -(map_size + cutoff - 1)) {
        tile.type = TileType::CONTROL;
      } else {
        if (distrib(generator_) == 1) {
          tile.type = TileType::GRASS;
        } else {
          tile.type = TileType::LUSH_GRASS;
        }
        tile.obj = Object::NONE;
      }
    }
  }
  map[5][5].obj = Object::SHROOM;

  deck.push_back({.type = CardType::RAIN_M, .amount = 2});
  deck.push_back({.type = CardType::SPORES_M, .amount = 1});
  deck.push_back({.type = CardType::WIND_M, .amount = 1});
  selectedCard = 0;
}

void Game::update(uint32_t dt) {
  if (state != GameState::MAIN_LOOP) {
    return;
  }
  updateAnimations(dt);
}

void Game::updateAnimations(uint32_t dt) {
  for (auto& row : map) {
    for (Tile& tile : row) {
      tile.obj_frame_time += dt;
      uint32_t frame_duration = object_frame_duration(tile.obj, tile.obj_frame);
      if (tile.obj_frame_time >= frame_duration) {
        tile.obj_frame_time -= frame_duration;
        tile.obj_frame = object_next_frame(tile.obj, tile.obj_frame);
      }
    }
  }
}

void Game::primaryAction() {}

std::optional<Tile> Game::activeTile() const {
  if (!hoveredTile) {
    return std::nullopt;
  }
  Hex3 hex = hoveredTile.value();
  if (!validTile(hex)) {
    return std::nullopt;
  }
  Tile tile = map.at(hex.q).at(hex.r);
  if (tile.type == TileType::NONE) {
    return std::nullopt;
  }
  return tile;
}

Card& Game::activeCard() {
  if (!selectedCard) {
    exit(1);
  }
  return deck[*selectedCard];
}

const Card& Game::peekActiveCard() const {
  if (!selectedCard) {
    exit(1);
  }
  return deck[*selectedCard];
}

bool Game::isAffected(Hex3 hex) const {
  return std::any_of(affectedTiles.begin(), affectedTiles.end(),
                     [hex](Hex3 affected) { return hex == affected; });
}

bool Game::validTile(Hex3 hex) const {
  return hex.q >= 0 && hex.r >= 0 && hex.q < static_cast<int>(map.size()) &&
         hex.r < static_cast<int>(map.at(0).size());
}

Tile& Game::tileAt(Hex3 hex) {
  return map[hex.q][hex.r];
}
