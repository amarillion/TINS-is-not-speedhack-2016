#include "textstyle.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <cstdio>
#include <string>
#include "strutil.h"
#include <iostream>

void draw_textf_with_background(ALLEGRO_FONT *font, ALLEGRO_COLOR text_color, ALLEGRO_COLOR background_color, int x, int y, int alignment, const char *text, ...)
{
	char buf[4096];
	va_list ap;
	va_start(ap, text);
	vsnprintf(buf, sizeof(buf), text, ap);
	va_end (ap);
	draw_text_with_background (font, text_color, background_color, x, y, alignment, buf);
}

void draw_text_with_background(ALLEGRO_FONT *font, ALLEGRO_COLOR text_color, ALLEGRO_COLOR background_color, int x, int y, int alignment, const char *text)
{
	int textw = al_get_text_width(font, text);
	int texth = al_get_font_line_height(font);

	int x1 = 0, x2 = 0;

	switch (alignment)
	{
	case ALLEGRO_ALIGN_LEFT:
		x1 = x;
		x2 = x + textw;
		break;
	case ALLEGRO_ALIGN_RIGHT:
		x1 = x - textw;
		x2 = x;
		break;
	case ALLEGRO_ALIGN_CENTER:
		x1 = x - (textw / 2);
		x2 = x + (textw / 2);
		break;
	default:
		assert (false);
		break;
	}

	//TODO: text with background fill function.
	al_draw_filled_rectangle (x1, y, x2, y + texth, background_color);
	al_draw_text (font, text_color, x, y, alignment, text);
}

void draw_shaded_text(ALLEGRO_FONT *font, ALLEGRO_COLOR text_color, ALLEGRO_COLOR shade_color, float x, float y, int alignment, const char *text)
{
	al_draw_text (font, shade_color, x+1, y+1, ALLEGRO_ALIGN_CENTER, text);
	al_draw_text (font, text_color, x, y,  ALLEGRO_ALIGN_CENTER, text);
}

void draw_shaded_textf(ALLEGRO_FONT *font, ALLEGRO_COLOR text_color, ALLEGRO_COLOR shade_color, float x, float y, int alignment, const char *text, ...)
{
	char buf[4096];
	va_list ap;
	va_start(ap, text);
	vsnprintf(buf, sizeof(buf), text, ap);
	va_end (ap);
	draw_shaded_text (font, text_color, shade_color, x, y, alignment, buf);
}

