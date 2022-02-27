#ifndef GAME_H
#define GAME_H

#include <optional>
#include <random>
#include <unordered_set>
#include <vector>

#include "util.h"

enum class GameState {
  MENU,
  MAIN_LOOP,
  QUIT,
};

enum class Object {
  NONE,
  SHROOM,
  SHROOMS,
  SPORES,
};

enum class TileType {
  NONE,
  CONTROL,
  GRASS,
  LUSH_GRASS,
  MOSS,
  SAND,
  TREE,
};

enum class CardType {
  RAIN_M,
  SPORES_M,
  WIND_M,
};

struct Tile {
  TileType type;
  Object obj;
  Hex3 coords;
  int tile_frame;
  int obj_frame;
  uint32_t tile_frame_time;
  uint32_t obj_frame_time;
};

struct Card {
  CardType type;
  int amount;
  bool selectingOrigin;
  bool selectingDirection;
};

typedef std::vector<std::vector<Tile>> Grid;

class Game {
 public:
  Game();

  void update(uint32_t dt);

  void primaryAction();

  std::optional<Tile> activeTile() const;
  Card& activeCard();
  const Card& peekActiveCard() const;
  bool isAffected(Hex3 hex) const;
  bool validTile(Hex3 hex) const;
  Tile& tileAt(Hex3 hex);

 private:
  void updateAnimations(uint32_t dt);

 public:
  GameState state = GameState::MENU;
  bool gameover = false;
  bool debug = false;

  Grid map;
  std::optional<Hex3> hoveredTile;
  std::optional<Hex3> selectedTile;
  std::vector<Hex3> affectedTiles;
  std::vector<Hex3> highlightedTiles;
  std::optional<size_t> hoveredCard;
  std::optional<size_t> selectedCard;
  std::vector<Card> deck;

 private:
  uint32_t time_;

  std::default_random_engine generator_;
};

#endif  // GAME_H
