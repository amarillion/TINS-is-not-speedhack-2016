module draggable;
import io;
import slot;
import spring;
import text_zone;

import std.algorithm;
import std.array;

import allegro5.allegro;
import allegro5.allegro_font;

class Draggable : TextZone
{
	string matchText;

	this (Io parent_, int relX_, int relY_, int w_, int h_, int radius_,
	    ALLEGRO_COLOR fillColor_, ALLEGRO_COLOR textColor_,
	    ALLEGRO_FONT * textFont_, string matchText_)
	{
		matchText = matchText_;
		auto centerText_ = matchText.toAllegroUstr ();
		super (parent_, relX_, relY_, w_, h_, radius_, fillColor_,
		textColor_, textFont_, centerText_);
	}

	override bool clickThis (int posX, int posY)
	{
		if (cursorRoot !is null)
		{
			return false;
		}
		parent.child = parent.child.filter !(x => x !is this).array;
		relX = 0;
		relY = 0;
		cursorRoot = this;
		auto cur = cast (Slot) parent;
		if (cur !is null)
		{
			swap (cur.w, cur.wPrev);
			cur.recalcWidth ();
			cur.hasMatch = false;
			recalcNextButton ();
		}
		this.parent = null;
		return true;
	}
}
