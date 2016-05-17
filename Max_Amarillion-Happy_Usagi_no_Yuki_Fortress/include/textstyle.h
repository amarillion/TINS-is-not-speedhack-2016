#ifndef TEXTSTYLE_H
#define TEXTSTYLE_H

struct ALLEGRO_FONT;
struct ALLEGRO_COLOR;

void draw_textf_with_background(ALLEGRO_FONT *font, ALLEGRO_COLOR text_color, ALLEGRO_COLOR background_color, int x, int y, int alignment, const char *text, ...);
void draw_text_with_background(ALLEGRO_FONT *font, ALLEGRO_COLOR text_color, ALLEGRO_COLOR background_color, int x, int y, int alignment, const char *text);

void draw_shaded_textf(ALLEGRO_FONT *font, ALLEGRO_COLOR text_color, ALLEGRO_COLOR shade_color, float x, float y, int alignment, const char *text, ...);
void draw_shaded_text(ALLEGRO_FONT *font, ALLEGRO_COLOR text_color, ALLEGRO_COLOR shade_color, float x, float y, int alignment, const char *text);


#endif
