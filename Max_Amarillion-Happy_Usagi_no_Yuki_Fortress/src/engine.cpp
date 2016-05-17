#include "engine.h"
#include "game.h"
#include "resources.h"
#include "util.h"
#include "mainloop.h"
#include "control.h"
#include "DrawStrategy.h"
#include "text.h"
#include "color.h"

using namespace std;

const int DIRNUM = 4;
const char *DIRECTIONS[DIRNUM] = { "n", "e", "s", "w" };

class EngineImpl : public Engine
{
private:
	shared_ptr<Game> game;
	Resources resources;
	shared_ptr<IContainer> menu;

public:
	EngineImpl() : game(Game::newInstance(this)), resources(), menu()
	{
	}

	virtual Resources* getResources() override { return &resources; }

	virtual int init() override
	{
		Anim::setDirectionModel (make_shared<DirectionModel>(DIRECTIONS, DIRNUM));
		srand(time(0));

		if (!(
			resources.addFiles("data/*.ttf") &&
			resources.addFiles("data/*.png") &&
			resources.addFiles("data/*.xml") &&
			resources.addFiles("data/*.ogg") &&
			resources.addFiles("data/*.xm") &&
			resources.addFiles("data/*.wav")
			))
		{
			allegro_message ("Error while loading resources!\n%s", resources.getErrorMsg());
			return 1;
		}

//		sfont = al_create_builtin_font(); // TODO: move to resources class;
		sfont = resources.getFont("jap_16");

		add(game, FLAG_SLEEP);
		initMenu();
		setFocus(menu);

		game->init();
		startMusic();
		return 0;
	}

	virtual void done () override
	{
	}

	void startMusic()
	{
		MainLoop::getMainLoop()->playMusic(resources.getMusic("Boterham_happy"));
	}

	void initMenu()
	{
		int buttonw = 200;
		int buttonh = 40;
		int yco = 50;
		int xco = (getw() - buttonw) / 2;
		int margin = 10;

		menu = make_shared<IContainer>();

		menu->add(ClearScreen::build(BLACK).get());

		menu->add(Text::build(RED, ALLEGRO_ALIGN_CENTER, "Happy Usagi No Yuki Fortress\n幸せなウサギの雪要塞").xywh(getw() / 2, yco, getw(), 80).
				font(resources.getFont("jap_32")).get());

		yco += 100;

		menu->add(Button::build(MSG_POEM, 			"PLAY").xywh(xco, yco, buttonw, buttonh).get());
		yco += buttonh + margin;
		menu->add(Button::build(MSG_TOGGLE_WINDOWED,"WINDOWED ON/OFF").xywh(xco, yco, buttonw, buttonh).get());
		yco += buttonh + margin;
		menu->add(Button::build(MSG_TOGGLE_MUSIC,	"MUSIC ON/OFF").xywh(xco, yco, buttonw, buttonh).get());
		yco += buttonh + margin;

		//TODO
//		menu->add(Button::build(MSG_CREDITS, 		"CREDITS").xywh(xco, yco, buttonw, buttonh).get());
//		yco += buttonh + margin;

		menu->add(Button::build(MSG_QUIT_BUTTON, 	"QUIT").xywh(xco, yco, buttonw, buttonh).get());
		add (menu);
	}

	virtual void handleMessage(int code) override
	{
		switch (code)
		{
		case MSG_POEM:
		{
			auto poemScreen = make_shared<IContainer>();
			poemScreen->add(ClearScreen::build(WHITE).get());
			poemScreen->add(Text::build(GREY, ALLEGRO_ALIGN_LEFT, "binky crazy jump\n\nbunny in the fluffy snow\n\nwhat makes you happy?").xy(100, 200).
					font(resources.getFont("jap_40")).get());
			poemScreen->add(make_shared<Timer>(150, MSG_PLAY));
			add (poemScreen);
			setFocus (poemScreen);
			break;
		}
		case MSG_MAIN_MENU:
			setFocus(menu);
			break;
		case MSG_QUIT:
			pushMsg(-1);
			break;
		case MSG_QUIT_BUTTON:
		{
			auto exitScreen = make_shared<IContainer>();
			exitScreen->add(ClearScreen::build(BLUE).get());
			exitScreen->add(Text::build(WHITE, ALLEGRO_ALIGN_CENTER, "THANKS FOR PLAYING\nMax & Amarillion").center(game).get());
			exitScreen->add(make_shared<Timer>(150, MSG_QUIT));
			add (exitScreen);
			setFocus (exitScreen);
			break;
		}
		case MSG_PLAY:
			setFocus(game);
			break;
		case MSG_TOGGLE_WINDOWED:
			MainLoop::getMainLoop()->toggleWindowed();
			break;
		case MSG_TOGGLE_MUSIC:
		{
			bool enabled = MainLoop::getMainLoop()->isMusicOn();
			enabled = !enabled;
			MainLoop::getMainLoop()->setMusicOn(enabled);
			if (enabled) startMusic();
		}
			break;
		case MSG_CREDITS:
			// TODO
			break;
		}
	}
};

shared_ptr<Engine> Engine::newInstance()
{
	return make_shared<EngineImpl>();
}
