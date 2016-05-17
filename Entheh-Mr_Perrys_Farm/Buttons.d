module Buttons;
import allegro5.allegro;
import Main;
import std.math;
static import MainMenu;
import allegro5.allegro_font;
import Colours;

//Draw function calls clearButtons() at start and then registers a button when it's about to draw it.
//Event handler calls onClick if the mouse is clicked on it in the frame after it was registered.
//Therefore, either onClick should re-validate the button, or we call clearButtons() when they might become invalid.

struct Button {
	int x1, y1, x2, y2;
	void delegate() onClick;
	bool autoRepeat;
}

Button[] buttons;

void clearButtons() {
	buttons.length = 0;
}

//Returns true if mouseover.
bool registerButton(int x1, int y1, int x2, int y2, void delegate() onClick, bool autoRepeat = false) {
	//Only allow clicks on the part of the button currently unclipped.
	int cx, cy, cw, ch;
	al_get_clipping_rectangle(&cx, &cy, &cw, &ch);
	if (x1 < cx) x1 = cx;
	if (y1 < cy) y1 = cy;
	if (x2 >= cx + cw) x2 = cx + cw;
	if (y2 >= cy + ch) y2 = cy + ch;

	buttons ~= Button(x1, y1, x2, y2, onClick, autoRepeat);
	ALLEGRO_MOUSE_STATE state;
	getConvertedMouseState(&state);
	return state.x >= x1 && state.x < x2 && state.y >= y1 && state.y < y2;
}

void clickableTextLeft(int x1, int y1, int x2, in char* text, void delegate() onClick) {
	int y2 = y1 + MainMenu.lineSpacing;
	bool hover = registerButton(x1, y1, x2, y2, onClick);
	al_draw_text(theFont, hover ? white : green, x1, y1, ALLEGRO_ALIGN_LEFT, text);
}

void clickableTextCentre(int x1, int y1, int x2, in char* text, void delegate() onClick) {
	int y2 = y1 + MainMenu.lineSpacing;
	bool hover = registerButton(x1, y1, x2, y2, onClick);
	al_draw_text(theFont, hover ? white : green, (x1+x2)/2, y1, ALLEGRO_ALIGN_CENTRE, text);
}

ALLEGRO_EVENT buttonEvent;

enum initialRepeat = 0.5f;
enum laterRepeat = 0.06f;
float nextRepeatTime = float.nan;

bool checkButtons(ALLEGRO_EVENT* event, bool repeating = false) {
	if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
		foreach_reverse (i, ref button; buttons) {
			if (event.mouse.x >= button.x1 && event.mouse.x < button.x2 && event.mouse.y >= button.y1 && event.mouse.y < button.y2) {
				buttonEvent = *event;
				button.onClick();
				if (button.autoRepeat) nextRepeatTime = event.any.timestamp + (repeating ? laterRepeat : initialRepeat);
				return true;
			}
		}
	}
	if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) nextRepeatTime = float.nan;
	return false;
}

void updateButtons() {
	if (!nextRepeatTime.isNaN) {
		while (time >= nextRepeatTime) {
			buttonEvent.any.timestamp = nextRepeatTime;
			checkButtons(&buttonEvent, true);
		}
	}
}
