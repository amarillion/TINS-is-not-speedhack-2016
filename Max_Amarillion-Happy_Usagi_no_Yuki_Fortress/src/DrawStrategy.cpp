/*
 * DrawStrategy.cpp
 *
 *  Created on: 3 Aug 2012
 *      Author: martijn
 */

#include "DrawStrategy.h"
#include "color.h"
#include <allegro5/allegro.h>
#include "component.h"
#ifdef USE_TEGEL
#include <tegel5.h>
#endif
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "anim.h"

using namespace std;

ComponentBuilder ClearScreen::build(ALLEGRO_COLOR color)
{
	return ComponentBuilder(make_shared<ClearScreen>(color));
}

void ClearScreen::draw(const GraphicsContext &gc)
{
	al_set_target_bitmap (gc.buffer);
	al_clear_to_color(color);
}

ComponentBuilder Pattern::build(ALLEGRO_BITMAP *bmp)
{
	return ComponentBuilder(make_shared<Pattern>(bmp));
}

ComponentBuilder Pattern::build(shared_ptr<ALLEGRO_BITMAP> bmp)
{
	return ComponentBuilder(make_shared<Pattern>(bmp));
}

void Pattern::draw(const GraphicsContext &gc)
{
	int xofst = -gc.xofst;
	int yofst = -gc.yofst;

	if (motion)
	{
		xofst = motion->getdx(counter);
		yofst = motion->getdy(counter);
	}

	int TILEW = al_get_bitmap_width(texture);
	int TILEH = al_get_bitmap_height(texture);

	al_set_target_bitmap (gc.buffer);

	for (int x = (xofst % TILEW) - TILEW; x < al_get_bitmap_width(gc.buffer) + TILEW; x += TILEW)
	{
		for (int y = (yofst % TILEH) - TILEH; y < al_get_bitmap_height(gc.buffer) + TILEH; y += TILEH)
		{
			al_draw_bitmap (texture, x, y, 0);
		}
	}
}

#ifdef USE_TEGEL

void TileMap::draw(const GraphicsContext &gc)
{
	int xofst = 0;
	int yofst = 0;
	int counter = 0;

	if (motion)
	{
		xofst = motion->getdx(counter);
		yofst = motion->getdy(counter);
	}

	xofst += gc.xofst;
	yofst += gc.yofst;

	int frame = (counter / 20) % tilemap->tilelist->animsteps;
//	teg_draw_frame (tilemap, 0, xofst, yofst, frame);
	teg_draw_repeated(gc.buffer, tilemap, 0, xofst, yofst, frame);
}

ComponentBuilder TileMap::build(TEG_MAP *map)
{
	return ComponentBuilder(make_shared<TileMap>(map));
}

#endif

void BitmapComp::draw (const GraphicsContext &gc)
{
	int xofst = 0;
	int yofst = 0;

	xofst = getx() + (motion ? motion->getdx(counter) : 0);
	yofst = gety() + (motion ? motion->getdy(counter) : 0);

	xofst += gc.xofst;
	yofst += gc.yofst;

	if (isZoomed)
	{
		int origw = al_get_bitmap_width(rle);
		int origh = al_get_bitmap_height(rle);
		int neww = origw * zoom;
		int newh = origh * zoom;

		al_draw_scaled_bitmap(rle,
			0, 0, origw, origh,
			xofst - neww / 2, yofst - newh / 8, neww, newh, 0);
	}
	else
	{
		al_set_target_bitmap (gc.buffer);
		al_draw_bitmap(rle, xofst, yofst, 0);
	}

}

ComponentBuilder BitmapComp::build(shared_ptr<ALLEGRO_BITMAP> rle)
{
	return ComponentBuilder(make_shared<BitmapComp>(rle));
}

ComponentBuilder BitmapComp::build(ALLEGRO_BITMAP *_rle)
{
	return ComponentBuilder(make_shared<BitmapComp>(_rle));
}


ComponentBuilder AnimComponent::build(Anim *anim)
{
	return ComponentBuilder(make_shared<AnimComponent>(anim));
}

AnimComponent::AnimComponent (Anim *anim) : exitCode(0), anim(anim), dir(0), state(0) {}

void AnimComponent::draw(const GraphicsContext &gc)
{
	anim->drawFrame(gc.buffer, state, dir, (counter * 20), getx(), gety());
}
