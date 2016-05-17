#define ALLEGRO_UNSTABLE

#include <allegro_flare/allegro_flare.h>

#define SCREEN_W 1920
#define SCREEN_H 1080

#include "style_assets.hpp"

#include "logging.hpp"
#include "target_id.hpp"
#include "inventory.hpp"
#include "start_screen.hpp"
#include "world_render.hpp"
#include "navigation.hpp"
#include "script.hpp"

#include "game/project.hpp"
#include "game/load_game_scripts.hpp"


////////////////////////////////////////////////////////////////


int main(int argc, char **argv)
{
  af::initialize();
  Display *display = af::create_display(SCREEN_W, SCREEN_H, ALLEGRO_OPENGL | ALLEGRO_PROGRAMMABLE_PIPELINE);
  Project *project = new Project(display);
  af::run_loop();
  return 0;
}


