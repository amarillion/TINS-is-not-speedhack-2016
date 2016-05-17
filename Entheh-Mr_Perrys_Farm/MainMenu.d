module MainMenu;
import Main;
import allegro5.allegro;
import allegro5.allegro_font;
import Maths;
import Game;
import Editor;
import Colours;

enum lineSpacing = 60;
enum lineWidth = screenWidth/4;
enum lineLeft = (screenWidth-lineWidth)/2;
enum lineRight = (screenWidth+lineWidth)/2;
enum menuTop = (screenHeight-lineSpacing*5)/2;

int selected = -1;

bool handleEvent(ALLEGRO_EVENT* event)
{
	if (event.mouse.x >= lineLeft && event.mouse.x < lineRight) {
		int y = (event.mouse.y - menuTop).divRoundDown(lineSpacing);
		if (y < 0 || y >= 3) y = -1;
		if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
			selected = y;
			return true;
		} else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && y >= 0) {
			selected = y;
			final switch (selected) {
				case 0: startGame(); break;
				case 1: theEditor = new Editor(); break;
				case 2: exit = true; break;
			}
			return true;
		}
	}

	return false;
}

void update()
{
}

void draw()
{
	al_clear_to_color(black);

	al_draw_text(theFont, white, screenWidth/2, menuTop - lineSpacing*2, ALLEGRO_ALIGN_CENTRE, "～ Mr Perry's Farm ～");

	al_draw_text(theFont, selected == 0 ? white : green, screenWidth/2, menuTop + lineSpacing*0, ALLEGRO_ALIGN_CENTRE, "Play it");
	al_draw_text(theFont, selected == 1 ? white : green, screenWidth/2, menuTop + lineSpacing*1, ALLEGRO_ALIGN_CENTRE, "Edit");
	al_draw_text(theFont, selected == 2 ? white : green, screenWidth/2, menuTop + lineSpacing*2, ALLEGRO_ALIGN_CENTRE, "Exit");

	al_draw_text(theFont, white, screenWidth/2, menuTop + lineSpacing*4, ALLEGRO_ALIGN_CENTRE, `Copyright 2016 弁 'Bruce "entheh" Perry' Wieczorek-Davis`);
	al_draw_text(theFont, white, screenWidth/2, menuTop + lineSpacing*5, ALLEGRO_ALIGN_CENTRE, `TINS Allegro Game Programming Competition 2016`);
	al_draw_text(theFont, white, screenWidth/2, menuTop + lineSpacing*6, ALLEGRO_ALIGN_CENTRE, `Font ふわふわフォント by いちごDesign`);
}
