#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

#include <iostream>
#include <memory>
#include <unordered_set>

#include "controller.h"
#include "game.h"
#include "renderer.h"

constexpr int RENDER_WIDTH = 400;
constexpr int RENDER_HEIGHT = 300;
constexpr int RENDER_SCALE = 4;

constexpr int FONT_SIZE = 18;

constexpr ALLEGRO_COLOR DEBUG_COLOR = {0.0, 1.0, 0.2, 1};

int real_main(int argc, char** argv) {
  al_init();
  al_install_keyboard();
  al_install_mouse();

  ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
  ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();

  // al_set_new_display_flags(ALLEGRO_RESIZABLE);
  ALLEGRO_DISPLAY* display = al_create_display(RENDER_WIDTH * RENDER_SCALE,
                                               RENDER_HEIGHT * RENDER_SCALE);

  al_register_event_source(queue, al_get_keyboard_event_source());
  al_register_event_source(queue, al_get_mouse_event_source());
  al_register_event_source(queue, al_get_display_event_source(display));

  al_register_event_source(queue, al_get_timer_event_source(timer));

  al_init_primitives_addon();
  al_init_ttf_addon();
  al_init_image_addon();

  ALLEGRO_FONT* font =
      al_load_ttf_font("assets/IBMPlexMono-Medium.ttf", FONT_SIZE, 0);
  ALLEGRO_FONT* big_font =
      al_load_ttf_font("assets/IBMPlexMono-Medium.ttf", 30, 0);

  al_hide_mouse_cursor(display);

  // Create the game

  Game game;
  Renderer renderer(RENDER_WIDTH, RENDER_HEIGHT, RENDER_SCALE);
  Controller controller;
  renderer.init();
  game.debug = false;
  game.state = GameState::MAIN_LOOP;

  uint32_t last_ticks = al_get_time() * 1000;

  ALLEGRO_COLOR text_color = al_map_rgb(0, 255, 0);
  ALLEGRO_EVENT event;
  ALLEGRO_KEYBOARD_STATE ks;

  char strbuff[200];
  bool redraw = true;
  int frame = 0;

  al_start_timer(timer);

  std::unordered_set<std::string> debug_flags;

  bool done = false;
  bool checkmouse = false;
  while (!done) {
    al_wait_for_event(queue, &event);

    if (event.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
      int width = al_get_display_width(display);
      int height = (width * RENDER_HEIGHT) / RENDER_WIDTH;
      // al_resize_display(display, width, height);
      al_acknowledge_resize(display);
      renderer.reset(RENDER_WIDTH, RENDER_HEIGHT, RENDER_SCALE);
    } else if (event.type == ALLEGRO_EVENT_TIMER) {
      redraw = true;
    } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
      done = true;
    } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
      if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE ||
          event.keyboard.keycode == ALLEGRO_KEY_Q) {
        game.state = GameState::QUIT;
        done = true;
      }
      if (event.keyboard.keycode == ALLEGRO_KEY_F1) {
        game.debug = !game.debug;
      }
      if (event.keyboard.keycode == ALLEGRO_KEY_F2) {
      }
      if (game.state == GameState::MENU &&
          event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
        game.state = GameState::MAIN_LOOP;
      }
    } else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
      checkmouse = true;
    }

    al_get_keyboard_state(&ks);


    ALLEGRO_MOUSE_STATE mouse;
    al_get_mouse_state(&mouse);
    controller.mousePos = {.x = mouse.x, .y = mouse.y};
    controller.mousePos /= RENDER_SCALE;

    if (checkmouse) {
      if (al_mouse_button_down(&mouse, 1)) {
        controller.mouseButton = 1;
      }
      checkmouse = false;
    }
    uint32_t ticks = al_get_time() * 1000;
    uint32_t dt = ticks - last_ticks;
    if (!game.gameover) {
      controller.command(game);
    }
    game.update(dt);
    last_ticks = ticks;

    if (redraw && al_is_event_queue_empty(queue)) {
      al_clear_to_color(al_map_rgb(0, 0, 0));
      if (game.state == GameState::MAIN_LOOP) {
        renderer.draw(game, controller);
      }
      if (game.state == GameState::MENU) {
        int line = 0;
        al_draw_text(big_font, text_color, RENDER_WIDTH * RENDER_SCALE / 2,
                     150 + ++line * 30, ALLEGRO_ALIGN_CENTRE, "Menu");
        ++line;
      }

      if (game.debug) {
        int debugX = RENDER_WIDTH * RENDER_SCALE - 400;
        int stri = 0;
        snprintf(strbuff, sizeof(strbuff), "FPS: %.1f", 1000.f / dt);
        al_draw_text(font, DEBUG_COLOR, debugX, ++stri * FONT_SIZE, 0, strbuff);
        snprintf(strbuff, sizeof(strbuff), "Mouse: %d / %d", mouse.x, mouse.y);
        al_draw_text(font, DEBUG_COLOR, debugX, ++stri * FONT_SIZE, 0, strbuff);
      }

      al_flip_display();

      redraw = false;
    }

    ++frame;
  }

  al_destroy_font(font);
  al_destroy_display(display);
  al_destroy_timer(timer);
  al_destroy_event_queue(queue);

  return 0;
}

int main(int argc, char** argv) {
  return real_main(argc, argv);
  // return al_run_main(argc, argv, real_main);
}
