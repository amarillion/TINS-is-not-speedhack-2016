#ifndef TINS_INC_PLAYER_HPP
#define TINS_INC_PLAYER_HPP

#include <ctime>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_primitives.h>

using std::max;
using std::cout;
using std::endl;
using std::time;
using std::string;
using std::vector;
using std::to_string;

class Player {

  public:

    Player(void);

    int getWidth(void);
    int getHeight(void);

    float getX(void);
    float getY(void);

    bool loadBitmap(void);
    bool loadSamples(void);

    void move(bool, bool, bool, bool, vector < vector < string > > &);

    void update(void);

    void render(const float, const float);

    void setSpawn(const int, const int);

    void useItem(vector < vector < string > > &);

    void switchItem(const int);

    void setMapTiles(vector < vector < string > > &);

    void setMapWidth(const int);
    void setMapHeight(const int);

    void destroyBitmap(void);
    void destroySamples(void);

    void detectCollision(vector < vector < string > >);

  private:

    int w;
    int h;

    float snow_percent;

    bool snow_walk;

    vector < int > item_count;

    int map_width;
    int map_height;

    float x;
    float y;

    float speed;

    bool use_japanese;

    bool spawned;
    bool show_dialog;

    int message_cap;
    int message_count;

    vector < string > messages;

    vector < vector < string > > names;

    vector < vector < int > > items_wanted;
    vector < vector < int > > items_owned;
    vector < vector < int > > items_count;

    bool die_after;

    int current_item;

    bool can_move;
    bool moving[4];
    bool is_moving;
    bool facing[4];
    bool dir_to_move[4];

    bool lock_controls;

    enum FACING_DIRECTIONS {UP = 0, DOWN, LEFT, RIGHT};

    ALLEGRO_BITMAP *bitmap;
    ALLEGRO_BITMAP *hud;

    ALLEGRO_FONT *builtin;
    ALLEGRO_FONT *japanese;

    ALLEGRO_SAMPLE *hit_sample;
    ALLEGRO_SAMPLE *plant_sample;
    ALLEGRO_SAMPLE *blip_sample;
    ALLEGRO_SAMPLE *blip2_sample;
    ALLEGRO_SAMPLE *death_sample;
    ALLEGRO_SAMPLE *boom_sample;
    ALLEGRO_SAMPLE *powerup_sample;

    void moveUp(void);
    void moveDown(void);
    void moveLeft(void);
    void moveRight(void);

    void setFacingDirection(const int);

    void convertTile(int, int, bool, string, string, ALLEGRO_SAMPLE *, vector < vector < string > > &);

    void setNPCName(void);

    void setNPCItem(void);
};

#endif
