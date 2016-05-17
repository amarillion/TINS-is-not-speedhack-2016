#include <inc/engine.hpp>

Engine::Engine(void) {

  window_w = 0;
  window_h = 0;

  is_running = true;
  is_updating = false;

  window_title = "";

  timer = NULL;
  display = NULL;
  event_queue = NULL;
}

int Engine::getScreenWidth(void) {

  return window_w;
}

int Engine::getScreenHeight(void) {

  return window_h;
}

bool Engine::isKey(const int key_code) {

  if (event.keyboard.keycode == key_code) {

    return true;
  }

  return false;
}

bool Engine::isRunning() {

  return is_running;
}

bool Engine::isKeyPressed(void) {

  if (event.type == ALLEGRO_EVENT_KEY_DOWN) {

    return true;
  }

  return false;
}

bool Engine::isKeyReleased(void) {

  if (event.type == ALLEGRO_EVENT_KEY_UP) {

    return true;
  }

  return false;
}

bool Engine::isRenderPhase(void) {

  return is_updating;
}

bool Engine::isUpdatePhase(void) {

  if ((!is_updating) && (al_is_event_queue_empty(event_queue))) {

    return true;
  }

  return false;
}

void Engine::flipPhase(void) {

  if (is_updating) {

    is_updating = false;
  }
  else {

    is_updating = true;
  }
}

void Engine::initialize(void) {

  checkReturn(al_init());

  checkReturn(al_init_font_addon());
  checkReturn(al_init_ttf_addon());
  checkReturn(al_init_image_addon());
  checkReturn(al_init_acodec_addon());
  checkReturn(al_init_primitives_addon());

  checkReturn(al_install_audio());
  checkReturn(al_install_keyboard());
}

void Engine::startTimer(void) {

  al_start_timer(timer);
}

void Engine::checkReturn(const bool condition) {

  if (!condition) {

    // Something failed; end the game.
    is_running = false;
  }
}

void Engine::flipDisplay(void) {

  al_flip_display();

  al_clear_to_color(al_map_rgb(255, 0, 0));
}

void Engine::createEvents(void) {

  al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);

  timer = al_create_timer(1.0 / 60.0);

  display = al_create_display(window_w, window_h);

  event_queue = al_create_event_queue();

  al_set_window_title(display, window_title.c_str());
}

void Engine::registerEvents(void) {

  al_register_event_source(event_queue, al_get_keyboard_event_source());
  al_register_event_source(event_queue, al_get_timer_event_source(timer));
  al_register_event_source(event_queue, al_get_display_event_source(display));
}

void Engine::setWindowTitle(const string title) {

  window_title = title;
}

void Engine::reserveSamples(const int number_of_samples) {

  al_reserve_samples(number_of_samples);
}

void Engine::checkEventQueue(void) {

  al_wait_for_event(event_queue, &event);
}

void Engine::setDisplayScale(const float scale) {

  ALLEGRO_TRANSFORM transform;

  al_identity_transform(&transform);

  al_scale_transform(&transform, scale, scale);

  al_use_transform(&transform);
}

void Engine::setResourcePath(const string directory) {

  ALLEGRO_PATH *path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);

  al_append_path_component(path, directory.c_str());

  al_change_directory(al_path_cstr(path, '/'));

  al_destroy_path(path);
}

void Engine::destroyResources(void) {

  al_destroy_timer(timer);
  al_destroy_display(display);
  al_destroy_event_queue(event_queue);
}

void Engine::manageWindowEvents(void) {

  if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {

    // The window was closed; end the game.
    is_running = false;
  }
  else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {

    if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {

      // Close the window and end the game.
      is_running = false;
    }
  }
}

void Engine::setWindowDimensions(const int width, const int height) {

  window_w = width;
  window_h = height;
}
