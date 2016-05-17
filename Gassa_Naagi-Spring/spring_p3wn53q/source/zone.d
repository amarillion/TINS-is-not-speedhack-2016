module zone;
import io;

import allegro5.allegro;
import allegro5.allegro_primitives;

class Zone : Io
{
	ALLEGRO_COLOR fillColor;
	int radius;

	this (Io parent_, int relX_, int relY_, int w_, int h_, int radius_,
	    ALLEGRO_COLOR fillColor_)
	{
		super (parent_, relX_, relY_, w_, h_);
		radius = radius_;
		fillColor = fillColor_;
	}

	override void drawThisPre ()
	{
		al_draw_filled_rounded_rectangle (0, 0, w, h,
		    radius, radius, fillColor);
	}
}
