#ifndef TINS_INC_GAME_HPP
#define TINS_INC_GAME_HPP

#include <iostream>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

using std::cout;

class Game {

  public:

    Game(void);

    void setSeed(const int);

    void initialize(void);

  private:

    int seed;

    bool keys[4];

    float scale;

    enum Keys {UP = 0, DOWN, LEFT, RIGHT};

    void loop(void);

    void loadResources(void);

    void destroyResources(void);

    ALLEGRO_FONT *builtin;
    ALLEGRO_FONT *japanese;
};

#endif
