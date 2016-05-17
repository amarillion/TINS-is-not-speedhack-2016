#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <allegro5/allegro.h>
#include "component.h"
#include "sound.h"
#include "timer.h"

#include <string>
#include <vector>

/**
 * Equivalent of mainLoop->getw().
 * see there.
 * <p>
 * Represents the logical screen size, the optimal screen size for which the game was designed to run.
 * Buffer or display size may be different because they may have a transformation on them.
 * <p>
 * For example, the game may (conservatively) be designed for a 640x480 screen resolution.
 * However, the desktop resolution is 1920x1080, and this is the size you'd get for
 * the buffer bitmap width or the display width. Because the transformation applies, you'd draw
 * to the buffer as though it was sized 640x480.
 */
#define MAIN_WIDTH MainLoop::getMainLoop()->getw()

/**
 * Equivalent of mainLoop->geth().
 * See MAIN_WIDTH
 */
#define MAIN_HEIGHT MainLoop::getMainLoop()->geth()

class MainLoop : public IComponent, public Sound, public ITimer
{
private:
	ALLEGRO_BITMAP *buffer;
	ALLEGRO_EVENT_QUEUE *equeue;
	ALLEGRO_TIMER *timer;
	ALLEGRO_DISPLAY *display;

	std::shared_ptr<IComponent> engine;

	ALLEGRO_PATH *localAppData;
	ALLEGRO_PATH *configPath;

	const char *configFilename;
	const char *title;
	const char *appname;
	
	int frame_count;
	int frame_counter;
	int last_fps;
	int lastUpdateMsec;

	// game options

	enum ScreenMode { WINDOWED = 0, FULLSCREEN_WINDOW, FULLSCREEN };
	ScreenMode screenMode;
	bool stretch;

	// in smoke test mode: don't create display, just test loading resources.
	// smokeTest is in headless mode.
	bool smokeTest;
	
	void getFromConfig(ALLEGRO_CONFIG *config);
	void getFromArgs(int argc, const char *const *argv);

	std::vector<int> resX;
	std::vector<int> resY;
	int logicIntervalMsec;

	static MainLoop *instance;
	int initDisplay();
protected:
	ALLEGRO_CONFIG *config;
	bool fpsOn;
public:
	bool isSmokeTest() { return smokeTest; }
	void adjustMickey(int &x, int &y);
	std::vector<std::string> options;
	
	void addResolution (int w, int h);
	ALLEGRO_CONFIG *getConfig() { return config; }
	
	//TODO: DEPRECATED. use getMsecCounter
	int getCounter () { return al_get_timer_count(timer) * logicIntervalMsec; }
	int getMsecCounter () { return al_get_timer_count(timer) * logicIntervalMsec; }

	MainLoop (IComponent *_engine, const char *configFilename, const char *title, int _bufw = 640, int _bufh = 480);
	MainLoop ();

	MainLoop &setTitle(const char *_title);
	MainLoop &setAppName(const char *_appname);
	MainLoop &setConfigFilename(const char *_configFilename);
	MainLoop &setEngine(std::shared_ptr<IComponent> _engine);

	int init(int argc, const char *const *argv);
	void run();	
	virtual ~MainLoop();
	
	virtual int postInit() { return 0; };
	virtual void parseOpts(std::vector<std::string> &opts) {};
	void setLogicIntervalMsec (int value) { logicIntervalMsec = value; }
	int getLogicIntervalMsec () { return logicIntervalMsec; }

	void toggleWindowed();

	static MainLoop *getMainLoop();
};

#endif
