
namespace Style
{

enum BUTTON_STATE
{
  NORMAL = 0,
  SELECTED,
  ERROR,
};

ALLEGRO_COLOR get_state_color(BUTTON_STATE state)
{
  switch(state)
  {
    case NORMAL:
      return color::white;
      break;
    case SELECTED:
      return color::dodgerblue;
      break;
    case ERROR:
      return color::red;
      break;
    default:
      return color::navajowhite;
      break;
  }
}

void draw_button(BUTTON_STATE state, float x, float y, float w, float h, std::string text="", ALLEGRO_BITMAP *bitmap=NULL)
{
  int inset = 4;

  ALLEGRO_COLOR col = get_state_color(state);

  al_draw_filled_rounded_rectangle(x+inset, y+inset, w-inset, h-inset, 4, 4, color::color(color::black, 0.4));

  if (bitmap)
  {
    BitmapObject image(bitmap);
    image.scale_to_fit(w, h);
    image.align(0.5, 0.5);
    image.position(w/2, h/2);
    image.draw();
  }

  if (!text.empty())
  {
    ALLEGRO_FONT *f = af::fonts["space age.otf 34"];
    al_draw_text(f, color::white, w/2, h/2-al_get_font_line_height(f)/2, ALLEGRO_ALIGN_CENTER, text.c_str());
  }

  al_draw_rounded_rectangle(x, y, w, h, 8, 8, col, 2.0);
}

void draw_text_box(float x, float y, float w, float h, std::string text="")
{
  int inset = 4;

  ALLEGRO_COLOR col = color::aquamarine;
  al_draw_filled_rounded_rectangle(x+inset, y+inset, w-inset, h-inset, 4, 4, color::color(col, 0.4));

  if (!text.empty())
  {
    ALLEGRO_FONT *f = af::fonts["DroidSans.ttf 34"];
    al_draw_multiline_text(f, color::black, x+20, y+10, w-20, h, 0, text.c_str());
  }

  al_draw_rounded_rectangle(x, y, w, h, 8, 8, col, 2.0);
}

void draw_button(BUTTON_STATE state, const placement2d &place, std::string text="", ALLEGRO_BITMAP *bitmap=NULL)
{
  draw_button(state, 0, 0, place.size.x, place.size.y, text, bitmap);
}

};

