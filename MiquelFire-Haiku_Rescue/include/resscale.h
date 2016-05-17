#ifndef RESSCALE_H
#define RESSCALE_H

#include <allegro5/allegro.h>

/**
	Generate some values to make resolution independant transforms.
*/
class ResScale {
public:
	/**
		Target a certain res
		Default uses the current target bitmap for scaling
	*/
	ResScale(float tw, float th);
	ResScale(ALLEGRO_BITMAP *bitmap, float tw, float th);
	ResScale(ALLEGRO_DISPLAY *display, float tw, float th);

	void getTransform(ALLEGRO_TRANSFORM *t);
	float getOffsetx() { return offX; }
	float getOffsety() { return offY; }
private:
	void init(float w, float h, float tw, float th);

	ALLEGRO_TRANSFORM trans;
	float offY, offX;
};

#endif // RESSCALE_H