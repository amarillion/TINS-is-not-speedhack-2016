#include <assert.h>
#include "anim.h"
#include "color.h"
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <memory>

using namespace xdom;
using namespace std;

#include "util.h"

const char * defaultdirections[] = { "n", "e", "s", "w" };

class DefaultDirectionModel : public IDirectionModel
{
private:
	map<string, int> dirMap;
public:
	virtual int idToIndex(const string &id)
	{
		if (dirMap.find(id) == dirMap.end())
		{
			dirMap.insert (pair<string, int>(id, dirMap.size()));
		}

		return dirMap[id];
	}
};

std::shared_ptr<IDirectionModel> Anim::model = make_shared<DefaultDirectionModel>();

struct CascadingProps
{
	/**
	 * properties hotx and hoty start at some default,
	 * and can be overriden at any level in the XML hierarchy.
	 *
	 * This way a cascading stylesheet effect is achieved, and
	 * redundant attributes are avoided.
	 */
	int hotx;
	int hoty;
	CascadingProps () : hotx (0), hoty (0) {}

	/* check the node for overridable properties */
	void checkProperties (DomNode &n);
};

/**
 * Anim properties start at some default,
 * and can be overriden at any level in the XML hierarchy.
 *
 */
class AnimBuilder
{
private:
	Anim *temp;
	IBitmapProvider *res;

	enum { TR_FLIP, TR_VFLIP, TR_ROT };
	/* NB: props must be copied for each branch of the XML tree, so props must be pass-by-value */
	std::vector < Sequence > loadState (xdom::DomNode &i, CascadingProps props);
	Sequence loadSequence (xdom::DomNode &i, CascadingProps props); // props must be pass-by-value
	Frame loadFrame (xdom::DomNode &i, CascadingProps props); // props must be pass-by-value
	CompositePart loadPart (xdom::DomNode &i, CascadingProps props); // props must be pass-by-value
	ALLEGRO_BITMAP *getLitRle(ALLEGRO_BITMAP *input, int r, int g, int b, int a);

	/** angle only needs to be provided for TR_ROT, using 360-degree circle */
	ALLEGRO_BITMAP *getTransformRle(ALLEGRO_BITMAP *input, int type, int angle = 0);
public:
	AnimBuilder(IBitmapProvider *_res) : temp(NULL), res(_res) {}
	void loadFromXml (xdom::DomNode &n);
	Anim *get() { return temp; }
};

void CascadingProps::checkProperties (DomNode &n)
{
	if (n.attributes.find ("hotx") != n.attributes.end())
		hotx = atoi (n.attributes["hotx"].c_str());
	if (n.attributes.find ("hoty") != n.attributes.end())
		hoty = atoi (n.attributes["hoty"].c_str());
}

Anim::Anim() : frames(), sizex (0), sizey (0), sizez (0)
{
}

void Anim::loadFromXml (DomNode &n, IBitmapProvider *res, map<string, Anim*> &result)
{
	AnimBuilder builder (res);
	builder.loadFromXml (n);
	string name = n.attributes["id"];
	Anim *temp = builder.get();
	result.insert (pair<string, Anim*>(name, temp));
}

void AnimBuilder::loadFromXml (DomNode &n)
{
	assert (n.name == "anim");

	string name = n.attributes["id"];
	vector<DomNode>::iterator h;
	
	temp = new Anim();
	temp->sizex = atoi (n.attributes["dx"].c_str());
	temp->sizey = atoi (n.attributes["dy"].c_str());
	temp->sizez = atoi (n.attributes["dz"].c_str());
	
	CascadingProps props;
	props.checkProperties(n);

	h = n.children.begin();
	if (h->name == "state")
	{
		for (; h != n.children.end(); ++h)
		{
			temp->frames.push_back (loadState (*h, props));
		}
	}
	else
	{
		temp->frames.push_back (loadState (n, props));
	}
}

Sequence AnimBuilder::loadSequence (xdom::DomNode &i, CascadingProps props)
{
	Sequence result;
	result.totalLength = 0;
	result.loop = true;

	props.checkProperties(i);

	vector<DomNode>::iterator j;
	for (j = i.children.begin(); j != i.children.end(); ++j)
	{
		Frame f = loadFrame (*j, props);
		result.add (f);
	}
	assert (result.size() > 0);
	return result;
}

vector < Sequence > AnimBuilder::loadState (DomNode &i, CascadingProps props)
{
	vector < Sequence > result;
	
	vector<DomNode>::iterator h;
	
	int totalDirs = 0;

	props.checkProperties(i);

	h = i.children.begin();
	
	if (h->name == "frame" || h->name == "composite")
	{
		result.push_back (loadSequence (i, props));
		totalDirs = 1;
	}
	else if (h->name == "dir")
	{	
		for (; h != i.children.end(); ++h)
		{
			int length = 0;	
			string id = h->attributes["id"];
			int dir = Anim::model->idToIndex(id);
			if (dir < 0)
			{
				allegro_message ("Unknown direction '%s'", id.c_str());
				assert (false); // unknown tag
			}
			else
			{
				totalDirs++;
			}
			if (result.size() < (unsigned)(dir+1)) result.resize(dir+1);
			result[dir] = loadSequence (*h, props);
		}
	}
	else
	{
		allegro_message ("Unknown tag %s", h->name.c_str());
		assert (false);
	}
	
	return result;
}

void Frame::getCompositeBounds (int &sprx, int &spry, int &width, int &height) const
{
	vector<CompositePart>::const_iterator i;
	i = parts.begin();
	int x1 = i->hotx;
	int y1 = i->hoty;
	int x2 = i->hotx + al_get_bitmap_width(i->rle);
	int y2 = i->hoty + al_get_bitmap_height(i->rle);
	++i;
	// now grow those bounds if any of the next parts fall outside it
	for (; i != parts.end(); ++i)
	{
		if (i->hotx < x1) x1 = i->hotx;
		if (i->hoty < y1) y1 = i->hoty;
		if (i->hotx + al_get_bitmap_width(i->rle) > x2) x2 = i->hotx + al_get_bitmap_width(i->rle);
		if (i->hoty + al_get_bitmap_height(i->rle) > y2) y2 = i->hoty + al_get_bitmap_height(i->rle);
	}
	sprx = x1;
	spry = y1;
	width = x2 - x1;
	height = y2 - y1;
}

Frame AnimBuilder::loadFrame (DomNode &i, CascadingProps props)
{
	Frame result;
	vector<DomNode>::iterator j;
	
	props.checkProperties(i);

	int time = 0;
	if (i.attributes.find ("time") != i.attributes.end())
		time = atoi(i.attributes["time"].c_str());
	result.length = time;
	if (i.name == "frame")
	{
		// just a single part.
		CompositePart part = loadPart (i, props);
		result.parts.push_back (part);
	}
	else if (i.name == "composite")
	{
		for (j = i.children.begin(); j != i.children.end(); ++j)
		{
			// multiple parts
			CompositePart part = loadPart (*j, props);
			result.parts.push_back (part);
		}
	}
	else
	{
		assert (false); // unknown tag
	}
	
	return result;
}

ALLEGRO_BITMAP *AnimBuilder::getLitRle(ALLEGRO_BITMAP *s, int r, int g, int b, int a)
{
	assert (s);
	int w = al_get_bitmap_width(s);
	int h = al_get_bitmap_height(s);
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY_WITH_ALPHA);
	ALLEGRO_BITMAP *temp = al_create_bitmap (w, h);
	assert (temp);

	al_set_target_bitmap(temp);

	al_draw_bitmap (s, 0, 0, 0); // copy sprite
	al_draw_filled_rectangle (0, 0, w, h, al_map_rgba(r, g, b, a)); // overlay lit color in complete area
	al_set_blender (ALLEGRO_ADD, ALLEGRO_ZERO, ALLEGRO_ALPHA);
	al_draw_bitmap (s, 0, 0, 0); // copy transparent areas
	al_set_blender (ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);

	char buf[256];
	snprintf (buf, sizeof(buf), "rle_lit_%p", temp);
	res->putBitmap (buf, temp);
	return temp;
}

ALLEGRO_BITMAP *AnimBuilder::getTransformRle(ALLEGRO_BITMAP *s, int type, int angle)
{
	int w = al_get_bitmap_width(s);
	int h = al_get_bitmap_height(s);
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY_WITH_ALPHA);
	ALLEGRO_BITMAP *temp = al_create_bitmap (w, h);
	al_set_target_bitmap (temp);
	al_clear_to_color (MAGIC_PINK);

	switch (type)
	{
	case TR_FLIP: al_draw_bitmap(s, 0, 0, ALLEGRO_FLIP_HORIZONTAL); break;
	case TR_VFLIP: al_draw_bitmap(s, 0, 0, ALLEGRO_FLIP_VERTICAL); break;
	case TR_ROT: al_draw_rotated_bitmap (s, w / 2, h / 2, w / 2, h / 2, angle * ALLEGRO_PI / 180, 0); break;
	default: al_draw_bitmap (s, 0, 0, 0); break;
	}

	char buf[256];
	snprintf (buf, sizeof(buf), "rle_transform_%p", temp);
	res->putBitmap (buf, temp);
	return temp;
}

CompositePart AnimBuilder::loadPart (DomNode &i, CascadingProps props)
{
	CompositePart result;
	
	props.checkProperties(i);
	result.hotx = props.hotx;
	result.hoty = props.hoty;
	
	// handle "lit" effect
	ALLEGRO_BITMAP *temprle = res->getBitmap (i.attributes["rle"]);
	if (!temprle)
	{
		allegro_message ("error loading animation frame %s", i.attributes["rle"].c_str());
	}

	vector<DomNode>::iterator m;
	for (m = i.children.begin(); m != i.children.end(); ++m)
	{
		if (m->name == "lit")
		{
			int r = atoi (m->attributes["r"].c_str());
			int g = atoi (m->attributes["g"].c_str());
			int b = atoi (m->attributes["b"].c_str());
			int a = 255; // default alpha is 255
			if (m->attributes.find ("alpha") != m->attributes.end())
				a = atoi (m->attributes["alpha"].c_str());
			temprle = getLitRle (temprle, r, g, b, a);
		}
		else if (m->name == "flip")
		{
			temprle = getTransformRle (temprle, TR_FLIP);
		}
		else if (m->name == "vflip")
		{
			temprle = getTransformRle (temprle, TR_VFLIP);
		}
		else if (m->name == "rot")
		{
			int a = atoi (m->attributes["angle"].c_str());
			if (a >= 0 && a <= 360) temprle = getTransformRle (temprle, TR_ROT, a);
			else allegro_message ("Invalid rotation %i", a);
		}
	}

	if (!temprle)
	{
		allegro_message ("error loading animation frame %s", i.attributes["rle"].c_str());
	}
	result.rle = temprle;

	return result;
}

void Frame::draw(ALLEGRO_BITMAP * buf, int x, int y) const
{
	vector<CompositePart>::const_iterator i;
	al_set_target_bitmap (buf);
	for (i = parts.begin(); i != parts.end(); ++i)
	{
		al_draw_bitmap (i->rle, x + i->hotx, y + i->hoty, 0);
	}
}

const Frame *Sequence::getFrame (int time) const
{
	if (loop && totalLength > 0)
	{
		time %= totalLength;
	}
	
	unsigned int i = 0;
	while ((i < frames.size()) && (time > frames[i].cumulative)) { i++; }
	if (i == frames.size()) i--;
	
	return &frames[i];
}

const Frame *Anim::getFrame (unsigned int state, unsigned int dir, int time) const
{
	assert (frames.size() > 0);
	assert (state < frames.size());
	dir %= frames[state].size();
	const Sequence &s = frames[state][dir];
	return s.getFrame (time);
}

void Anim::drawFrame (ALLEGRO_BITMAP *buf, unsigned int state, unsigned int dir, int time, int x, int y) const
{
	const Frame *current = getFrame (state, dir, time);
	current->draw (buf, x, y);
}

void Sequence::add (Frame &f)
{
	totalLength += f.length;
	f.cumulative = totalLength;
	frames.push_back (f);
}

void Anim::add (Frame &f, unsigned int state, unsigned int dir)
{
	if (state >= frames.size()) frames.resize(state + 1);
	if (dir >= frames[state].size()) frames[state].resize(dir + 1);
	frames[state][dir].add(f);
}
