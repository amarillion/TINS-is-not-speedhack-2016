module slot;
import board;
import draggable;
import io;
import spring;
import zone;

import std.range;

import allegro5.allegro;
import allegro5.allegro_primitives;

class Slot : Board
{
	ALLEGRO_COLOR matchColor;
	string matchText;
	int wPrev;
	bool hasMatch;

	this (Io parent_, int relX_, int relY_, int w_, int h_, int radius_,
	    ALLEGRO_COLOR fillColor_, ALLEGRO_COLOR matchColor_,
	    string matchText_)
	{
		super (parent_, relX_, relY_, w_, h_, radius_, fillColor_);
		matchColor = matchColor_;
		matchText = matchText_;
		hasMatch = false;
	}

	void recalcWidth ()
	{
		foreach (sibling; parent.child)
		{
			if (cast (Slot) sibling !is null &&
			    sibling.relY == relY &&
			    sibling.relX > relX)
			{
				sibling.relX += w - wPrev;
			}
		}
	}

	override bool unclickThis (int posX, int posY)
	{
		if (!child.empty)
		{
			return false;
		}
		if (cursorRoot !is null)
		{
			cursorRoot.relX = 2;
			cursorRoot.relY = 2;
			wPrev = w;
			w = cursorRoot.w + 4;
			recalcWidth ();
			child ~= cursorRoot;
			cursorRoot.parent = this;
			auto cur = cast (Draggable) child.front;
			hasMatch = (cur !is null &&
			    matchText == cur.matchText);
			recalcNextButton ();
			cursorRoot = null;
			return true;
		}
		return false;
	}

	override void drawThisPost ()
	{
		if (hasMatch)
		{
			al_draw_filled_rounded_rectangle (0, 0, w, h,
			    radius, radius, matchColor);
		}
	}
}
