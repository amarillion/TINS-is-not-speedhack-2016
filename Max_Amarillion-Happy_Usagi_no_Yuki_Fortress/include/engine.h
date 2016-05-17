#ifndef ENGINE_H
#define ENGINE_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include "input.h"
#include "settings.h"
#include <list>
#include "game.h"
#include "IContainer.h"

class Resources;

class Engine : public IContainer
{
private:
public:
	virtual int init() = 0; // call once during startup
	virtual void done() = 0;
	virtual Resources* getResources() = 0;

	static std::shared_ptr<Engine> newInstance();

	enum Msg { MSG_MAIN_MENU, MSG_POEM, MSG_QUIT_BUTTON, MSG_QUIT = 100, MSG_PLAY, MSG_TOGGLE_WINDOWED, MSG_TOGGLE_MUSIC, MSG_CREDITS };

};


#endif
