import std.stdio;
import std.exception;
import allegro5.allegro;
import allegro5.allegro_font;
import allegro5.allegro_ttf;
import allegro5.allegro_primitives;
import allegro5.allegro_native_dialog;
import allegro5.allegro_image;
import allegro5.allegro_audio;
import allegro5.allegro_acodec;
import Game;
import Editor;
static import MainMenu;
import Colours;
import Buttons;
import LevelModule;
import std.string;

ALLEGRO_FONT* theFont;

ALLEGRO_BITMAP* machineCore;
ALLEGRO_BITMAP* bloodDroplet;
ALLEGRO_BITMAP* bloodSplat;
ALLEGRO_BITMAP* burnMark;
ALLEGRO_BITMAP* fireBall;
ALLEGRO_BITMAP* wallSprite;
ALLEGRO_BITMAP*[ComponentType.max+1] componentSprites;
ALLEGRO_BITMAP*[AnimalType.max+1] animalSprites;
ALLEGRO_BITMAP*[FoodType.max+1] foodSprites;
ALLEGRO_BITMAP*[18] grassSprites;

ALLEGRO_SAMPLE*[AnimalType.max+1] animalSounds;
ALLEGRO_SAMPLE* splatSound;
ALLEGRO_SAMPLE* poofSound;

Game theGame;
Editor theEditor;

double frameStartTime, time, deltaTime;

bool exit;

enum screenWidth  = 1920;
enum screenHeight = 1080;

bool windowed = false;

//If display doesn't have our ideal resolution, we draw to screen first and then scale/frame it into display.
ALLEGRO_DISPLAY* display;
ALLEGRO_BITMAP* screen;

int main(string[] args)
{
	try {
		return main2(args);
	} catch (Throwable t) {
		writeln(t.toString());
		writeln("Press Enter to close...");
		readln();
		return 1;
	}
}

int main2(string[] args)
{
	return al_run_allegro({
		enforce(al_init());
		enforce(al_init_font_addon());
		enforce(al_init_ttf_addon());
		enforce(al_init_primitives_addon());
		enforce(al_init_native_dialog_addon());
		enforce(al_init_image_addon());

		enforce(al_install_mouse());
		enforce(al_install_keyboard());

		al_install_audio();
		al_reserve_samples(32);
		al_init_acodec_addon();

		al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

		//This doesn't compile - is it an unstable feature?
		//al_set_new_display_option(ALLEGRO_SUPPORTED_ORIENTATIONS, ALLEGRO_DISPLAY_ORIENTATION_LANDSCAPE, ALLEGRO_SUGGEST);

		al_set_new_window_title("Mr Perry's Farm");

		//We force OpenGL because Allegro 5.2.0 has a D3D bug in it that crashes us after a resize.
		if (windowed) {
			al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE | ALLEGRO_OPENGL);
			display = enforce(al_create_display(screenWidth/2, screenHeight/2));
		} else {
			al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW | ALLEGRO_OPENGL);
			display = enforce(al_create_display(screenWidth, screenHeight));
		}
		scope (exit) al_destroy_display(display);

		if (al_get_display_width(display) != screenWidth || al_get_display_height(display) != screenHeight)
			screen = al_create_bitmap(screenWidth, screenHeight);
		scope (exit) al_destroy_bitmap(screen);

		theFont = enforce(al_load_ttf_font("Resources/hosofuwafont/Hosofuwafont-tweaked.ttf", 56, 0));
		scope (exit) al_destroy_font(theFont);

		machineCore = enforce(al_load_bitmap("Resources/Graphics/Misc/MachineCore.png"));
		scope (exit) al_destroy_bitmap(machineCore);

		al_set_display_icon(display, machineCore);

		bloodDroplet = enforce(al_load_bitmap("Resources/Graphics/Misc/BloodDroplet.png"));
		scope (exit) al_destroy_bitmap(bloodDroplet);

		bloodSplat = enforce(al_load_bitmap("Resources/Graphics/Misc/BloodSplat.png"));
		scope (exit) al_destroy_bitmap(bloodSplat);

		burnMark = enforce(al_load_bitmap("Resources/Graphics/Misc/BurnMark.png"));
		scope (exit) al_destroy_bitmap(burnMark);

		fireBall = enforce(al_load_bitmap("Resources/Graphics/Misc/FireBall.png"));
		scope (exit) al_destroy_bitmap(fireBall);

		wallSprite = enforce(al_load_bitmap("Resources/Graphics/Tiles/Wall.png"));
		scope (exit) al_destroy_bitmap(wallSprite);

		scope (exit) foreach (sprite; componentSprites) al_destroy_bitmap(sprite);
		componentSprites[ComponentType.Bomb       ] = enforce(al_load_bitmap("Resources/Graphics/Components/Bomb.png"));
		componentSprites[ComponentType.TurnChanger] = enforce(al_load_bitmap("Resources/Graphics/Components/TurnChanger.png"));
		componentSprites[ComponentType.Multiply   ] = enforce(al_load_bitmap("Resources/Graphics/Components/Multiply.png"));
		componentSprites[ComponentType.Rain       ] = enforce(al_load_bitmap("Resources/Graphics/Components/Rain.png"));
		componentSprites[ComponentType.Snow       ] = enforce(al_load_bitmap("Resources/Graphics/Components/Snow.png"));
		componentSprites[ComponentType.Sun        ] = enforce(al_load_bitmap("Resources/Graphics/Components/Sun.png"));

		scope (exit) foreach (sprite; animalSprites) al_destroy_bitmap(sprite);
		animalSprites[AnimalType.Cow  ] = enforce(al_load_bitmap("Resources/Graphics/Animals/Cow.png"));
		animalSprites[AnimalType.Sheep] = enforce(al_load_bitmap("Resources/Graphics/Animals/Sheep.png"));
		animalSprites[AnimalType.Pig  ] = enforce(al_load_bitmap("Resources/Graphics/Animals/Pig.png"));
		animalSprites[AnimalType.Duck ] = enforce(al_load_bitmap("Resources/Graphics/Animals/Duck.png"));

		scope (exit) foreach (sprite; foodSprites) al_destroy_bitmap(sprite);
		foodSprites[FoodType.Beef ] = enforce(al_load_bitmap("Resources/Graphics/Food/Beef.png"));
		foodSprites[FoodType.Lamb ] = enforce(al_load_bitmap("Resources/Graphics/Food/Lamb.png"));
		foodSprites[FoodType.Bacon] = enforce(al_load_bitmap("Resources/Graphics/Food/Bacon.png"));
		foodSprites[FoodType.Duck ] = enforce(al_load_bitmap("Resources/Graphics/Food/Duck.png"));

		scope (exit) foreach (sprite; grassSprites) al_destroy_bitmap(sprite);
		foreach (i, ref sprite; grassSprites) sprite = enforce(al_load_bitmap(format("Resources/Graphics/Grass/Grass%s.png", i).toStringz()));

		scope (exit) foreach (sound; animalSounds) al_destroy_sample(sound);
		animalSounds[AnimalType.Cow  ] = al_load_sample("Resources/Audio/Animals/Moo.wav");
		animalSounds[AnimalType.Sheep] = al_load_sample("Resources/Audio/Animals/Baa.wav");
		animalSounds[AnimalType.Pig  ] = al_load_sample("Resources/Audio/Animals/Oink.wav");
		animalSounds[AnimalType.Duck ] = al_load_sample("Resources/Audio/Animals/Quack.wav");

		splatSound = al_load_sample("Resources/Audio/Splat.wav");
		scope (exit) al_destroy_sample(splatSound);

		poofSound = al_load_sample("Resources/Audio/Poof.wav");
		scope (exit) al_destroy_sample(poofSound);

		auto eventQueue = enforce(al_create_event_queue());
		scope (exit) al_destroy_event_queue(eventQueue);
		al_register_event_source(eventQueue, al_get_display_event_source(display));
		al_register_event_source(eventQueue, al_get_mouse_event_source());
		al_register_event_source(eventQueue, al_get_keyboard_event_source());

		frameStartTime = al_get_time();

		mainLoop: while (true)
		{
			ALLEGRO_EVENT event;
			while (al_get_next_event(eventQueue, &event))
			{
				convertMouseEvent(&event);
				time = event.any.timestamp;

				switch (event.type)
				{
					case ALLEGRO_EVENT_DISPLAY_CLOSE:
						exit = true;
						break;

					case ALLEGRO_EVENT_DISPLAY_RESIZE:
						al_acknowledge_resize(display);
						break;

					default:
						if (checkButtons(&event)) clearButtons();
						else if (theGame) theGame.handleEvent(&event);
						else if (theEditor) theEditor.handleEvent(&event);
						else MainMenu.handleEvent(&event);
				}

				if (exit) {
					if (theEditor && !theEditor.checkSaved())
						exit = false;
					else
						break mainLoop;
				}
			}

			time = al_get_time();
			deltaTime = time - frameStartTime;
			frameStartTime = time;

			update();
			draw();
		}

		return 0;
	});
}

void update()
{
	updateButtons();
	if (theGame) theGame.update();
	else if (theEditor) theEditor.update();
	else MainMenu.update();
}

void draw()
{
	if (screen) al_set_target_bitmap(screen);

	clearButtons();

	if (theGame) theGame.draw();
	else if (theEditor) theEditor.draw();
	else MainMenu.draw();

	if (screen) {
		al_set_target_backbuffer(display);

		int displayWidth = al_get_display_width(display);
		int displayHeight = al_get_display_height(display);

		//Pillarbox or letterbox appropriately.
		float screenAspect = cast(float)screenWidth / screenHeight;
		float displayAspect = cast(float)displayWidth / displayHeight;
		int pillar = 0, letter = 0;
		if (screenAspect > displayAspect) {
			letter = (displayHeight - screenHeight*displayWidth/screenWidth) / 2;
			al_draw_filled_rectangle(0, 0, displayWidth, letter, black);
			al_draw_filled_rectangle(0, displayHeight-letter, displayWidth, displayHeight, black);
		} else {
			pillar = (displayWidth - screenWidth*displayHeight/screenHeight) / 2;
			al_draw_filled_rectangle(0, 0, pillar, displayHeight, black);
			al_draw_filled_rectangle(displayWidth-pillar, 0, displayWidth, displayHeight, black);
		}

		al_draw_scaled_bitmap(screen, 0, 0, screenWidth, screenHeight, pillar, letter, displayWidth-2*pillar, displayHeight-2*letter, 0);
	}

	al_flip_display();
}

void convertMouseEvent(ALLEGRO_EVENT* event) {
	//Hack :o
	if (event.type >= ALLEGRO_EVENT_MOUSE_AXES && event.type < ALLEGRO_EVENT_TIMER) {
		convertMousePosition(&event.mouse.x, &event.mouse.y);
	}
}

void getConvertedMouseState(ALLEGRO_MOUSE_STATE* state) {
	al_get_mouse_state(state);
	convertMousePosition(&state.x, &state.y);
}

void convertMousePosition(int* x, int* y) {
	if (screen) {
		int displayWidth = al_get_display_width(display);
		int displayHeight = al_get_display_height(display);

		//Pillarbox or letterbox appropriately.
		float screenAspect = cast(float)screenWidth / screenHeight;
		float displayAspect = cast(float)displayWidth / displayHeight;
		int pillar = 0, letter = 0;
		if (screenAspect > displayAspect) {
			letter = (displayHeight - screenHeight*displayWidth/screenWidth) / 2;
		} else {
			pillar = (displayWidth - screenWidth*displayHeight/screenHeight) / 2;
		}

		*x = (*x - pillar) * screenWidth / (displayWidth-2*pillar);
		*y = (*y - letter) * screenHeight / (displayHeight-2*letter);
	}
}
