#include <inc/map.hpp>
#include <inc/game.hpp>
#include <inc/camera.hpp>
#include <inc/engine.hpp>
#include <inc/player.hpp>

Map Map;
Camera Camera;
Engine Engine;
Player Player;

Game::Game(void) {

  scale = 4;

  keys[UP] = false;
  keys[DOWN] = false;
  keys[LEFT] = false;
  keys[RIGHT] = false;

  seed = 0;

  builtin = NULL;
  japanese = NULL;
}

void Game::loop(void) {

  Engine.startTimer();

  while (Engine.isRunning()) {

    Engine.checkEventQueue();

    Engine.manageWindowEvents();

    if (Engine.isKeyPressed()) {

      if (Engine.isKey(ALLEGRO_KEY_UP)) {

        keys[UP] = true;
      }
      else if (Engine.isKey(ALLEGRO_KEY_DOWN)) {

        keys[DOWN] = true;
      }
      else if (Engine.isKey(ALLEGRO_KEY_LEFT)) {

        keys[LEFT] = true;
      }
      else if (Engine.isKey(ALLEGRO_KEY_RIGHT)) {

        keys[RIGHT] = true;
      }
      else if (Engine.isKey(ALLEGRO_KEY_Z)) {

        Player.switchItem(0);
      }
      else if (Engine.isKey(ALLEGRO_KEY_C)) {

        Player.switchItem(1);
      }
      else if (Engine.isKey(ALLEGRO_KEY_X)) {

        Player.useItem(Map.getTiles());
      }
    }
    else if (Engine.isKeyReleased()) {

      if (Engine.isKey(ALLEGRO_KEY_UP)) {

        keys[UP] = false;
      }
      else if (Engine.isKey(ALLEGRO_KEY_DOWN)) {

        keys[DOWN] = false;
      }
      else if (Engine.isKey(ALLEGRO_KEY_LEFT)) {

        keys[LEFT] = false;
      }
      else if (Engine.isKey(ALLEGRO_KEY_RIGHT)) {

        keys[RIGHT] = false;
      }
    }

    if (Engine.isUpdatePhase()) {

      Engine.flipPhase();

      Player.move(keys[UP], keys[DOWN], keys[LEFT], keys[RIGHT], Map.getTiles());

      Player.update();

      Camera.setTargetX(Player.getX());
      Camera.setTargetY(Player.getY());

      Camera.pan();
    }

    if (Engine.isRenderPhase()) {

      Engine.flipPhase();

      Map.render(Camera.getX(), Camera.getY());

      Player.render(Camera.getX(), Camera.getY());

      Engine.flipDisplay();
    }
  }
}

void Game::setSeed(const int s) {

  seed = s;
}

void Game::initialize(void) {

  Engine.initialize();

  Engine.setWindowTitle("Mori");

  Engine.setWindowDimensions(768, 448);

  Engine.createEvents();
  Engine.registerEvents();

  Engine.setDisplayScale(scale);

  Engine.reserveSamples(15);

  loadResources();

  loop();

  destroyResources();
}

void Game::loadResources(void) {

  //seed = 123;//time(NULL);

  Engine.setResourcePath("data");

  builtin = al_create_builtin_font();

  japanese = al_load_ttf_font("ttf/misaki_gothic.ttf", 8, 0);

  Map.setSeed(seed);

  Engine.checkReturn(Map.loadBitmap());
  Engine.checkReturn(Map.loadSample());

  Map.loadTiles();

  Camera.setMapWidth(Map.getWidth());
  Camera.setMapHeight(Map.getHeight());
  Camera.setDisplayWidth(Engine.getScreenWidth() / scale);
  Camera.setDisplayHeight(Engine.getScreenHeight() / scale);

  Engine.checkReturn(Player.loadBitmap());
  Engine.checkReturn(Player.loadSamples());

  Player.setSpawn(Map.getWidth(), Map.getHeight());
  Player.setMapWidth(Map.getWidth());
  Player.setMapHeight(Map.getHeight());
}

void Game::destroyResources(void) {

  al_destroy_font(builtin);
  al_destroy_font(japanese);

  Map.destroyBitmap();

  Player.destroyBitmap();
  Player.destroySamples();

  Engine.destroyResources();
}
