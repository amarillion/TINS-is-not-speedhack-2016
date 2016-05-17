#ifndef DRAWSTRATEGY_H_
#define DRAWSTRATEGY_H_

#include "component.h"
#include <memory>
#include "graphicscontext.h"
#include <allegro5/allegro.h>
#include "ComponentBuilder.h"

struct ALLEGRO_BITMAP;

#ifdef USE_TEGEL
struct TEG_MAP;
#endif

class ClearScreen : public IComponent
{
public:
	ClearScreen(ALLEGRO_COLOR color) { this->color = color; }
	virtual void draw(const GraphicsContext &gc);
	static ComponentBuilder build(ALLEGRO_COLOR color);
private:
	ALLEGRO_COLOR color;
};

class Pattern : public IComponent
{
public:
	Pattern(ALLEGRO_BITMAP *bmp) : texture(bmp), textureHolder() {}
	Pattern(std::shared_ptr<ALLEGRO_BITMAP> _bmp) : texture(_bmp.get()), textureHolder(_bmp) {}

	virtual void draw(const GraphicsContext &gc);
	static ComponentBuilder build(ALLEGRO_BITMAP *bmp);
	static ComponentBuilder build(std::shared_ptr<ALLEGRO_BITMAP> bmp);
private:

	/**
	 * Actual reference used for drawing in held and non-held situation
	 */
	ALLEGRO_BITMAP *texture;

	/**
	 * For owned bitmaps, this will ensure that texture is deleted
	 */
	std::shared_ptr<ALLEGRO_BITMAP> textureHolder;
};

class BitmapComp : public IComponent
{
private:

	/**
	 * For owned bitmaps, this will ensure that rle is deleted
	 */
	std::shared_ptr<ALLEGRO_BITMAP> rleHolder;

	/**
	 * Actual reference used for drawing in held and non-held situation
	 */
	ALLEGRO_BITMAP *rle;

	double zoom;
	bool isZoomed;
public:

	/**
	 * Call this constructor when ownership is transferred to BitmapComp. BitmapComp will ensure al_destroy_bitmap is called.
	 */
	BitmapComp(std::shared_ptr<ALLEGRO_BITMAP> _rle) : rleHolder(_rle), rle(_rle.get()), zoom(1.0), isZoomed(false) {}

	/**
	 * Call this constructor when ownership of bitmap is not transferred
	 */
	BitmapComp(ALLEGRO_BITMAP*  _rle) : rleHolder(), rle(_rle), zoom(1.0), isZoomed(false) {}

	virtual void draw(const GraphicsContext &gc);

	/**
	 * Call this builder when ownership is transferred to BitmapComp. BitmapComp will ensure al_destroy_bitmap is called.
	 */
	static ComponentBuilder build(std::shared_ptr<ALLEGRO_BITMAP> _rle);

	/**
	 * Call this builder when ownership of bitmap is not transferred
	 */
	static ComponentBuilder build(ALLEGRO_BITMAP *_rle);

	void setZoom(double _zoom) { zoom = _zoom; isZoomed = true; }
	double getZoom() { return zoom; }
};

#ifdef USE_TEGEL
class TileMap : public IComponent
{
public:
	TileMap(TEG_MAP *tilemap) { this->tilemap = tilemap; }
	virtual void draw(const GraphicsContext &gc);
	static ComponentBuilder build(TEG_MAP *map);
private:
	TEG_MAP *tilemap;
};
#endif


class Anim;

class AnimComponent : public IComponent
{
private:
	int exitCode;
	Anim *anim;
	int dir;
	int state;
public:
	static ComponentBuilder build(Anim *anim);
	AnimComponent (Anim *anim);
	virtual void draw(const GraphicsContext &gc) override;
};


#endif /* DRAWSTRATEGY_H_ */
