#include "resscale.h"

ResScale::ResScale(float tw, float th) {
	float w, h;
	ALLEGRO_BITMAP *bitmap;

	bitmap = al_get_target_bitmap();
	w = al_get_bitmap_width(bitmap);
	h = al_get_bitmap_height(bitmap);
	init(w, h, tw, th);
}

ResScale::ResScale(ALLEGRO_DISPLAY *display, float tw, float th) {
	float w, h;

	w = al_get_display_width(display);
	h = al_get_display_height(display);
	init(w, h, tw, th);
}

ResScale::ResScale(ALLEGRO_BITMAP *bitmap, float tw, float th) {
	float w, h;

	w = al_get_bitmap_width(bitmap);
	h = al_get_bitmap_height(bitmap);
	init(w, h, tw, th);
}

void ResScale::init(float w, float h, float tw, float th) {
	float ar = w / h, tar = tw / th, sw = w / tw, sh = h / th;

	offX = offY = 0.0;
	al_identity_transform(&trans);

	if (ar > tar) {
		al_scale_transform(&trans, sh, sh);
		offX = (w - tw * sh) / 2 / sh;
	}
	else {
		al_scale_transform(&trans, sw, sw);
		offY = (h - tw * sw) / 2 / sw;
	}
}

void ResScale::getTransform(ALLEGRO_TRANSFORM *t) {
	al_copy_transform(t, &trans);
}
