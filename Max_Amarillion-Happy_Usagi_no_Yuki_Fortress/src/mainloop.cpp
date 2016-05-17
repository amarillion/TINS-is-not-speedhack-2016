#include <assert.h>
#include "mainloop.h"
#include "color.h"

#ifdef USE_TEGEL
#include <tegel5.h>
#endif

#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include "util.h"
#include "textstyle.h"

#include <iostream>

using namespace std;

MainLoop *MainLoop::instance = NULL;

MainLoop *MainLoop::getMainLoop()
{
	return instance;
}

MainLoop &MainLoop::setTitle(const char *_title)
{
	title = _title;
	return *this;
}

MainLoop &MainLoop::setAppName(const char *_appname)
{
	appname = _appname;
	return *this;
}

MainLoop &MainLoop::setConfigFilename(const char *_configFilename)
{
	configFilename = _configFilename;
	return *this;
}

MainLoop &MainLoop::setEngine(shared_ptr<IComponent> _engine)
{
	engine = _engine;
	return *this;
}

MainLoop::MainLoop() :
		display(NULL),
		engine(), localAppData(NULL), configPath(NULL),
		configFilename("twist.cfg"), title("untitled"), appname(NULL), stretch (false), smokeTest(false), logicIntervalMsec(20), config(NULL)
{
	w = 640;
	h = 480;
	buffer = NULL;
	last_fps = 0;
	lastUpdateMsec = 0;
	frame_count = 0;
	frame_counter = 0;
#ifdef DEBUG
	// default in debug version
	screenMode = WINDOWED;
	fpsOn = true;
#else
	// default in release version
	screenMode = FULLSCREEN_WINDOW;
	fpsOn = false;
#endif
	instance = this;
}

MainLoop::MainLoop (IComponent *_engine, const char *configFilename, const char *title, int _bufw, int _bufh) :
		display(NULL),
	engine (_engine), localAppData(NULL), configPath(NULL),
	configFilename(configFilename), title(title), appname(NULL), stretch (false), smokeTest(false), logicIntervalMsec(20), config(NULL)
{
	w = _bufw;
	h = _bufh;
	buffer = NULL;
	last_fps = 0;
	lastUpdateMsec = 0;
	frame_count = 0;
	frame_counter = 0;

#ifdef DEBUG
	// default in debug version
	screenMode = WINDOWED;
	fpsOn = true;
#else
	// default in release version
	screenMode = FULLSCREEN_WINDOW;
	fpsOn = false;
#endif
	instance = this;
}

void MainLoop::getFromConfig(ALLEGRO_CONFIG *config)
{
	getSoundFromConfig(config);
	fpsOn = get_config_int (config, "twist", "fps", fpsOn);
	screenMode = (ScreenMode)get_config_int (config, "twist", "windowed", screenMode);
}

void MainLoop::getFromArgs(int argc, const char *const *argv)
{
	// parse command line arguments
	int i;
	for (i = 1; i < argc; i++)
	{
		if (strcmp (argv[i], "-nosound") == 0)
		{
			setSoundInstalled(false);
		}
		else if (strcmp (argv[i], "-windowed") == 0)
		{
			screenMode = WINDOWED;
		}
		else if (strcmp (argv[i], "-fullscreen") == 0)
		{
			screenMode = FULLSCREEN_WINDOW; //TODO: add third option for FULLSCREEN for extra efficiency
		}
		else if (strcmp (argv[i], "-showfps") == 0)
		{
			fpsOn = true;
		}
		else if (strcmp (argv[i], "-smoketest") == 0)
		{
			smokeTest = true;
		}
		else
		{
			options.push_back (string(argv[i]));
		}
	}	
}

int MainLoop::init(int argc, const char *const *argv)
// returns 1 on success, 0 on failure
{
	assert (engine != NULL); // must have initialised engine by now.

	if (al_init () < 0)
	{
		allegro_message("al_init() failed");
		return 0;
	}

	// initialise application name
	assert (appname != NULL);

	al_set_app_name(appname);
	al_set_org_name("helixsoft.nl");

	localAppData = al_get_standard_path(ALLEGRO_USER_SETTINGS_PATH);

	bool result = al_make_directory(al_path_cstr(localAppData, ALLEGRO_NATIVE_PATH_SEP));
	if (!result)
	{
		cout << "Failed to create application data directory " << al_get_errno();
		//TODO: write message to log
	}

	if (al_init_image_addon() < 0)
	{
		allegro_message("init image addon failed");
		return 0;
	}

	al_init_font_addon(); //never fails, no return value...

	if (configFilename != NULL)
	{
		configPath = al_clone_path(localAppData);
		al_set_path_filename(configPath, configFilename);

		config = al_load_config_file (al_path_cstr(configPath, ALLEGRO_NATIVE_PATH_SEP));
	}

	if (config == NULL)
	{
		config = al_create_config();
	}

	getFromConfig(config);
	getFromArgs (argc, argv);

	parseOpts(options);
	
	if (al_install_keyboard () < 0)
	{
		allegro_message("install keyboard failed");
		return 0;
	}
		
	equeue = al_create_event_queue();
	timer = al_create_timer (0.02);
	al_register_event_source(equeue, al_get_timer_event_source(timer));
	al_start_timer(timer);

	if (!al_init_acodec_addon())
	{
		allegro_message("Could not initialize acodec addon. ");
	}

	if (!al_init_ttf_addon())
	{
		allegro_message ("Could not initialize ttf addon. ");
	}

	if (!al_init_primitives_addon())
	{
		allegro_message ("Could not initialize primitives addon. ");
	}

	// set_volume_per_voice (1); //TODO
	if (isSoundInstalled())
	{
		if (al_install_audio() < 0)
		{
			// could not get sound to work
			setSoundInstalled(false);
//			allegro_message ("Could not initialize sound. Sound is turned off.\n%s\n", allegro_error); //TODO
			allegro_message ("Could not initialize sound. Sound is turned off.");
		}
		else
		{
			bool success = al_reserve_samples(16);
			if (!success)
			{
				allegro_message ("Could not reserve samples");
			}
		}
		initSound();
	}

	if (initDisplay() == 0)
	{
		return 0;
	}

#ifdef USE_MOUSE
	if (al_install_mouse() == -1)
	{
		allegro_message ("could not install mouse");
		//set_gfx_mode (GFX_TEXT, 0, 0, 0, 0); //TODO....
//		allegro_exit(); //TODO.. obsolete?
		return 0;
	}
#endif
	return postInit();
}

int MainLoop::initDisplay()
{
	bool success = false;
	int display_flags = 0;
	switch (screenMode)
	{
	case FULLSCREEN:
		display_flags = ALLEGRO_FULLSCREEN;
		break;
	case FULLSCREEN_WINDOW:
		display_flags = ALLEGRO_FULLSCREEN_WINDOW;
		break;
	case WINDOWED:
		display_flags = ALLEGRO_WINDOWED;
		break;
	default:
		assert (false);
		break;
	}

	int resNum = resX.size();
	al_set_new_display_flags(display_flags);

	// if there is no resolution defined, use default of bufw x bufh
	if (resNum == 0)
	{
		addResolution (w, h);
		resNum = 1;
	}

	for (int i = 0; i < resNum; ++i)
	{
		int rx = resX[i];
		int ry = resY[i];
		display = al_create_display(rx, ry);
		if (display != NULL)
		{
			success = true;
			if (al_get_display_width(display) != w || al_get_display_height(display) != h) stretch = true;
			break;
		}
	}

	if (!success)
	{
//		allegro_message("Unable initialize graphics module\n%s\n", allegro_error); //TODO
		allegro_message("Unable initialize graphics module");
		return 0;
	}

    al_set_target_backbuffer(display);
    engine->setDimension(w, h);

    buffer = NULL;

	// use the first resolution as the primary game resolution.
	// not necessarily the same size as the actual game resolution
	if (stretch)
	{
		al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
		buffer = al_create_bitmap(w, h);
	}

	if (!buffer)
	{
		buffer = al_get_backbuffer(display);
	}

	if (!buffer)
	{
		allegro_message ("Error creating background buffer");
		return 0;
	}

	if (title != NULL)
	{
		al_set_window_title (display, title);
	}

	return 1;
}

void MainLoop::adjustMickey(int &x, int &y)
{
	if (stretch)
	{
		x = x * w / al_get_display_width(display);
		y = y * h / al_get_display_height(display);
	}
}

void MainLoop::run()
{
	if (smokeTest) return;

	// Start the event queue to handle keyboard input and our timer
	ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_display_event_source(display));
#ifdef USE_MOUSE
	al_register_event_source(queue, al_get_mouse_event_source());
#endif
	ALLEGRO_TIMER *timer = al_create_timer(logicIntervalMsec / 1000.0f);
	al_start_timer(timer);
	al_register_event_source(queue, al_get_timer_event_source(timer));

	bool done = false;
	bool need_redraw = true;

	while (!done)
	{
		ALLEGRO_EVENT event;
		al_wait_for_event(queue, &event);

		switch (event.type)
		{
			case ALLEGRO_EVENT_TIMER: {
				engine->update();
				if (engine->hasMsg()) done = true;

				counter++;
				need_redraw = true;
				break;
			}
			case ALLEGRO_EVENT_DISPLAY_CLOSE: {
				done = true;
				break;
			}
			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
			case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
			case ALLEGRO_EVENT_MOUSE_AXES: {
				adjustMickey(event.mouse.x, event.mouse.y);
				engine->handleEvent (event);
				if (engine->hasMsg()) done = true;
				break;
			}
			case ALLEGRO_EVENT_KEY_UP:
			case ALLEGRO_EVENT_KEY_DOWN:
			case ALLEGRO_EVENT_KEY_CHAR: {
#ifdef DEBUG
				if (event.keyboard.keycode == ALLEGRO_KEY_F10) {
					done = true;
					break;
				}
#endif
				engine->handleEvent (event);
				if (engine->hasMsg()) done = true;

				break;
			}
		}

		if (need_redraw && al_event_queue_is_empty(queue))
		{
			GraphicsContext gc;
			gc.buffer = buffer;
			gc.xofst = 0;
			gc.yofst = 0;

			al_set_target_bitmap(buffer);
			engine->draw(gc);
			need_redraw = false;

	        int msecCounter = getMsecCounter();

			if ((msecCounter - frame_counter) > 1000)
			{
				last_fps = frame_count;
				frame_count = 0;
				frame_counter = msecCounter;
			}
			frame_count++;

	        if (fpsOn && getFont())
			{
				draw_textf_with_background(getFont(), WHITE, BLACK, 0, 0,
					  ALLEGRO_ALIGN_LEFT, "fps: %d msec: %07d ", last_fps, msecCounter);
			}

	        if (stretch)
	        {
	    		// I tried using ALLEGRO_TRANSFORM instead of a separate buffer and using al_stretch_blit. But it's actually a lot slower.
 	        	al_set_target_bitmap (al_get_backbuffer(display));
	        	al_draw_scaled_bitmap(buffer, 0, 0, w, h, 0, 0, al_get_display_width(display), al_get_display_height(display), 0);
	        }

			al_flip_display();
		}
	}

	// cleanup
	if (configFilename != NULL)
	{
		al_save_config_file(al_path_cstr(configPath, ALLEGRO_NATIVE_PATH_SEP), config);
	}

	// stop sound - important that this is done before the ALLEGRO_AUDIO_STREAM resources are destroyed
	doneSound();
}

MainLoop::~MainLoop()
{
	if (localAppData)
		al_destroy_path(localAppData);

	if (configPath)
		al_destroy_path(configPath);

//	if (buffer) al_destroy_bitmap (buffer); //TODO / not usually necessary?

	if (timer) al_destroy_timer(timer);
	if (equeue) al_destroy_event_queue(equeue);
	if (config) al_destroy_config(config);

	if (display) al_destroy_display(display);
	al_uninstall_system();
}

void MainLoop::addResolution (int w, int h)
{
	resX.push_back (w);
	resY.push_back (h);
}

void MainLoop::toggleWindowed()
{
	if (screenMode != WINDOWED)
		screenMode = WINDOWED;
	else
		screenMode = FULLSCREEN_WINDOW;

	//TODO: reload all fonts... for FULLSCREEN - (not FULLSCREEN_WINDOW)

	al_destroy_display(display);
	initDisplay();

	set_config_int (config, "twist", "windowed", screenMode);
}

