#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "game.h"

class Controller
{
 public:
  Controller();

  void mouseClick(int button);
  void command(Game& game);

 public:
  Vec2i mousePos;
  int mouseButton;

 private:
  std::default_random_engine generator_;
};

#endif // CONTROLLER_H
