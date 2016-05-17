module spring;
import board;
import button;
import draggable;
import io;
import piece;
import puzzle;
import slot;
import text_zone;
import zone;

import core.stdc.stdlib;
import std.algorithm;
import std.exception;
import std.random;
import std.range;
import std.stdio;
import std.string;

pragma (lib, "dallegro5");
pragma (lib, "allegro");
pragma (lib, "allegro_font");
pragma (lib, "allegro_primitives");
pragma (lib, "allegro_ttf");

import allegro5.allegro;
import allegro5.allegro_font;
import allegro5.allegro_primitives;
import allegro5.allegro_ttf;

immutable int MAX_X = 800;
immutable int MAX_Y = 600;
immutable real FPS = 60.0;

ALLEGRO_DISPLAY * display;
ALLEGRO_EVENT_QUEUE * eventQueue;
ALLEGRO_TIMER * drawTimer;
ALLEGRO_FONT * textFont;
ALLEGRO_FONT * buttonFont;
ALLEGRO_FONT * captionFont;

auto toAllegroUstr (const char [] s)
{
	auto tempUstrInfo = new ALLEGRO_USTR_INFO;
	return al_ref_buffer (tempUstrInfo, s.ptr, s.length);
}

void init ()
{
	enforce (al_init ());
	enforce (al_init_font_addon ());
	enforce (al_init_primitives_addon ());
	enforce (al_init_ttf_addon ());
	enforce (al_install_keyboard ());
	enforce (al_install_mouse ());

	display = al_create_display (MAX_X, MAX_Y);
	enforce (display);

	drawTimer = al_create_timer (1.0 / FPS);
	enforce (drawTimer);
	al_start_timer (drawTimer);

	textFont = al_load_ttf_font ("data/EBGaramond.otf", 24, 0);
	enforce (textFont);

	buttonFont = al_load_ttf_font ("data/EBGaramond.otf", 30, 0);
	enforce (buttonFont);

	captionFont = al_load_ttf_font ("data/EBGaramond.otf", 48, 0);
	enforce (captionFont);

	eventQueue = al_create_event_queue ();
	enforce (eventQueue);

	al_register_event_source (eventQueue, al_get_keyboard_event_source ());
	al_register_event_source (eventQueue, al_get_mouse_event_source ());
	al_register_event_source (eventQueue, al_get_timer_event_source (drawTimer));
	al_register_event_source (eventQueue, al_get_display_event_source (display));
}

Io ioRoot;
Io cursorRoot;
Io menu;
Io nextButton;

Puzzle [] puzzleRu;
Puzzle [] puzzleEn;

void draw ()
{
	al_clear_to_color (al_map_rgb_f (0.0, 0.0, 0.0));
	enforce (ioRoot !is null);
	ioRoot.draw ();
	ALLEGRO_MOUSE_STATE mouse;
	al_get_mouse_state (&mouse);
	if (cursorRoot !is null)
	{
		ALLEGRO_BITMAP * prevBuffer = al_get_target_bitmap ();
		ALLEGRO_BITMAP * curBuffer = al_create_sub_bitmap
		    (prevBuffer, mouse.x - cursorRoot.w / 2,
		    mouse.y - cursorRoot.h / 2, cursorRoot.w, cursorRoot.h);
		al_set_target_bitmap (curBuffer);
		cursorRoot.draw ();
		al_set_target_bitmap (prevBuffer);
		al_destroy_bitmap (curBuffer);
	}
	al_flip_display ();
}

bool isFinished = false;

auto prepareText (string fileName)
{
	auto file = File (fileName, "rt");
	Puzzle [] puzzle;
	string [] curLines;
	foreach (line; file.byLineCopy ())
	{
		line = line.strip;
		if (line == "")
		{
			continue;
		}
		curLines ~= line;
		if (curLines.length < 4)
		{
			continue;
		}
		puzzle ~= new Puzzle (curLines);
		curLines.length = 0;
	}
	return puzzle;
}

void recalcNextButton ()
{
	auto nextButtonOn = ioRoot.child.filter
	    !(x => cast (Slot) x !is null).all
	    !(x => (cast (Slot) x).hasMatch);
	if (nextButtonOn)
	{
		if (nextButton.parent is null)
		{
			nextButton.parent = ioRoot;
			ioRoot.child ~= nextButton;
		}
	}
	else
	{
		if (nextButton.parent !is null)
		{
			nextButton.parent.child =
			    nextButton.parent.child.filter
			    !(x => x !is nextButton).array;
			nextButton.parent = null;
		}
	}
}

auto solve (Puzzle [] puzzle)
{
	if (puzzle.empty)
	{
		return menu;
	}
	auto solveForm = new Board (null, 0, 0, MAX_X, MAX_Y, 0,
	    al_map_rgb_f (0.1, 0.5, 0.1));
	auto buttonColor = al_map_rgb_f (0.1, 0.3, 0.5);
	nextButton = new Button (null,
	    MAX_X * 1 / 3 - 140 / 2, 535, 140, 40, 5,
	    buttonColor, al_map_rgb_f (0.5, 0.9, 0.5), buttonFont,
	    (puzzle.length > 1 ? "Next" : "Finish").toAllegroUstr (),
	    (int posX, int posY) {ioRoot = solve (puzzle[1..$]);});
	auto exitButton = new Button (solveForm,
	    MAX_X * 2 / 3 - 140 / 2, 535, 140, 40, 5,
	    buttonColor, al_map_rgb_f (0.9, 0.5, 0.5),
	    buttonFont, "Exit".toAllegroUstr (),
	    (int posX, int posY) {ioRoot = menu;});
	auto curPuzzle = puzzle.front;
	int curY = 100;
	foreach (line; curPuzzle.piece)
	{
		int curX = 100;
		curX += 50;
		foreach (piece; line)
		{
			auto w = piece.w;
			int randomX = uniform (100, 601);
			int randomY = uniform (300, 451);
			auto puzzleSlot = new Slot (solveForm,
			    curX, curY, 60, 42, 5,
			    al_map_rgb_f (0.6, 0.6, 0.4),
			    al_map_rgba_f (0.0, 0.0, 0.1, 0.1),
			    piece.contents);
			auto puzzlePart = new Draggable (solveForm,
			    randomX, randomY, w + 10, 38, 5,
			    al_map_rgb_f (0.7, 0.7, 0.5),
			    al_map_rgb_f (0.0, 0.0, 0.2),
			    textFont, piece.contents);
			curX += 60 + 10;
		}
		curY += 50;
	}
	return solveForm;
}

auto prepareMenu ()
{
	auto menu = new Zone (null, 0, 0, MAX_X, MAX_Y, 0,
	    al_map_rgb_f (0.2, 0.3, 0.1));
	auto buttonColor = al_map_rgb_f (0.1, 0.3, 0.5);
	auto caption = new TextZone (menu,
	    (MAX_X - 200) / 2, 110, 200, 50, 0,
	    al_map_rgba_f (0.0, 0.0, 0.0, 0.0), al_map_rgb_f (0.4, 0.9, 0.1),
	    captionFont, "SPRING".toAllegroUstr ());
	auto someText = new TextZone (menu,
	    (MAX_X - 400) / 2, 190, 400, 50, 0,
	    al_map_rgba_f (0.0, 0.0, 0.0, 0.0), al_map_rgb_f (0.5, 0.8, 0.8),
	    textFont, "Fight snow with verse!".toAllegroUstr ());
	auto ruButton = new Button (menu,
	    MAX_X * 1 / 4 - 140 / 2, 300, 140, 40, 5,
	    buttonColor, al_map_rgb_f (0.9, 0.9, 0.5),
	    buttonFont, "Тютчев".toAllegroUstr (),
	    (int posX, int posY) {ioRoot = solve (puzzleRu);});
	auto enButton = new Button (menu,
	    MAX_X * 3 / 4 - 140 / 2, 300, 140, 40, 5,
	    buttonColor, al_map_rgb_f (0.9, 0.9, 0.5),
	    buttonFont, "Dickinson".toAllegroUstr (),
	    (int posX, int posY) {ioRoot = solve (puzzleEn);});
	auto exitButton = new Button (menu,
	    (MAX_X - 140) / 2, 375, 140, 40, 5,
	    buttonColor, al_map_rgb_f (0.9, 0.5, 0.5),
	    buttonFont, "Exit".toAllegroUstr (),
	    (int posX, int posY) {isFinished = true;});
	return menu;
}

void mainLoop ()
{
	puzzleRu = prepareText ("data/tutchev.txt");
	puzzleEn = prepareText ("data/dickinson.txt");
	menu = prepareMenu ();
	ioRoot = menu;
	cursorRoot = null;
	draw ();

	isFinished = false;
	while (!isFinished)
	{
		ALLEGRO_EVENT currentEvent;
		al_wait_for_event (eventQueue, &currentEvent);

		switch (currentEvent.type)
		{
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				isFinished = true;
				break;

			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
				if (currentEvent.mouse.button == 1)
				{
					auto x = currentEvent.mouse.x;
					auto y = currentEvent.mouse.y;
					ioRoot.click (x, y);
				}
				break;

			case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
				if (currentEvent.mouse.button == 1)
				{
					auto x = currentEvent.mouse.x;
					auto y = currentEvent.mouse.y;
					ioRoot.unclick (x, y);
				}
				break;

			case ALLEGRO_EVENT_TIMER:
				draw ();
				break;

			default:
				break;
		}
	}
}

void happyEnd ()
{
	al_destroy_event_queue (eventQueue);
	al_destroy_font (captionFont);
	al_destroy_font (buttonFont);
	al_destroy_font (textFont);
	al_destroy_timer (drawTimer);
	al_destroy_display (display);

	al_shutdown_font_addon ();
	al_shutdown_primitives_addon ();
	al_shutdown_ttf_addon ();

	exit (EXIT_SUCCESS);
}

int main (string [] args)
{
	return al_run_allegro (
	{
		init ();
		mainLoop ();
		happyEnd ();
		return 0;
	});
}
