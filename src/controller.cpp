#include "controller.h"

#include <iostream>

#include "data.h"
#include "util.h"

Controller::Controller()
    : mousePos({0, 0})
    , mouseButton(0) {}

void Controller::command(Game& game) {
  Vec2i gridOrigin = vec2i(GRID_ORIGIN);
  Vec2i gridMousePos = mousePos - gridOrigin;
  Hex3 tileCoord = point2hex(gridMousePos, HEX_SIZE);
  game.hoveredTile = std::nullopt;
  if (tileCoord.q >= 0 && tileCoord.q < static_cast<int>(game.map.size()) &&
      tileCoord.r >= 0 &&
      tileCoord.r < static_cast<int>(game.map.at(0).size())) {
    if (game.map[tileCoord.q][tileCoord.r].type != TileType::NONE) {
      game.hoveredTile = tileCoord;
    }
  }

  game.hoveredCard = std::nullopt;
  int cardTypeIndex = 0;
  for (const Card& card : game.deck) {
    for (int cardIndex = 0; cardIndex < card.amount; ++cardIndex) {
      int cardX = DECK_ORIGIN.x;
      int cardY = DECK_ORIGIN.y + cardTypeIndex * 72 + cardIndex * 8;
      if (mousePos.x > cardX && mousePos.y > cardY &&
          mousePos.x < cardX + 48 + (card.amount - 1) * 12 &&
          mousePos.y < cardY + 64) {
        game.hoveredCard = cardTypeIndex;
      }
    }
    ++cardTypeIndex;
  }

  game.affectedTiles.clear();
  auto activeCard = game.activeCard();
  auto activeTile = game.activeTile();
  if (game.selectedCard && activeTile) {
    Hex3 target = *game.hoveredTile;
    std::vector<Hex3> maybeAffected;
    if (activeCard.type == CardType::SPORES_M &&
        activeTile->obj == Object::SHROOM) {
      maybeAffected.push_back({target.q - 1, target.r + 1, target.s});
      maybeAffected.push_back({target.q - 1, target.r, target.s + 1});
      maybeAffected.push_back({target.q + 1, target.r - 1, target.s});
      maybeAffected.push_back({target.q + 1, target.r, target.s - 1});
      maybeAffected.push_back({target.q, target.r + 1, target.s - 1});
      maybeAffected.push_back({target.q, target.r - 1, target.s + 1});
    } else if (activeCard.type == CardType::RAIN_M) {
      maybeAffected.push_back(target);
      maybeAffected.push_back({target.q - 1, target.r + 1, target.s});
      maybeAffected.push_back({target.q - 1, target.r, target.s + 1});
      maybeAffected.push_back({target.q + 1, target.r - 1, target.s});
      maybeAffected.push_back({target.q + 1, target.r, target.s - 1});
      maybeAffected.push_back({target.q, target.r + 1, target.s - 1});
      maybeAffected.push_back({target.q, target.r - 1, target.s + 1});
    } else if (activeCard.type == CardType::WIND_M &&
               activeCard.selectingDirection) {
      int qdiff = activeTile->coords.q - game.selectedTile->q;
      int rdiff = activeTile->coords.r - game.selectedTile->r;
      std::cout << qdiff << " " << rdiff << std::endl;
      if ((qdiff != 0 || rdiff != 0) && abs(qdiff + rdiff) <= 1 &&
          abs(qdiff) <= 1 && abs(rdiff) <= 1) {
        Hex3 next = target;
        while (game.validTile(next)) {
          maybeAffected.push_back(next);
          next = {next.q + qdiff, next.r + rdiff, next.s - qdiff - rdiff};
        }
      }
    }
    for (Hex3 hex : maybeAffected) {
      if (game.validTile(hex)) {
        Tile tile = game.tileAt(hex);
        if (tile.type == TileType::NONE || tile.type == TileType::CONTROL) {
          continue;
        }
        if (activeCard.type == CardType::RAIN_M) {
          game.affectedTiles.push_back(hex);
        } else if (activeCard.type == CardType::SPORES_M) {
          if (tile.obj == Object::NONE) {
            game.affectedTiles.push_back(hex);
          }
        } else if (activeCard.type == CardType::WIND_M) {
          game.affectedTiles.push_back(hex);
        }
      }
    }
  }

  if (mouseButton == 1) {
    if (game.selectedCard && activeTile) {
      if (activeCard.type == CardType::SPORES_M &&
          activeTile->obj == Object::SHROOM) {
        std::uniform_int_distribution<> distrib(0, 4);
        for (Hex3 hex : game.affectedTiles) {
          Tile& tile = game.tileAt(hex);
          if (tile.obj == Object::NONE) {
            tile.obj = Object::SPORES;
            tile.obj_frame = distrib(generator_);
          }
        }
      } else if (activeCard.type == CardType::RAIN_M) {
        std::uniform_int_distribution<> distrib(0, 3);
        for (Hex3 hex : game.affectedTiles) {
          Tile& tile = game.tileAt(hex);
          if (tile.obj == Object::SPORES) {
            tile.obj = Object::SHROOM;
            tile.obj_frame = distrib(generator_);
          }
        }
      } else if (activeCard.type == CardType::WIND_M &&
                 activeCard.selectingOrigin) {
        game.activeCard().selectingOrigin = false;
        game.activeCard().selectingDirection = true;
        game.selectedTile = activeTile->coords;
      }
    } else if (game.hoveredCard) {
      game.selectedCard = game.hoveredCard;
      game.activeCard().selectingOrigin = true;
      game.activeCard().selectingDirection = false;
    }
    mouseButton = 0;
  }
}
