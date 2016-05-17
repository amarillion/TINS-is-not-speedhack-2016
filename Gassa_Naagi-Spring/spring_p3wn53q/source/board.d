module board;
import io;
import spring;
import zone;

import allegro5.allegro;

class Board : Zone
{
	this (Io parent_, int relX_, int relY_, int w_, int h_, int radius_,
	    ALLEGRO_COLOR fillColor_)
	{
		super (parent_, relX_, relY_, w_, h_, radius_, fillColor_);
	}

	override bool unclickThis (int posX, int posY)
	{
		if (cursorRoot !is null)
		{
			cursorRoot.relX = posX - cursorRoot.w / 2;
			cursorRoot.relY = posY - cursorRoot.h / 2;
			child ~= cursorRoot;
			cursorRoot.parent = this;
			cursorRoot = null;
			return true;
		}
		return false;
	}
}
