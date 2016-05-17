#ifndef TEXT_H
#define TEXT_H

#include "component.h"
#include "DrawStrategy.h"
#include <vector>
#include "ComponentBuilder.h"
#include <allegro5/allegro_font.h>

class Text : public IComponent
{
private:
	std::vector<std::string> elems;
	ALLEGRO_COLOR color;
	int align;

	void split (const std::string &s);
public:
	static ComponentBuilder build(ALLEGRO_COLOR _color, std::string val);
	static ComponentBuilder build(ALLEGRO_COLOR _color, int _align, std::string val);
	static ComponentBuilder buildf(ALLEGRO_COLOR _color, int _align, const char *msg, ...);
	static ComponentBuilder buildf(ALLEGRO_COLOR _color, const char *msg, ...);

	Text(ALLEGRO_COLOR _color, int _align, const std::string &_s) : elems(), color(_color), align(_align)
	{
		split (_s);
		showlines = elems.size();
		blink = false;
	}

	virtual void draw(const GraphicsContext &gc);
	int showlines;
	int lineh;
	bool blink;
	int size() { return elems.size(); }
};


// for animated text
//TODO: rename to distinguish from class "Text"
class TextComponent : public Text
{
public:
	TextComponent (ALLEGRO_COLOR _color, int _align, const std::string &_s) : Text(_color, _align, _s) {
		showlines = 1;
	}

	virtual void update()
	{
		IComponent::update();
		if (counter % 48 == 0)
		{
			showlines++;
			if (showlines > size())
			{
				pushMsg(-1);
			}
		}
	}
};

typedef std::shared_ptr<TextComponent> textComponentPtr;

#endif
