#ifndef TINS_INC_ENGINE_HPP
#define TINS_INC_ENGINE_HPP

#include <string>
#include <iostream>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>

using std::cout;
using std::string;

class Engine {

  public:

    Engine(void);

    int getScreenWidth(void);
    int getScreenHeight(void);

    bool isKey(const int);

    bool isRunning(void);

    bool isKeyPressed(void);
    bool isKeyReleased(void);

    bool isRenderPhase(void);
    bool isUpdatePhase(void);

    void flipPhase(void);

    void initialize(void);

    void startTimer(void);

    void checkReturn(const bool);

    void flipDisplay(void);

    void createEvents(void);
    void registerEvents(void);

    void reserveSamples(const int);

    void setWindowTitle(const string);

    void checkEventQueue(void);

    void setDisplayScale(const float);

    void setResourcePath(const string);

    void destroyResources(void);

    void manageWindowEvents(void);

    void setWindowDimensions(const int, const int);

  private:

    int window_w;
    int window_h;

    bool is_running;
    bool is_updating;

    string window_title;

    ALLEGRO_EVENT event;

    ALLEGRO_TIMER *timer;
    ALLEGRO_DISPLAY *display;
    ALLEGRO_EVENT_QUEUE *event_queue;
};

#endif
