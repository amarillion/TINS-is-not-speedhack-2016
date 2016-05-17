#include <inc/map.hpp>

Map::Map(void) {

  w = 0;
  h = 0;

  seed = 0;

  tile_w = 16;
  tile_h = 16;

  number_of_screens = 16;

  bitmap = NULL;

  grow = NULL;
}

int Map::getWidth(void) {

  return w;
}

int Map::getHeight(void) {

  return h;
}

bool Map::loadBitmap(void) {

  bitmap = al_load_bitmap("png/map.png");

  return bitmap;
}

bool Map::loadSample(void) {

  grow = al_load_sample("ogg/grow.ogg");

  return grow;
}

void Map::render(const float camera_x, const float camera_y) {

  al_hold_bitmap_drawing(true);

  for (int y = 0; y < h; ++y) {

    if (y * tile_h < camera_y - tile_h) {

      continue;
    }

    if (y * tile_h > camera_y + (tile_h * 6)) {

      continue;
    }

    for (int x = 0; x < w; ++x) {

      if (x * tile_w < camera_x - tile_w) {

        continue;
      }

      if (x * tile_w > camera_x + (tile_w * 12)) {

        continue;
      }

      int sx = stoi(tiles[x][y].substr(0, 2)) * tile_w;
      int sy = stoi(tiles[x][y].substr(3, 5)) * tile_h;

      // Draw a layer of grass first.
      al_draw_bitmap_region(

        bitmap, 0, 0, tile_w, tile_h, x * tile_w - camera_x, y * tile_h - camera_y, 0
      );

      al_draw_bitmap_region(

        bitmap, sx, sy, tile_w, tile_h, x * tile_w - camera_x, y * tile_h - camera_y, 0
      );

      if (tiles[x][y] == "00x02S" || tiles[x][y] == "01x02S" || tiles[x][y] == "02x02S" || tiles[x][y] == "03x02S") {

        // Draw tinted variant of NPC.
        al_draw_tinted_bitmap_region(

          bitmap, colors[x][y], sx, sy, tile_w, tile_h, x * tile_w - camera_x, y * tile_h - camera_y, 0
        );
      }
    }
  }

  for (unsigned int y = 0; y < tiles[0].size(); ++y) {

    for (unsigned int x = 0; x < tiles.size(); ++x) {

      if (tiles[x][y] == "03x01S") {

        --ticks[x][y];

        if (ticks[x][y] < 1) {

          // Conver the acorn to a tree.
          tiles[x][y] = "01x00S";

          al_play_sample(grow, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

          // Reset ticks for this tile.
          ticks[x][y] = 300 + (60 * (getRandomNumber() % 6));
        }
      }
    }
  }

  al_hold_bitmap_drawing(false);
}

void Map::setSeed(const int s) {

  seed = s;
}

void Map::loadTiles(void) {

  srand(seed);

  w = number_of_screens * 12;
  h = number_of_screens * 6;

  tiles.resize(w);
  ticks.resize(w);

  colors.resize(w);

  for (int y = 0; y < h; ++y) {

    for (int x = 0; x < w; ++x) {

      tiles[x].resize(h);

      ticks[x].resize(h);

      colors[x].resize(h);

      //colors[x][y] = al_map_rgba(getRandomNumber() % 255, getRandomNumber() % 255, getRandomNumber() % 255, getRandomNumber() % 64);

      colors[x][y] = al_map_rgba(255, 255, 255, 255);

      ticks[x][y] = 300 + (60 * (getRandomNumber() % 6));

      // Set all tiles initially to grass.
      tiles[x][y] = "00x00L";
    }
  }

  // Add borders around map to prevent player from leaving boundaries.
  for (int y = 0; y < h; ++y) {

    // Left
    tiles[0][y] = "99x99S";

    // Right
    tiles[w - 1][y] = "99x99S";

    for (int x = 0; x < w; ++x) {

      // Top
      tiles[x][0] = "99x99S";

      // Bottom
      tiles[x][h - 1] = "99x99S";
      tiles[x][h - 2] = "99x99S"; // For HUD overlay.
    }
  }

  // Generate mushrooms.
  generateTiles(45, "03x00S");

  // Generate trees.
  generateTiles(3, "01x00S");

  // Generate dead trees.
  //generateTiles(20, "02x00S");

  // Generate flowers.
  generateTiles(40, "01x01L");

  // Generate bones.
  //generateTiles(80, "00x01S");

  // Generate NPCs
  generateTiles(99, "01x02S");
}

void Map::destroyBitmap(void) {

  al_destroy_bitmap(bitmap);
}

vector < vector < string > > &Map::getTiles(void) {

  return tiles;
}

int Map::getRandomNumber(void) {

  seed = seed * 123456789 + 123456789;

  int shift = seed >> 16;

  if (shift < 0) {

    shift = -shift;
  }

  return shift;
}

void Map::generateTiles(const int probability, const string tile_data) {

  for (int y = 0; y < h; ++y) {

    for (int x = 0; x < w; ++x) {

      if (getRandomNumber() % probability == 1) {

        if (tiles[x][y] == "00x00L") {

          // Place new tile on top of grass tile.
          tiles[x][y] = tile_data;
        }
      }
    }
  }
}
