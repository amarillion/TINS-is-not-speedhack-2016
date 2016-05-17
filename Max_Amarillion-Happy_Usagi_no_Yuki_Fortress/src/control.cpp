#include "control.h"
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include "color.h"
#include <memory>

using namespace std;

void Button::draw(const GraphicsContext &gc)
{
	int x1 = x;
	int y1 = y;
	int x2 = x + w - 4;
	int y2 = y + h - 4;

	if (pressed)
	{
		x1 += 3;
		y1 += 3;
		x2 += 3;
		y2 += 3;
	}

	al_draw_filled_rectangle(x1, y1, x2, y2, enabled ? BLACK : GREY);

	al_set_target_bitmap(gc.buffer);
    if (icon != NULL) { al_draw_bitmap (icon, x1, y1, 0); }

    al_draw_text (sfont, (enabled ? WHITE : LIGHT_GREY),
        (x2 + x1) / 2, (y2 + y1 - al_get_font_line_height(sfont)) / 2, ALLEGRO_ALIGN_CENTRE,
		text.c_str());


    if (hasFocus)
    {
    	//TODO
    	// replace with border function
     //   dotted_rect (gc.buffer, x, y, x + w - 1, y + h - 1, BLACK);
    	al_draw_rectangle (x1, y1, x2, y2, GREEN, 1.0);
    }
    else
        al_draw_rectangle (x1, y1, x2, y2, WHITE, 1.0);
}


/**
guibutton message handler
*/
void Button::handleEvent(ALLEGRO_EVENT &event)
{

	switch (event.type)
	{
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
			if (!enabled) break;
			if (!pressed) // TODO - receiving two DOWN events after each other. This is a hack to prevent duplicate messages, but really this should be fixed elsewhere
			{
				pushMsg(action);
			}
			pressed = true;
			break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
			pressed = false; //TODO - also when mouse exits... // guarantee exit event
			break;
		case ALLEGRO_EVENT_KEY_CHAR:
		{
			if (!enabled) break;
			switch (event.keyboard.keycode)
			{
				case ALLEGRO_KEY_SPACE: case ALLEGRO_KEY_ENTER:
					pushMsg(action);
					break;
			}
			break;
		}
    }
}

void Button::handleMessage(int msg)
{
	// for component, unfocus means that "awake" and "visible" are set to false. -> This should be the meaning for Layer...
	// for Button, focus means that component is highlighted.
	switch (msg)
	{
	case MSG_FOCUS:
		hasFocus = true;
		break;
	case MSG_UNFOCUS:
		hasFocus = false;
		break;
	}
}

ComponentBuilder Button::build(int action, std::string text, ALLEGRO_BITMAP* icon)
{
	return ComponentBuilder(make_shared<Button>(action, text, icon));
}
