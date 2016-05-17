#include "text.h"
#include <string>
#include <sstream>
#include "color.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

using namespace std;

ComponentBuilder Text::build(ALLEGRO_COLOR _color, std::string val)
{
	return build(_color, ALLEGRO_ALIGN_CENTER, val);
}

ComponentBuilder Text::build(ALLEGRO_COLOR _color, int _align, std::string val)
{
	return ComponentBuilder(make_shared<Text>(_color, _align, val));
}

ComponentBuilder Text::buildf(ALLEGRO_COLOR _color, int _align, const char *msg, ...)
{
	char buf[256];

	va_list ap;
	va_start(ap, msg);
	vsnprintf (buf, sizeof(buf), msg, ap);
	va_end(ap);

	return ComponentBuilder(make_shared<Text>(_color, _align, buf));
}

ComponentBuilder Text::buildf(ALLEGRO_COLOR _color, const char *msg, ...)
{
	char buf[256];

	va_list ap;
	va_start(ap, msg);
	vsnprintf (buf, sizeof(buf), msg, ap);
	va_end(ap);

	return ComponentBuilder(make_shared<Text>(_color, ALLEGRO_ALIGN_CENTER, buf));
}


void Text::split(const string &s)
{
	stringstream ss(s);
	string item;
	while(std::getline(ss, item, '\n')) {
		elems.push_back(item);
	}
}

void Text::draw(const GraphicsContext &gc)
{
	IMotionPtr motion;

	int xofst = motion ? motion->getdx(counter) : 0;
	int yofst = motion ? motion->getdy(counter) : 0;

	if (!sfont) return; //TODO: warn("Attempt to draw text without a valid font")

	int yco = gety();
	int lineh = al_get_font_line_height(sfont);
	al_set_target_bitmap (gc.buffer);
	int remain = showlines;
	for (vector<string>::iterator i = elems.begin(); i != elems.end() && remain > 0; ++i, remain--)
	{
		al_draw_text(sfont, color, getx() + xofst, yco + yofst, align, (*i).c_str());
		yco += lineh;
	}
}
