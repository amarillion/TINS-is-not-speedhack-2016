#include "main.h"
#include "title.h"
#include "input.h"

#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

bool title_screen(void) {
	ALLEGRO_FONT *font, *font2;
	ALLEGRO_BITMAP *bitmap;
	ALLEGRO_EVENT event;
	ALLEGRO_TRANSFORM t;
	ALLEGRO_COLOR black = al_map_rgb(0, 0, 0), red = al_map_rgb(255, 0, 0), white = al_map_rgb(255, 255, 255), offWhite = al_map_rgba(255, 255, 255, 128), selected = al_map_rgb(0, 0, 128);
	ALLEGRO_USTR *titleStr;

	Input input;
	bool ret = false, running = true, redraw = true, mouse_down = false, mouse_selected = false;
	int selection = 0, mselection = 0, x, y, w, h;
	float cx, cy, scale, ar, tx, ty, px, py, spx, spy, qx, qy, sqx, sqy, pbx, pby, pbw, pbh, qbx, qby, qbw, qbh;

	al_identity_transform(&t);
	bitmap = al_get_target_bitmap();
	cx = al_get_bitmap_width(bitmap);
	cy = al_get_bitmap_height(bitmap);
	ar = cx / cy;
	scale = cy / 480.0;
	cx = cx / scale;
	al_scale_transform(&t, scale, scale);

	font = al_load_ttf_font("jap.otf", -80, 0);
	if (!font) return false;
	font2 = al_load_ttf_font("jap.otf", -30, ALLEGRO_TTF_MONOCHROME);
	if (!font2) {
		al_destroy_font(font);
		return false;
	}

	titleStr = al_ustr_new("Haiku \xE4\xBF\xB3\xE5\x8F\xA5 Rescue");

	al_get_ustr_dimensions(font, titleStr, &x, &y, &w, &h);
	tx = ((480 * ar) - w) / 2.0;
	ty = 80;

	al_get_text_dimensions(font2, "Play", &x, &y, &w, &h);
	px = ((480 * ar) - w) / 2.0;
	al_get_text_dimensions(font2, "> Play <", &x, &y, &w, &h);
	pbx = spx = ((480 * ar) - w) / 2.0;

	pby = spy = py = 300;
	pbw = w + pbx;
	pbh = h + pby;
	qby = sqy = qy = py + h + 5;

	al_get_text_dimensions(font2, "Quit", &x, &y, &w, &h);
	qx = ((480 * ar) - (w - x)) / 2.0;
	al_get_text_dimensions(font2, "> Quit <", &x, &y, &w, &h);
	qbx = sqx = ((480 * ar) - (w - x)) / 2.0;

	qbw = w + qbx;
	qbh = h + qby;

	al_start_timer(timer);
	while (running) {
		while (!al_is_event_queue_empty(queue)) {
			al_get_next_event(queue, &event);

			switch (event.type) {
			case ALLEGRO_EVENT_MOUSE_AXES:
				// Change the highlight, if mouse is down, and the selection would be different than before, remove selection
				if ((event.mouse.x / scale > pbx) && (event.mouse.x / scale < pbw) && (event.mouse.y / scale > pby) && (event.mouse.y / scale < pbh)) {
					if (mouse_down && mselection != 1) {
						mselection = 0;
					}
					else {
						mselection = 1;
						selection = 0;
					}
				}
				else if ((event.mouse.x / scale > qbx) && (event.mouse.x / scale < qbw) && (event.mouse.y / scale > qby) && (event.mouse.y / scale < qbh)) {
					if (mouse_down && mselection != 2) {
						mselection = 0;
					}
					else {
						mselection = 2;
						selection = 1;
					}
				}
				else {
					mselection = 0;
				}
				break;
			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
				if (event.mouse.button == 1) {
					mouse_down = true;
				}
				break;
			case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
				if (event.mouse.button == 1) {
					mouse_down = false;
					mouse_selected = true;
				}
				break;
			case ALLEGRO_EVENT_KEY_DOWN:
				input.keyDown(&event);
				break;
			case ALLEGRO_EVENT_KEY_UP:
				input.keyUp(&event);
				break;
			case ALLEGRO_EVENT_TIMER:
				// Do Stuff!
				if (input.pausePressed()) {
					// Need a pause menu before this is done the right way
					running = false;
				}

				if (input.upPressed() || input.downPressed()) {
					selection ^= 1;
				}
				if (input.firePressed()) {
					running = false;
					if (!selection) ret = true;
				}

				input.resetChanged();

				// Handle Mouse stuff
				if (mouse_selected) {
					mouse_selected = false;
					if (mselection == 1) {
						running = false;
						ret = true;
					}
					if (mselection == 2) {
						running = false;
					}
				}
			case ALLEGRO_EVENT_DISPLAY_EXPOSE:
				redraw = true;
				break;
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				running = false;
				break;
			}
		}

		if (redraw) {
			al_use_transform(&t);
			al_clear_to_color(black);

			al_draw_multiline_ustr(font, red, cx / 2, ty, cx - 20, 80, ALLEGRO_ALIGN_CENTRE, titleStr);

			if (mselection == 1) {
				al_draw_filled_rounded_rectangle(pbx, pby, pbw, pbh, 6.0, 6.0, selected);
			}
			if (mselection == 2) {
				al_draw_filled_rounded_rectangle(qbx, qby, qbw, qbh, 6.0, 6.0, selected);
			}
			
			if (selection) {
				al_draw_text(font2, offWhite, px, py, 0, "Play");
				al_draw_text(font2, white, sqx, sqy, 0, "> Quit <");
			}
			else {
				al_draw_text(font2, white, spx, spy, 0, "> Play <");
				al_draw_text(font2, offWhite, qx, qy, 0, "Quit");
			}

			al_flip_display();
			redraw = false;
		}
	}
	al_stop_timer(timer);

	al_ustr_free(titleStr);

	al_destroy_font(font);
	al_destroy_font(font2);
	return ret;
}
