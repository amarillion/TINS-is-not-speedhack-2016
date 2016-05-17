#ifndef TINS_INC_MAP_HPP
#define TINS_INC_MAP_HPP

#include <ctime>
#include <string>
#include <vector>
#include <iostream>

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>

using std::cin;
using std::cout;
using std::stoi;
using std::time;
using std::srand;
using std::string;
using std::vector;

class Map {

  public:

    Map(void);

    int getWidth(void);
    int getHeight(void);

    bool loadBitmap(void);
    bool loadSample(void);

    void render(const float, const float);

    void setSeed(const int);

    void loadTiles(void);

    void destroyBitmap(void);

    vector < vector < string > > &getTiles(void);

  private:

    int w;
    int h;

    int seed;

    int tile_w;
    int tile_h;

    int number_of_screens;

    vector < vector < int > > ticks;
    vector < vector < ALLEGRO_COLOR > > colors;

    vector < vector < string > > tiles;

    ALLEGRO_BITMAP *bitmap;

    ALLEGRO_SAMPLE *grow;

    int getRandomNumber(void);

    void generateTiles(const int, const string);
};

#endif
