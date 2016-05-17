module io;

import allegro5.allegro;

/// Interface Object
class Io
{
	Io parent;
	Io [] child;
	int relX;
	int relY;
	int w;
	int h;

	this (Io parent_, int relX_, int relY_, int w_, int h_)
	{
		parent = parent_;
		if (parent !is null)
		{
			parent.child ~= this;
		}
		relX = relX_;
		relY = relY_;
		w = w_;
		h = h_;
	}

	final void draw ()
	{
		ALLEGRO_BITMAP * prevBuffer = al_get_target_bitmap ();
		ALLEGRO_BITMAP * curBuffer = al_create_sub_bitmap
		    (prevBuffer, relX, relY, w, h);
		al_set_target_bitmap (curBuffer);
		drawThisPre ();
		foreach (c; child)
		{
			c.draw ();
		}
		drawThisPost ();
		al_set_target_bitmap (prevBuffer);
		al_destroy_bitmap (curBuffer);
	}

	void drawThisPre ()
	{
	}

	void drawThisPost ()
	{
	}

	final bool coordPostOrder (alias fun) (int posX, int posY)
	{
		posX -= relX;
		posY -= relY;
		if (posX < 0 || w <= posX || posY < 0 || h <= posY)
		{
			return false;
		}
		foreach_reverse (c; child)
		{
			if (c.coordPostOrder !(fun) (posX, posY))
			{
				return true;
			}
		}
		return fun (posX, posY);
	}

	final bool click (int posX, int posY)
	{
		return coordPostOrder !(clickThis) (posX, posY);
	}

	bool clickThis (int posX, int posY)
	{
		return false;
	}

	final bool unclick (int posX, int posY)
	{
		return coordPostOrder !(unclickThis) (posX, posY);
	}

	bool unclickThis (int posX, int posY)
	{
		return false;
	}
}
