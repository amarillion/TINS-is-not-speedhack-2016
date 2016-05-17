#include <inc/player.hpp>

Player::Player(void) {

  use_japanese = false;

  dir_to_move[UP] = false;
  dir_to_move[DOWN] = false;
  dir_to_move[LEFT] = false;
  dir_to_move[RIGHT] = false;

  snow_walk = false;

  lock_controls = false;

  snow_percent = 0.0;

  message_cap = 3;
  message_count = 0;

  messages.resize(9);
  names.resize(333);

  items_wanted.resize(333);
  items_owned.resize(333);
  items_count.resize(333);

  die_after = false;

  for (int i = 0; i < 333; ++i) {

    names[i].resize(333);

    items_wanted[i].resize(333);
    items_owned[i].resize(333);
    items_count[i].resize(333);
  }

  for (int y = 0; y < 333; ++y) {

    for (int x = 0; x < 333; ++x) {

      names[x][y] = "???";
      items_wanted[x][y] = -1;
      items_owned[x][y] = 0;
      items_count[x][y] = 0;
    }
  }

  w = 16;
  h = 16;

  item_count.resize(12);

  for (unsigned int i = 0; i < item_count.size(); ++i) {

    // Zero items by default.
    item_count[i] = 0;
  }

  show_dialog = false;

  // 64 interactive. Doesn't matter so long as > 0.
  item_count[0] = 64;

  current_item = 0;

  map_width = 0;
  map_height = 0;

  x = 0.0;
  y = 0.0;

  speed = 1.0;

  moving[UP] = false;
  moving[DOWN] = false;
  moving[LEFT] = false;
  moving[RIGHT] = false;

  spawned = false;

  is_moving = false;

  bitmap = NULL;
  hud = NULL;

  builtin = NULL;
  japanese = NULL;

  hit_sample = NULL;
  blip_sample = NULL;
  blip2_sample = NULL;
  plant_sample = NULL;
  death_sample = NULL;
  boom_sample = NULL;

  setFacingDirection(RIGHT);
}

int Player::getWidth(void) {

  return w;
}

int Player::getHeight(void) {

  return h;
}

float Player::getX(void) {

  return x;
}

float Player::getY(void) {

  return y;
}

bool Player::loadBitmap(void) {

  bitmap = al_load_bitmap("png/player.png");

  if (!bitmap) {

    return false;
  }

  hud = al_load_bitmap("png/hud.png");

  if (!hud) {

    return false;
  }

  builtin = al_create_builtin_font();

  japanese = al_load_ttf_font("ttf/misaki_gothic.ttf", 8, 0);

  return true;
}

bool Player::loadSamples(void) {

  hit_sample = al_load_sample("ogg/hit.ogg");

  plant_sample = al_load_sample("ogg/plant.ogg");

  blip_sample = al_load_sample("ogg/blip.ogg");

  blip2_sample = al_load_sample("ogg/blip2.ogg");

  death_sample = al_load_sample("ogg/death.ogg");

  boom_sample = al_load_sample("ogg/boom.ogg");

  powerup_sample = al_load_sample("ogg/powerup.ogg");

  if (!hit_sample || !plant_sample || !blip_sample || !blip_sample || !death_sample || !boom_sample || !powerup_sample) {

    return false;
  }

  return true;
}

// @TODO: Make these const.
void Player::move(bool up, bool down, bool left, bool right, vector < vector < string > > &map_tiles) {

  if (lock_controls) {

    up = false;
    down = false;
    left = false;
    right = false;
  }

  if (show_dialog) return;

  if (snow_walk) {

    static bool just_collected = true;

    if (just_collected) {

      lock_controls = true;

      if (dir_to_move[UP]) {

        up = true;

        setFacingDirection(UP);
      }
      else if (dir_to_move[DOWN]) {

        down = true;

        setFacingDirection(DOWN);
      }
      else if (dir_to_move[LEFT]) {

        left = true;

        setFacingDirection(LEFT);
      }
      else if (dir_to_move[RIGHT]) {

        right = true;

        setFacingDirection(RIGHT);
      }

      if (map_tiles[(x / w)][(y / h) - 1].substr(5, 6) == "S") {

        if (map_tiles[(x / w)][(y / h) - 1] != "99x99S") {

          map_tiles[(x / w)][(y / h) - 1] = "00x00L";

          al_play_sample(boom_sample, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
        }
      }

      if (map_tiles[(x / w)][(y / h) + 1].substr(5, 6) == "S") {

        if (map_tiles[(x / w)][(y / h) + 1] != "99x99S") {

          map_tiles[(x / w)][(y / h) + 1] = "00x00L";

          al_play_sample(boom_sample, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
        }
      }

      if (map_tiles[(x / w) - 1][(y / h)].substr(5, 6) == "S") {

        if (map_tiles[(x / w) - 1][(y / h)] != "99x99S") {

          map_tiles[(x / w) - 1][(y / h)] = "00x00L";

          al_play_sample(boom_sample, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
        }
      }

      if (map_tiles[(x / w) + 1][(y / h)].substr(5, 6) == "S") {

        if (map_tiles[(x / w) + 1][(y / h)] != "99x99S") {

          map_tiles[(x / w) + 1][(y / h)] = "00x00L";

          al_play_sample(boom_sample, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
        }
      }

      static int sy = y / h;
      static int sx = x / w;

      if (dir_to_move[UP] && map_tiles[int(x / w)][int(y / h) - 1] == "99x99S") {

        sy = 9999;
      }

      if (dir_to_move[DOWN] && map_tiles[int(x / w)][int(y / h) + 1] == "99x99S") {

        sy = -9999;
      }

      if (dir_to_move[UP] && map_tiles[int(x / w) - 1][int(y / h)] == "99x99S") {

        sx = 9999;
      }

      if (dir_to_move[DOWN] && map_tiles[int(x / w) + 1][int(y / h)] == "99x99S") {

        sx = -9999;
      }

      if (dir_to_move[UP] && int(y / h) <= sy - 10) {

        just_collected = false;

        setFacingDirection(DOWN);

        lock_controls = false;
      }

      if (dir_to_move[DOWN] && int(y / h) >= sy + 10) {

        just_collected = false;

        setFacingDirection(DOWN);

        lock_controls = false;
      }

      if (dir_to_move[LEFT] && int(x / w) <= sx - 10) {

        just_collected = false;

        setFacingDirection(DOWN);

        lock_controls = false;
      }

      if (dir_to_move[RIGHT] && int(x / w) >= sx + 10) {

        just_collected = false;

        setFacingDirection(DOWN);

        lock_controls = false;
      }
    }

    int map_w = map_tiles.size();
    int map_h = map_tiles[0].size();

    int cx = x / w;
    int cy = y / h;

    int tiles_up = cy;
    int tiles_down = map_h - cy;

    int tiles_left = cx;
    int tiles_right = map_w - cx;

    int y_results = 0;
    int x_results = 0;

    if (tiles_up > tiles_down) {

      // More tiles up.
      y_results = 0;
    }
    else if (tiles_up < tiles_down) {

      // More tiles down.
      y_results = 1;
    }
    else {

      // Probably equal.
      y_results = rand() % 1;
    }

    if (tiles_left > tiles_right) {

      // More tiles left.
      x_results = 0;
    }
    else if (tiles_left < tiles_right) {

      // More tiles right.
      x_results = 1;
    }
    else {

      // Probably equal.
      x_results = rand() % 1;
    }

    if (x_results > y_results) {

      // More space on X axis.

      if (rand() % 1) {

        dir_to_move[LEFT] = true;
      }
      else {

        dir_to_move[right] = true;
      }
    }
    else {

      if (rand() % 1) {

        dir_to_move[UP] = true;
      }
      else {

        dir_to_move[DOWN] = true;
      }
    }

    if (map_tiles[int(x / w)][int(y / h)] == "00x00L") {

      map_tiles[int(x / w)][int(y / h)] = "01x03L";
    }

    if (map_tiles[int(x / w)][int(y / h) - 1] == "00x00L") {

      map_tiles[int(x / w)][int(y / h) - 1] = "01x03L";
    }

    if (map_tiles[int(x / w)][int(y / h) + 1] == "00x00L") {

      map_tiles[int(x / w)][int(y / h) + 1] = "01x03L";
    }

    if (map_tiles[int(x / w) - 1][int(y / h)] == "00x00L") {

      map_tiles[int(x / w) - 1][int(y / h)] = "01x03L";
    }

    if (map_tiles[int(x / w) + 1][int(y / h)] == "00x00L") {

      map_tiles[int(x / w) + 1][int(y / h)] = "01x03L";
    }
  }

  static bool calculated = false;

  static float area = 0;
  float tiles = 0;

  if (!calculated) {

    calculated = true;

    // Get area of map.
    area = map_tiles.size() * map_tiles[0].size();
  }

  for (unsigned int i = 0; i < map_tiles[0].size(); ++i) {

    for (unsigned int j = 0; j < map_tiles.size(); ++j) {

      if (map_tiles[j][i] == "01x03L") {

        // Get number of snow tiles.
        ++tiles;
      }
    }
  }

  // How much of the map is covered in snow?
  snow_percent = tiles / area;

  for (unsigned int i = 0; i < map_tiles[0].size(); ++i) {

    for (unsigned int j = 0; j < map_tiles.size(); ++j) {

      if ((map_tiles[j][i] == "00x02S") || (map_tiles[j][i] == "01x02S") || (map_tiles[j][i] == "02x02S") || (map_tiles[j][i] == "03x02S")) {

        if ((x / w) > j) {

          // Make NPC look to the right.
          map_tiles[j][i] = "03x02S";
        }
        else if ((x / w) < j) {

          // Make NPC look to the left.
          map_tiles[j][i] = "02x02S";
        }
        else if ((y / h) > i) {

          // Make NPC look down.
          map_tiles[j][i] = "01x02S";
        }
        else if ((y / h) < i) {

          // Make NPC look up.
          map_tiles[j][i] = "00x02S";
        }
      }
    }
  }

  if (!spawned) {

    spawned = true;

    if (map_tiles[int(x / w)][int(y / h)] == "99x99S") {

      // Player spawned out of bounds. Try another spawn spot.
      setSpawn(map_width, map_height);
    }

    // Change the player's spawn tile to grass (don't want to get stuck in a solid tile!).
    map_tiles[int(x / w)][int(y / h)] = "00x00L";

    messages = {

      "Hey, welcome to the game. (Press X)",

      "Z and C move the item selector.",

      "Press X to use the current item.",

      "Use arrow keys to move around.",

      "Use \"!\" to interact with things.",

      "You can trade items with forest folk.",

      "Go forth and cover the land in snow!"
    };

    message_cap = messages.size();

    show_dialog = true;
  }

  bool within_boundaries = false;

  if (y >= 0 && y <= map_height * h && x >= 0 && x <= map_width * w) {

    within_boundaries = true;
  }

  if (!is_moving) {

    if (up) {

      static int ticks = 0;

      if (!facing[UP]) {

        up = false;

        setFacingDirection(UP);

        ticks = 0;

        return;
      }

      if (ticks < 5) {

        ++ticks;

        return;
      }

      if (within_boundaries) {

        if (y / h == 0) {

          return;
        }

        if (map_tiles[(x / w)][(y / h) - 1].substr(5, 6) == "S") {

          return;
        }
      }

      is_moving = true;

      moving[UP] = true;
      moving[DOWN] = false;
      moving[LEFT] = false;
      moving[RIGHT] = false;

      setFacingDirection(UP);
    }
    else if (down) {

      static int ticks = 0;

      if (!facing[DOWN]) {

        down = false;

        setFacingDirection(DOWN);

        ticks = 0;

        return;
      }

      if (ticks < 5) {

        ++ticks;

        return;
      }

      if (within_boundaries) {

        if (y / h == map_height - 1) {

          return;
        }

        if (map_tiles[(x / w)][(y / h) + 1].substr(5, 6) == "S") {

          return;
        }
      }

      is_moving = true;

      moving[UP] = false;
      moving[DOWN] = true;
      moving[LEFT] = false;
      moving[RIGHT] = false;

      setFacingDirection(DOWN);
    }
    else if (left) {

      static int ticks = 0;

      if (!facing[LEFT]) {

        left = false;

        setFacingDirection(LEFT);

        ticks = 0;

        return;
      }

      if (ticks < 5) {

        ++ticks;

        return;
      }

      if (within_boundaries) {

        if (x / w == 0) {

          return;
        }

        if (map_tiles[(x / w) - 1][(y / h)].substr(5, 6) == "S") {

          return;
        }
      }

      is_moving = true;

      moving[UP] = false;
      moving[DOWN] = false;
      moving[LEFT] = true;
      moving[RIGHT] = false;
    }
    else if (right) {

      static int ticks = 0;

      if (!facing[RIGHT]) {

        setFacingDirection(RIGHT);

        ticks = 0;

        return;
      }

      if (ticks < 5) {

        ++ticks;

        return;
      }

      if (within_boundaries) {

        if (x / w == map_width - 1) {

          return;
        }

        if (map_tiles[(x / w) + 1][(y / h)].substr(5, 6) == "S") {

          return;
        }
      }

      is_moving = true;

      moving[UP] = false;
      moving[DOWN] = false;
      moving[LEFT] = false;
      moving[RIGHT] = true;
    }
  }
}

void Player::update(void) {

  static bool win_the_game = false;

  if (!win_the_game) {

    if (snow_percent >= 0.005) {

      snow_walk = true;

      show_dialog = true;

      int total_time = al_current_time();

      message_count = 0;

      messages = {

        "Congratulations! You did it!",
        "You reached the snow goal!",
        "It took you " + to_string(total_time) + " seconds.",
        "Your prize is snowball feet! Try it!",
        "Move around and watch the snow!"
      };
      message_cap = messages.size();

      al_play_sample(powerup_sample, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

      win_the_game = true;
    }
  }

  for (unsigned int i = 0; i < item_count.size(); ++i) {

    if (item_count[i] > 64) {

      // Can't hold more than 64 items.
      item_count[i] = 64;
    }
  }

  if (show_dialog) {

    // Don't move during dialog.
    return;
  }

  if (is_moving) {

    if (moving[UP]) {

      moveUp();
    }
    else if (moving[DOWN]) {

      moveDown();
    }
    else if (moving[LEFT]) {

      moveLeft();
    }
    else if (moving[RIGHT]) {

      moveRight();
    }
  }
}

void Player::render(const float camera_x, const float camera_y) {

  int dir = 0;

  for (int i = 0; i < 4; ++i) {

    if (facing[i]) {

      dir = i;
    }
  }

  float px = x - camera_x;
  float py = y - camera_y;

  al_draw_bitmap_region(bitmap, w * dir, 0, w, h, px, py, 0);

  //al_draw_bitmap(hud, 0, 8 * 2 * 6, 0);

  al_hold_bitmap_drawing(true);

  for (int i = 0; i < 12; ++i) {

    // Draw the HUD background.
    al_draw_bitmap_region(hud, 0, 0, 16, 16, 16 * i, 16 * 6, 0);
  }

  al_hold_bitmap_drawing(false);

  //al_draw_bitmap_region(hud, 8 * 16, 0, 8 * 16, 16, 0 + 2, 0 + 2, 0);

  // Draw the HUD item highlighter.
  al_draw_bitmap_region(hud, 16, 0, 16, 16, 16 * current_item, 16 * 6, 0);

  al_hold_bitmap_drawing(true);

  for (int i = 0; i < 12; ++i) {

    // Draw the HUD items.
    al_draw_bitmap_region(hud, 32 + (16 * i), 0, 16 + 32 + (16 * i), 16, 16 * i, 16 * 6, 0);
  }

  al_hold_bitmap_drawing(false);

  float scale = 2.0;

  ALLEGRO_TRANSFORM transform;

  al_identity_transform(&transform);

  al_scale_transform(&transform, scale, scale);

  al_use_transform(&transform);

  static ALLEGRO_COLOR normal = al_map_rgb(234, 248, 246);
  static ALLEGRO_COLOR different = al_map_rgb(179, 179, 179);

  // Draw snow percent.
  al_draw_textf(builtin, al_map_rgb(76, 89, 95), 9, 9, 0, "Snow goal: %i%%", int((snow_percent / 0.005) * 100));
  al_draw_textf(builtin, normal, 8, 8, 0, "Snow goal: %i%%", int((snow_percent / 0.005) * 100));

  if (item_count[1] == 0 || item_count[1] == 64) {

    al_draw_textf(builtin, different, 30 * 2 + 1, 16 * 6 * 2 + 23, ALLEGRO_ALIGN_CENTER, "%i", item_count[1]);
  }
  else {

    al_draw_textf(builtin, normal, 30 * 2 + 1, 16 * 6 * 2 + 23, ALLEGRO_ALIGN_CENTER, "%i", item_count[1]);
  }

  if (item_count[2] == 0 || item_count[2] == 64) {

    al_draw_textf(builtin, different, 30 * 3 + 3, 16 * 6 * 2 + 23, ALLEGRO_ALIGN_CENTER, "%i", item_count[2]);
  }
  else {

    al_draw_textf(builtin, normal, 30 * 3 + 3, 16 * 6 * 2 + 23, ALLEGRO_ALIGN_CENTER, "%i", item_count[2]);
  }

  if (item_count[3] == 0 || item_count[3] == 64) {

    al_draw_textf(builtin, different, 30 * 4 + 5, 16 * 6 * 2 + 23, ALLEGRO_ALIGN_CENTER, "%i", item_count[3]);
  }
  else {

    al_draw_textf(builtin, normal, 30 * 4 + 5, 16 * 6 * 2 + 23, ALLEGRO_ALIGN_CENTER, "%i", item_count[3]);
  }

  if (item_count[4] == 0 || item_count[4] == 64) {

    al_draw_textf(builtin, different, 30 * 5 + 7, 16 * 6 * 2 + 23, ALLEGRO_ALIGN_CENTER, "%i", item_count[4]);
  }
  else {

    al_draw_textf(builtin, normal, 30 * 5 + 7, 16 * 6 * 2 + 23, ALLEGRO_ALIGN_CENTER, "%i", item_count[4]);
  }

  if (item_count[5] == 0 || item_count[5] == 64) {

    al_draw_textf(builtin, different, 30 * 6 + 9, 16 * 6 * 2 + 23, ALLEGRO_ALIGN_CENTER, "%i", item_count[5]);
  }
  else {

    al_draw_textf(builtin, normal, 30 * 6 + 9, 16 * 6 * 2 + 23, ALLEGRO_ALIGN_CENTER, "%i", item_count[5]);
  }

  if (item_count[6] == 0 || item_count[6] == 64) {

    al_draw_textf(builtin, different, 30 * 7 + 11, 16 * 6 * 2 + 23, ALLEGRO_ALIGN_CENTER, "%i", item_count[6]);
  }
  else {

    al_draw_textf(builtin, normal, 30 * 7 + 11, 16 * 6 * 2 + 23, ALLEGRO_ALIGN_CENTER, "%i", item_count[6]);
  }

  scale = 4.0;

  al_identity_transform(&transform);

  al_scale_transform(&transform, scale, scale);

  al_use_transform(&transform);

  if (show_dialog) {

    al_draw_filled_rectangle(8 * 2, 8 * 2, 8 * 2 * 11, 8 * 2 * 2, al_map_rgb(59, 44, 44));

    scale = 2.0;

    al_identity_transform(&transform);

    al_scale_transform(&transform, scale, scale);

    al_use_transform(&transform);

    if (use_japanese) {

      al_draw_textf(japanese, al_map_rgb(255, 255, 255), 8 * 5, 8 * 5 + 4, 0, "%s", messages[message_count].c_str());
    }
    else {

      al_draw_textf(builtin, al_map_rgb(255, 255, 255), 8 * 5, 8 * 5 + 4, 0, "%s", messages[message_count].c_str());
    }

    scale = 4.0;

    al_identity_transform(&transform);

    al_scale_transform(&transform, scale, scale);

    al_use_transform(&transform);
  }
}

void Player::setSpawn(const int map_wi, const int map_he) {

  map_width = map_wi;
  map_height = map_he;

  x = (rand() % map_wi) * w;
  y = (rand() % map_he) * h;

  if (x == w * 0) {

    x = w * 1;
  }
  else if (x == map_wi * w - w) {

    x = map_wi * w - w * 2;
  }

  if (y == h * 0) {

    y = h * 1;
  }
  else if (y == map_he * h - h) {

    y = map_he * h - h * 2;
  }

  int facing_direction = rand() % 4;

  setFacingDirection(facing_direction);
}

void Player::useItem(vector < vector < string > > &t) {

  if (show_dialog) {

    ++message_count;

    if (message_count >= int(messages.size())) {

      show_dialog = false;

      for (unsigned int i = 0; i < messages.size(); ++i) {

        if (use_japanese) {

          use_japanese = false;
        }

        // Clear messages.
        messages[i].clear();
      }
    }

    return;
  }

  if (is_moving) {

    return;
  }

  switch (current_item) {

    case 0:

      // Using interactive item.
      // in, a
      // Item number used, add item, decrement this item, tile original, tile replace, sample, tiles.

      int item;

      int r;
      r = rand() % 5;

      switch (r) {

        case 0:

          // Acorn.
          item = 1;
        break;

        case 1:

          // Acorn.
          item = 1;
        break;

        case 3:

          // Stick.
          item = 5;
        break;

        case 4:

          // Stick.
          item = 5;
        break;

        default:

          // Acorn AND stick!
          item = 77;
        break;
      }

      convertTile(0, item, false, "02x00S", "00x00L", hit_sample, t); // Dead tree.
      convertTile(0, 0, false, "01x00S", "02x00S", hit_sample, t); // Healthy tree.
      convertTile(0, 2, false, "03x00S", "00x00L", hit_sample, t); // Mushroom.
      convertTile(0, 3, false, "00x01S", "00x00L", hit_sample, t); // Bone.
      convertTile(0, 4, false, "01x01L", "00x00L", hit_sample, t); // Flower.
      convertTile(0, 1, false, "03x01S", "00x00L", hit_sample, t); // Acorn.
      convertTile(0, 5, false, "00x03S", "00x00L", hit_sample, t); // Stick.
      convertTile(0, 6, false, "01x03L", "00x00L", hit_sample, t); // Snowball.

      if (!show_dialog) {

        message_count = 0;

        if (t[int(x / w)][int(y / h) - 1] == "01x02S") {

          if (facing[UP]) {

            show_dialog = true;

            if (names[int(x / w)][int(y / h) - 1] == "???") {

              setNPCName();
              setNPCItem();

              string count = to_string(items_count[int(x / w)][int(y / h) - 1]);

              string wanted = to_string(items_wanted[int(x / w)][int(y / h) - 1]);

              switch (items_wanted[int(x / w)][int(y / h) - 1]) {

                case 1:

                  wanted = "acorns";
                break;

                case 2:

                  wanted = "mushrooms";
                break;

                case 3:

                  wanted = "bones";
                break;

                case 4:

                  wanted = "flowers";
                break;

                case 5:

                  wanted = "sticks";
                break;

                case 6:

                  wanted = "snowballs";
                break;
              }

              messages = {

                "Hi there. I'm " + names[int(x / w)][int(y / h) - 1] + ".",
                "I have a snow-maker in my soul!",
                "Want snowballs? Bring me items!",
                "I want " + wanted + "! Bring me " + count + "!",
                "Approach me with the item selected..."
              };

              return;
            }

            if (snow_walk) {

              messages = {

                "Thanks for covering the world in snow!"
              };

              return;
            }

            switch (rand() % 7) {

              case 0:

                messages = {

                  "I love haikus. I'll recite one now.",
                  "Over-ripe sushi, the Master...",
                  "... is full of regret.",
                  "That was by Yosa Buson. Cool, eh?"
                };
              break;

              case 1:

                //
                messages = {

                  "Listen to my joke! So a vegan...",
                  "... a Jew, and a dog visit a bar...",
                  "... they have a great time!",
                  "HAHAHAHAHAHAHAHAHAHAHAHAHA!!!",
                  "Wait... why aren't you laughing?",
                  "..."
                };
              break;

              case 2:

                messages = {

                  "Hey! Listen to this haiku:",
                  "The wren earns his living...",
                  "... noiselessly.",
                  "That one was by Kobayashi Issa!"
                };
              break;

              case 3:

                messages = {

                  "That which is below...",
                  "... is like that which is above.",
                  "Likewise, that which is above...",
                  "... is like that which is below.",
                  "That's from the Emerald Tablet.",
                  "Alchemy rocks!"
                };
              break;

              case 4:

                use_japanese = true;

                messages = {

                  // 昨日、わたしのくるまがジャイアントなねこにたべられた！
                  // Yesterday, my car was eaten by a giant cat!
                  // Had to skim most kanji here, as the font was too small for it.
                  // Also, I used わたし instead of ぼく, because the NPC might have a female name.
                  "\u6628\u65e5\u3001\u308f\u305f\u3057\u306e\u304f\u308b\u307e\u304c\u30b8\u30e3\u30a4\u30a2\u30f3\u30c8\u306a\u306d\u3053\u306b\u305f\u3079\u3089\u308c\u305f\uff01",

                  // けっきょく、いのちはもくてきといみがない・・・
                  // In the end, life has neither meaning nor purpose...
                  "\u3051\u3063\u304d\u3087\u304f\u3001\u3044\u306e\u3061\u306f\u3082\u304f\u3066\u304d\u3068\u3044\u307f\u304c\u306a\u3044\u30fb\u30fb\u30fb",

                  // きみは日本語をよめるか？
                  // Can you read Japanese?
                  "\u304d\u307f\u306f\u65e5\u672c\u8a9e\u3092\u3088\u3081\u308b\u304b\uff1f"
                };
              break;

              case 5:

                messages = {

                  "You know, we die when we make snow..."
                };
              break;

              case 6:

                messages = {

                  "You should strive to reach your goals!"
                };
              break;
            }

            message_cap = messages.size();
          }
        }
        else if (t[int(x / w)][int(y / h) + 1] == "00x02S") {

          if (facing[DOWN]) {

            show_dialog = true;

            if (names[int(x / w)][int(y / h) + 1] == "???") {

              setNPCName();
              setNPCItem();

              string count = to_string(items_count[int(x / w)][int(y / h) + 1]);

              string wanted = to_string(items_wanted[int(x / w)][int(y / h) -+1]);

              switch (items_wanted[int(x / w)][int(y / h) + 1]) {

                case 1:

                  wanted = "acorns";
                break;

                case 2:

                  wanted = "mushrooms";
                break;

                case 3:

                  wanted = "bones";
                break;

                case 4:

                  wanted = "flowers";
                break;

                case 5:

                  wanted = "sticks";
                break;

                case 6:

                  wanted = "snowballs";
                break;
              }

              messages = {

                "Hi there. I'm " + names[int(x / w)][int(y / h) + 1] + ".",
                "I have a snow-maker in my soul!",
                "Want snowballs? Bring me items!",
                "I want " + wanted + "! Bring me " + count + "!",
                "Approach me with the item selected..."
              };

              return;
            }

            if (snow_walk) {

              messages = {

                "Thanks for covering the world in snow!"
              };

              return;
            }

            switch (rand() % 7) {

              case 0:

                messages = {

                  "I love haikus. I'll recite one now.",
                  "Over-ripe sushi, the Master...",
                  "... is full of regret.",
                  "That was by Yosa Buson. Cool, eh?"
                };
              break;

              case 1:

                //
                messages = {

                  "Listen to my joke! So a vegan...",
                  "... a Jew, and a dog visit a bar...",
                  "... they have a great time!",
                  "HAHAHAHAHAHAHAHAHAHAHAHAHA!!!",
                  "Wait... why aren't you laughing?",
                  "..."
                };
              break;

              case 2:

                messages = {

                  "Hey! Listen to this haiku:",
                  "The wren earns his living...",
                  "... noiselessly.",
                  "That one was by Kobayashi Issa!"
                };
              break;

              case 3:

                messages = {

                  "That which is below...",
                  "... is like that which is above.",
                  "Likewise, that which is above...",
                  "... is like that which is below.",
                  "That's from the Emerald Tablet.",
                  "Alchemy rocks!"
                };
              break;

              case 4:

                use_japanese = true;

                messages = {

                  // 昨日、わたしのくるまがジャイアントなねこにたべられた！
                  // Yesterday, my car was eaten by a giant cat!
                  // Had to skim most kanji here, as the font was too small for it.
                  // Also, I used わたし instead of ぼく, because the NPC might have a female name.
                  "\u6628\u65e5\u3001\u308f\u305f\u3057\u306e\u304f\u308b\u307e\u304c\u30b8\u30e3\u30a4\u30a2\u30f3\u30c8\u306a\u306d\u3053\u306b\u305f\u3079\u3089\u308c\u305f\uff01",

                  // けっきょく、いのちはもくてきといみがない・・・
                  // In the end, life has neither meaning nor purpose...
                  "\u3051\u3063\u304d\u3087\u304f\u3001\u3044\u306e\u3061\u306f\u3082\u304f\u3066\u304d\u3068\u3044\u307f\u304c\u306a\u3044\u30fb\u30fb\u30fb",

                  // きみは日本語をよめるか？
                  // Can you read Japanese?
                  "\u304d\u307f\u306f\u65e5\u672c\u8a9e\u3092\u3088\u3081\u308b\u304b\uff1f"
                };
              break;

              case 5:

                messages = {

                  "You know, we die when we make snow..."
                };
              break;

              case 6:

                messages = {

                  "You should strive to reach your goals!"
                };
              break;
            }

            message_cap = messages.size();
          }
        }
        else if (t[int(x / w) - 1][int(y / h)] == "03x02S") {

          if (facing[LEFT]) {

            show_dialog = true;

            if (names[int(x / w) - 1][int(y / h)] == "???") {

              setNPCName();
              setNPCItem();

              string count = to_string(items_count[int(x / w) - 1][int(y / h)]);

              string wanted = to_string(items_wanted[int(x / w) - 1][int(y / h)]);

              switch (items_wanted[int(x / w) - 1][int(y / h)]) {

                case 1:

                  wanted = "acorns";
                break;

                case 2:

                  wanted = "mushrooms";
                break;

                case 3:

                  wanted = "bones";
                break;

                case 4:

                  wanted = "flowers";
                break;

                case 5:

                  wanted = "sticks";
                break;

                case 6:

                  wanted = "snowballs";
                break;
              }

              messages = {

                "Hi there. I'm " + names[int(x / w) - 1][int(y / h)] + ".",
                "I have a snow-maker in my soul!",
                "Want snowballs? Bring me items!",
                "I want " + wanted + "! Bring me " + count + "!",
                "Approach me with the item selected..."
              };

              return;
            }

            if (snow_walk) {

              messages = {

                "Thanks for covering the world in snow!"
              };

              return;
            }

            switch (rand() % 7) {

              case 0:

                messages = {

                  "I love haikus. I'll recite one now.",
                  "Over-ripe sushi, the Master...",
                  "... is full of regret.",
                  "That was by Yosa Buson. Cool, eh?"
                };
              break;

              case 1:

                //
                messages = {

                  "Listen to my joke! So a vegan...",
                  "... a Jew, and a dog visit a bar...",
                  "... they have a great time!",
                  "HAHAHAHAHAHAHAHAHAHAHAHAHA!!!",
                  "Wait... why aren't you laughing?",
                  "..."
                };
              break;

              case 2:

                messages = {

                  "Hey! Listen to this haiku:",
                  "The wren earns his living...",
                  "... noiselessly.",
                  "That one was by Kobayashi Issa!"
                };
              break;

              case 3:

                messages = {

                  "That which is below...",
                  "... is like that which is above.",
                  "Likewise, that which is above...",
                  "... is like that which is below.",
                  "That's from the Emerald Tablet.",
                  "Alchemy rocks!"
                };
              break;

              case 4:

                use_japanese = true;

                messages = {

                  // 昨日、わたしのくるまがジャイアントなねこにたべられた！
                  // Yesterday, my car was eaten by a giant cat!
                  // Had to skim most kanji here, as the font was too small for it.
                  // Also, I used わたし instead of ぼく, because the NPC might have a female name.
                  "\u6628\u65e5\u3001\u308f\u305f\u3057\u306e\u304f\u308b\u307e\u304c\u30b8\u30e3\u30a4\u30a2\u30f3\u30c8\u306a\u306d\u3053\u306b\u305f\u3079\u3089\u308c\u305f\uff01",

                  // けっきょく、いのちはもくてきといみがない・・・
                  // In the end, life has neither meaning nor purpose...
                  "\u3051\u3063\u304d\u3087\u304f\u3001\u3044\u306e\u3061\u306f\u3082\u304f\u3066\u304d\u3068\u3044\u307f\u304c\u306a\u3044\u30fb\u30fb\u30fb",

                  // きみは日本語をよめるか？
                  // Can you read Japanese?
                  "\u304d\u307f\u306f\u65e5\u672c\u8a9e\u3092\u3088\u3081\u308b\u304b\uff1f"
                };
              break;

              case 5:

                messages = {

                  "You know, we die when we make snow..."
                };
              break;

              case 6:

                messages = {

                  "You should strive to reach your goals!"
                };
              break;
            }

            message_cap = messages.size();
          }
        }
        else if (t[int(x / w) + 1][int(y / h)] == "02x02S") {

          if (facing[RIGHT]) {

            show_dialog = true;

            if (names[int(x / w) + 1][int(y / h)] == "???") {

              setNPCName();
              setNPCItem();

              string count = to_string(items_count[int(x / w) + 1][int(y / h)]);

              string wanted = to_string(items_wanted[int(x / w) + 1][int(y / h)]);

              switch (items_wanted[int(x / w) + 1][int(y / h)]) {

                case 1:

                  wanted = "acorns";
                break;

                case 2:

                  wanted = "mushrooms";
                break;

                case 3:

                  wanted = "bones";
                break;

                case 4:

                  wanted = "flowers";
                break;

                case 5:

                  wanted = "sticks";
                break;

                case 6:

                  wanted = "snowballs";
                break;
              }

              messages = {

                "Hi there. I'm " + names[int(x / w) + 1][int(y / h)] + ".",
                "I have a snow-maker in my soul!",
                "Want snowballs? Bring me items!",
                "I want " + wanted + "! Bring me " + count + "!",
                "Approach me with the item selected..."
              };

              return;
            }

            if (snow_walk) {

              messages = {

                "Thanks for covering the world in snow!"
              };

              return;
            }

            switch (rand() % 7) {

              case 0:

                messages = {

                  "I love haikus. I'll recite one now.",
                  "Over-ripe sushi, the Master...",
                  "... is full of regret.",
                  "That was by Yosa Buson. Cool, eh?"
                };
              break;

              case 1:

                //
                messages = {

                  "Listen to my joke! So a vegan...",
                  "... a Jew, and a dog visit a bar...",
                  "... they have a great time!",
                  "HAHAHAHAHAHAHAHAHAHAHAHAHA!!!",
                  "Wait... why aren't you laughing?",
                  "..."
                };
              break;

              case 2:

                messages = {

                  "Hey! Listen to this haiku:",
                  "The wren earns his living...",
                  "... noiselessly.",
                  "That one was by Kobayashi Issa!"
                };
              break;

              case 3:

                messages = {

                  "That which is below...",
                  "... is like that which is above.",
                  "Likewise, that which is above...",
                  "... is like that which is below.",
                  "That's from the Emerald Tablet.",
                  "Alchemy rocks!"
                };
              break;

              case 4:

                use_japanese = true;

                messages = {

                  // 昨日、わたしのくるまがジャイアントなねこにたべられた！
                  // Yesterday, my car was eaten by a giant cat!
                  // Had to skim most kanji here, as the font was too small for it.
                  // Also, I used わたし instead of ぼく, because the NPC might have a female name.
                  "\u6628\u65e5\u3001\u308f\u305f\u3057\u306e\u304f\u308b\u307e\u304c\u30b8\u30e3\u30a4\u30a2\u30f3\u30c8\u306a\u306d\u3053\u306b\u305f\u3079\u3089\u308c\u305f\uff01",

                  // けっきょく、いのちはもくてきといみがない・・・
                  // In the end, life has neither meaning nor purpose...
                  "\u3051\u3063\u304d\u3087\u304f\u3001\u3044\u306e\u3061\u306f\u3082\u304f\u3066\u304d\u3068\u3044\u307f\u304c\u306a\u3044\u30fb\u30fb\u30fb",

                  // きみは日本語をよめるか？
                  // Can you read Japanese?
                  "\u304d\u307f\u306f\u65e5\u672c\u8a9e\u3092\u3088\u3081\u308b\u304b\uff1f"
                };
              break;

              case 5:

                messages = {

                  "You know, we die when we make snow..."
                };
              break;

              case 6:

                messages = {

                  "You should strive to reach your goals!"
                };
              break;
            }

            message_cap = messages.size();
          }
        }
      }
    break;

    case 1:

      // Using acorn item.
      convertTile(1, 0, true, "00x00L", "03x01S", plant_sample, t);
    break;

    case 2:

      // Using mushroom item.
      convertTile(2, 0, true, "00x00L", "03x00S", plant_sample, t);
    break;

    case 3:

      // Using bone item.
      convertTile(3, 0, true, "00x00L", "00x01S", plant_sample, t);
    break;

    case 4:

      // Using flower item.
      convertTile(4, 0, true, "00x00L", "01x01L", plant_sample, t);
    break;

    case 5:

      // Using stick item.
      convertTile(5, 0, true, "00x00L", "00x03S", plant_sample, t);
    break;

    case 6:

      // Using snowball item.
      convertTile(6, 0, true, "00x00L", "01x03L", plant_sample, t);
    break;

    case 11:

      static int times = 0;

      if (times > 3) {

        times = 0;

        al_play_sample(boom_sample, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

        for (unsigned int y = 0; y < t[0].size(); ++y) {

          for (unsigned int x = 0; x < t.size(); ++x) {

            if (t[x][y] == "01x00S") {

              // Convert healthy trees to acorns. This is a secret!
              t[x][y] = "03x01S";
            }
          }
        }
      }

      ++times;
    break;
  }

  if (current_item == 0) {

    return;
  }

  if (current_item == items_wanted[int(x / w)][int(y / h) - 1]) {

    if (facing[UP]) {

      string wanted;

      switch (items_wanted[int(x / w)][int(y / h) - 1]) {

        case 1:

          wanted = "acorns";
        break;

        case 2:

          wanted = "mushrooms";
        break;

        case 3:

          wanted = "bones";
        break;

        case 4:

          wanted = "flowers";
        break;

        case 5:

          wanted = "sticks";
        break;

        case 6:

          wanted = "snowballs";
        break;
      }

      // Does the player have ample amount to trade?
      if (item_count[current_item] < items_count[int(x / w)][int(y / h) - 1]) {

        message_count = 0;

        show_dialog = true;

        messages = {

          "You don't have enough " + wanted + "!"
        };

        message_cap = messages.size();
      }
      else {

        message_count = 0;

        show_dialog = true;

        messages = {

          "Thanks for the " + wanted + "!",
          "I give my life for you!",
          "Enjoy the snowballs!"
        };

        al_play_sample(boom_sample, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

        // Kill the forest folk and replace it with a bone.
        t[int(x / w)][int(y / h) - 1] = "00x01S";

        // Subtracted difference.
        item_count[current_item] -= items_count[int(x / w)][int(y / h) - 1];

        // Add snowballs to inventory.
        item_count[6] += 2 + (rand() % 8);

        // Reset desired item (as the NPC no longer exists).
        items_wanted[int(x / w)][int(y / h) - 1] = 0;

        message_cap = messages.size();
      }
    }
  }
  else if (current_item == items_wanted[int(x / w)][int(y / h) + 1]) {

    if (facing[DOWN]) {

      string wanted;

      switch (items_wanted[int(x / w)][int(y / h) + 1]) {

        case 1:

          wanted = "acorns";
        break;

        case 2:

          wanted = "mushrooms";
        break;

        case 3:

          wanted = "bones";
        break;

        case 4:

          wanted = "flowers";
        break;

        case 5:

          wanted = "sticks";
        break;

        case 6:

          wanted = "snowballs";
        break;
      }

      // Does the player have ample amount to trade?
      if (item_count[current_item] < items_count[int(x / w)][int(y / h) + 1]) {

        message_count = 0;

        show_dialog = true;

        messages = {

          "You don't have enough " + wanted + "!"
        };

        message_cap = messages.size();
      }
      else {

        message_count = 0;

        show_dialog = true;

        messages = {

          "Thanks for the " + wanted + "!",
          "I give my life for you!",
          "Enjoy the snowballs!"
        };

        al_play_sample(boom_sample, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

        // Kill the forest folk and replace it with a bone.
        t[int(x / w)][int(y / h) + 1] = "00x01S";

        // Subtracted difference.
        item_count[current_item] -= items_count[int(x / w)][int(y / h) + 1];

        // Add snowballs to inventory.
        item_count[6] += 2 + (rand() % 8);

        // Reset desired item (as the NPC no longer exists).
        items_wanted[int(x / w)][int(y / h) + 1] = 0;

        message_cap = messages.size();
      }
    }
  }
  else if (current_item == items_wanted[int(x / w) + 1][int(y / h)]) {

    if (facing[RIGHT]) {

      string wanted;

      switch (items_wanted[int(x / w) + 1][int(y / h)]) {

        case 1:

          wanted = "acorns";
        break;

        case 2:

          wanted = "mushrooms";
        break;

        case 3:

          wanted = "bones";
        break;

        case 4:

          wanted = "flowers";
        break;

        case 5:

          wanted = "sticks";
        break;

        case 6:

          wanted = "snowballs";
        break;
      }

      // Does the player have ample amount to trade?
      if (item_count[current_item] < items_count[int(x / w) + 1][int(y / h)]) {

        message_count = 0;

        show_dialog = true;

        messages = {

          "You don't have enough " + wanted + "!"
        };

        message_cap = messages.size();
      }
      else {

        message_count = 0;

        show_dialog = true;

        messages = {

          "Thanks for the " + wanted + "!",
          "I give my life for you!",
          "Enjoy the snowballs!"
        };

        al_play_sample(boom_sample, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

        // Kill the forest folk and replace it with a bone.
        t[int(x / w) + 1][int(y / h)] = "00x01S";

        // Subtracted difference.
        item_count[current_item] -= items_count[int(x / w) + 1][int(y / h)];

        // Add snowballs to inventory.
        item_count[6] += 2 + (rand() % 8);

        // Reset desired item (as the NPC no longer exists).
        items_wanted[int(x / w) + 1][int(y / h)] = 0;

        message_cap = messages.size();
      }
    }
  }
  else if (current_item == items_wanted[int(x / w) - 1][int(y / h)]) {

    if (facing[LEFT]) {

      string wanted;

      switch (items_wanted[int(x / w) - 1][int(y / h)]) {

        case 1:

          wanted = "acorns";
        break;

        case 2:

          wanted = "mushrooms";
        break;

        case 3:

          wanted = "bones";
        break;

        case 4:

          wanted = "flowers";
        break;

        case 5:

          wanted = "sticks";
        break;

        case 6:

          wanted = "snowballs";
        break;
      }

      // Does the player have ample amount to trade?
      if (item_count[current_item] < items_count[int(x / w) - 1][int(y / h)]) {

        message_count = 0;

        show_dialog = true;

        messages = {

          "You don't have enough " + wanted + "!"
        };

        message_cap = messages.size();
      }
      else {

        message_count = 0;

        show_dialog = true;

        messages = {

          "Thanks for the " + wanted + "!",
          "I give my life for you!",
          "Enjoy the snowballs!"
        };

        al_play_sample(boom_sample, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

        // Kill the forest folk and replace it with a bone.
        t[int(x / w) - 1][int(y / h)] = "00x01S";

        // Subtracted difference.
        item_count[current_item] -= items_count[int(x / w) - 1][int(y / h)];

        // Add snowballs to inventory.
        item_count[6] += 2 + (rand() % 8);

        // Reset desired item (as the NPC no longer exists).
        items_wanted[int(x / w) - 1][int(y / h)] = 0;

        message_cap = messages.size();
      }
    }
  }
  else {

    if (current_item > 6) {

      // Don't interact with the empty slots.
      return;
    }

    if (facing[UP] && t[int(x / w)][int(y / h) - 1] == "01x02S") {

      if (current_item != items_wanted[int(x / w)][int(y / h) - 1]) {

        show_dialog = true;

        message_count = 0;

        messages = {

          "I don't want that junk!"
        };

        message_cap = messages.size();
      }
    }

    if (facing[DOWN] && t[int(x / w)][int(y / h) + 1] == "00x02S") {

      if (current_item != items_wanted[int(x / w)][int(y / h) + 1]) {

        show_dialog = true;

        message_count = 0;

        messages = {

          "I don't want that junk!"
        };

        message_cap = messages.size();
      }
    }

    if (facing[LEFT] && t[int(x / w) - 1][int(y / h)] == "03x02S") {

      if (current_item != items_wanted[int(x / w) - 1][int(y / h)]) {

        show_dialog = true;

        message_count = 0;

        messages = {

          "I don't want that junk!"
        };

        message_cap = messages.size();
      }
    }

    if (facing[RIGHT] && t[int(x / w) + 1][int(y / h)] == "02x02S") {

      if (current_item != items_wanted[int(x / w) + 1][int(y / h)]) {

        show_dialog = true;

        message_count = 0;

        messages = {

          "I don't want that junk!"
        };

        message_cap = messages.size();
      }
    }
  }
}

void Player::switchItem(const int pol) {

  if (show_dialog) {

    // Don't allow item switching during dialog.
    return;
  }

  al_play_sample(blip_sample, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

  if (pol == 0) {

    --current_item;
  }
  else if (pol == 1) {

    ++current_item;
  }

  if (current_item > 11) {

    current_item = 0;
  }
  else if (current_item < 0) {

    current_item = 11;
  }
}

void Player::setMapWidth(const int width) {

  map_width = width;
}

void Player::setMapHeight(const int height) {

  map_height = height;
}

void Player::destroyBitmap(void) {

  al_destroy_bitmap(bitmap);

  al_destroy_font(builtin);
  al_destroy_font(japanese);
}

void Player::destroySamples(void) {

  al_destroy_sample(hit_sample);
  al_destroy_sample(plant_sample);
  al_destroy_sample(blip_sample);
  al_destroy_sample(blip2_sample);
  al_destroy_sample(death_sample);
  al_destroy_sample(boom_sample);
  al_destroy_sample(powerup_sample);
}

void Player::moveUp(void) {

  static int start_tile = y / h;

  static int stop_tile = start_tile - 1;

  if (start_tile == 0) {

    start_tile = y / h;
  }

  if (stop_tile == 0) {

    stop_tile = start_tile - 1;
  }

  if (y / h > stop_tile) {

    y -= speed;
  }
  else {

    is_moving = false;

    stop_tile = 0;
    start_tile = 0;
  }
}

void Player::moveDown(void) {

  static int start_tile = y / h;

  static int stop_tile = start_tile + 1;

  if (start_tile == 0) {

    start_tile = y / h;
  }

  if (stop_tile == 0) {

    stop_tile = start_tile + 1;
  }

  if (y / h < stop_tile) {

    y += speed;
  }
  else {

    is_moving = false;

    stop_tile = 0;
    start_tile = 0;
  }
}

void Player::moveLeft(void) {

  static int start_tile = x / w;

  static int stop_tile = start_tile - 1;

  if (start_tile == 0) {

    start_tile = x / w;
  }

  if (stop_tile == 0) {

    stop_tile = start_tile - 1;
  }

  if (x / w > stop_tile) {

    x -= speed;
  }
  else {

    is_moving = false;

    stop_tile = 0;
    start_tile = 0;
  }
}

void Player::moveRight(void) {

  static int start_tile = x / w;

  static int stop_tile = start_tile + 1;

  if (start_tile == 0) {

    start_tile = x / w;
  }

  if (stop_tile == 0) {

    stop_tile = start_tile + 1;
  }

  if (x / w < stop_tile) {

    x += speed;
  }
  else {

    is_moving = false;

    stop_tile = 0;
    start_tile = 0;
  }
}

void Player::setFacingDirection(const int direction) {

  for (int i = 0; i < 4; ++i) {

    facing[i] = false;
  }

  facing[direction] = true;
}

void Player::convertTile(int in, int a, bool d, string s, string e, ALLEGRO_SAMPLE *sa, vector<vector<string>> &t) {

  if (item_count[in] < 1) {

    al_play_sample(blip2_sample, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

    // Player has none of these items.
    return;
  }

  if (facing[UP]) {

    if (t[int(x / w)][int(y / h) - 1] == s) {

      al_play_sample(sa, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

      t[int(x / w)][int(y / h) - 1] = e;

      if (a == 77) {

        // Give acorn AND stick!
        ++item_count[1];
        ++item_count[5];
      }
      else {

        ++item_count[a];
      }

      if (d) {

        --item_count[in];
      }
    }
  }
  else if (facing[DOWN]) {

    if (t[int(x / w)][int(y / h) + 1] == s) {

      al_play_sample(sa, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

      t[int(x / w)][int(y / h) + 1] = e;

      if (a == 77) {

        // Give acorn AND stick!
        ++item_count[1];
        ++item_count[5];
      }
      else {

        ++item_count[a];
      }

      if (d) {

        --item_count[in];
      }
    }
  }
  else if (facing[LEFT]) {

    if (t[int(x / w) - 1][int(y / h)] == s) {

      al_play_sample(sa, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

      t[int(x / w) - 1][int(y / h)] = e;

      if (a == 77) {

        // Give acorn AND stick!
        ++item_count[1];
        ++item_count[5];
      }
      else {

        ++item_count[a];
      }

      if (d) {

        --item_count[in];
      }
    }
  }
  else if (facing[RIGHT]) {

    if (t[int(x / w) + 1][int(y / h)] == s) {

      al_play_sample(sa, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

      t[int(x / w) + 1][int(y / h)] = e;

      if (a == 77) {

        // Give acorn AND stick!
        ++item_count[1];
        ++item_count[5];
      }
      else {

        ++item_count[a];
      }

      if (d) {

        --item_count[in];
      }
    }
  }
}

void Player::setNPCName(void) {

  static vector < string > n = {

    "Eric",
    "Zach",
    "Emily",
    "Lisa",
    "Bill",
    "Rolf",
    "Trent",
    "Brad",
    "Jim",
    "Rod",
    "Megan",
    "Susan",
    "Sally",
    "Pablo",
    "Cherri",
    "David",
    "Sadie",
    "Keebo",
    "Josh",
    "Drake",
    "Karen",
    "Death"
  };

  if (names[int(x / w)][int(y / h) - 1] == "???") {

    names[int(x / w)][int(y / h) - 1] = n[rand() % n.size()];
  }

  if (names[int(x / w)][int(y / h) + 1] == "???") {

    names[int(x / w)][int(y / h) + 1] = n[rand() % n.size()];
  }

  if (names[int(x / w) - 1][int(y / h)] == "???") {

    names[int(x / w) - 1][int(y / h)] = n[rand() % n.size()];
  }

  if (names[int(x / w) + 1][int(y / h)] == "???") {

    names[int(x / w) + 1][int(y / h)] = n[rand() % n.size()];
  }
}

void Player::setNPCItem(void) {

  static vector < int > i = {

    1, // Acorn
    2, // Mushroom
    3, // Bone
    4, // Flower
    5, // Stick
    6 // Snowball
  };

  if (items_wanted[int(x / w)][int(y / h) - 1] == -1) {

    if (facing[UP]) {

      items_wanted[int(x / w)][int(y / h) - 1] = i[rand() % i.size()];
    }
  }

  if (items_wanted[int(x / w)][int(y / h) + 1] == -1) {

    if (facing[DOWN]) {

      items_wanted[int(x / w)][int(y / h) + 1] = i[rand() % i.size()];
    }
  }

  if (items_wanted[int(x / w) - 1][int(y / h)] == -1) {

    if (facing[LEFT]) {

      items_wanted[int(x / w) - 1][int(y / h)] = i[rand() % i.size()];
    }
  }

  if (items_wanted[int(x / w) + 1][int(y / h)] == -1) {

    if (facing[RIGHT]) {

      items_wanted[int(x / w) + 1][int(y / h)] = i[rand() % i.size()];
    }
  }

  // Also do quantity desired:

  if (items_count[int(x / w)][int(y / h) - 1] == 0) {

    items_count[int(x / w)][int(y / h) - 1] = 1 + rand() % 9;
  }

  if (items_count[int(x / w)][int(y / h) + 1] == 0) {

    items_count[int(x / w)][int(y / h) + 1] = 1 + rand() % 9;
  }

  if (items_count[int(x / w) - 1][int(y / h)] == 0) {

    items_count[int(x / w) - 1][int(y / h)] = 1 + rand() % 9;
  }

  if (items_count[int(x / w) + 1][int(y / h)] == 0) {

    items_count[int(x / w) + 1][int(y / h)] = 1 + rand() % 9;
  }
}
