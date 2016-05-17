#ifndef ISOMETRIC_H_
#define ISOMETRIC_H_

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <assert.h>
#include <math.h>
#include "graphicscontext.h"

extern float LIGHTX;
extern float LIGHTY;
extern float LIGHTZ;

const int TILEX = 32;
const int TILEY = 32;
const int TILEZ = 32;

/**
 * Draw a surface.
 *
 * z is the height, in tile units, of the top corner.
 *
 * dzleft, dzright and dzbot are the z-delta, in tile units, of the left,
 * right and bottom corners
 */
struct Cell
{
	int z;
	short dzleft;
	short dzright;
	short dzbot;
	Cell() { z = 0; dzleft= 0; dzright = 0; dzbot = 0; }
};

/**
 * The return value is between -1 and 1.
 */
float surfaceLighting(float x1, float y1, float z1, float x2, float y2, float z2);
ALLEGRO_COLOR litColor (ALLEGRO_COLOR color, float light);

template <class T> class Map
{
protected:
	T *data;
	int sizex;
	int sizey;
public:
	Map (int w, int h)
	{
		assert (w >= 0);
		assert (h >= 0);
		data = new T[w * h];
		sizex = w;
		sizey = h;
	}

	Map ()
	{
		data = NULL;
		sizex = 0;
		sizey = 0;
	}

	virtual ~Map()
	{
		if (data) delete[] data;
		data = NULL;
	}

	T &get (int x, int y)
	{
		assert (x >= 0);
		assert (y >= 0);
		assert (x < sizex);
		assert (y < sizey);
		assert (data);
		return data[x + sizex * y];
	}

	void init (int w, int h)
	{
		if (data)
			delete[] data;

		sizex = w;
		sizey = h;
		data = new T[sizex * sizey];
	}

	int getSizeX() { return sizex; }
	int getSizeY() { return sizey; }

	int canvasFromMapX (int mx, int my) const
	{
		return getXorig() + mx * 32 + my * -32;
	}

	int canvasFromMapY (int mx, int my) const
	{
		return getYorig() + mx * 16 + my * 16;
	}

	int canvasFromMapX (float mx, float my) const
	{
		return (int)(getXorig() + mx * 32.0 + my * -32.0);
	}

	int canvasFromMapY (float mx, float my) const
	{
		return (int)(getYorig() + mx * 16.0 + my * 16.0);
	}

	int mapFromCanvasX (int x, int y) const
	{
		return ((x - getXorig()) / 2 + (y - getYorig())) / 32;
	}

	int mapFromCanvasY (int x, int y) const
	{
		return  (y - getYorig() - (x - getXorig()) / 2) / 32;
	}

	int getw() const { return sizex * 64; }
	int geth() const { return sizey * 32; }

	/** distance from the cell at 0,0 to the edge of the virtual screen */
	int getXorig () const { return getw() / 2 + 32; }

	/** distance from the cell at 0,0 to the edge of the virtual screen */
	int getYorig () const { return 64; }

	void canvasFromIso_f (float x, float y, float z, float &rx, float &ry)
	{
		rx = getXorig() + (x - y);
		ry = getYorig() + (x * 0.5 + y * 0.5 - z);
	}

	void drawSurface(GraphicsContext *gc, int mx, int my, T &c)
	{
		al_set_target_bitmap (gc->buffer);

		ALLEGRO_VERTEX coord[4]; // hold memory for coordinates
		ALLEGRO_VERTEX *surf1[3] = { &coord[0], &coord[1], &coord[3] };
		ALLEGRO_VERTEX *surf2[3] = { &coord[1], &coord[2], &coord[3] };

		ALLEGRO_COLOR baseColor = al_map_rgb (192, 255, 192);

		canvasFromIso_f (	TILEX * mx,
						TILEY * my,
						TILEZ * c.z,
						coord[0].x, coord[0].y);

		canvasFromIso_f (	TILEX * (mx + 1),
						TILEY * my,
						TILEZ * (c.z + c.dzright),
						coord[1].x, coord[1].y);

		canvasFromIso_f (	TILEX * (mx + 1),
						TILEY * (my + 1),
						TILEZ * (c.z + c.dzbot),
						coord[2].x, coord[2].y);

		canvasFromIso_f (	TILEX * mx,
						TILEY * (my + 1),
						TILEZ * (c.z + c.dzleft),
						coord[3].x, coord[3].y);

		int color = litColor (baseColor,
				surfaceLighting (1, 0, c.dzright, 0, 1, c.dzleft) );

		for (int i = 0; i < 4; ++i)
		{
			coord[i].x -= gc->xofst;
			coord[i].y -= gc->yofst;
			coord[i].color = color;
		}


		al_draw_prim(surf1, NULL, NULL, 0, 3, /* POLYTYPE_FLAT */ ALLEGRO_PRIM_POINT_LIST);

		color = litColor (baseColor,
					surfaceLighting (0, -1, c.dzright - c.dzbot, -1, 0, c.dzleft - c.dzbot) );

		for (int i = 0; i < 4; ++i)
		{
			coord[i].color = color;
		}

		al_draw_prim(surf2, NULL, NULL, 0, 3, /* POLYTYPE_FLAT */ ALLEGRO_PRIM_POINT_LIST);
	}

	void drawLeftWall(GraphicsContext *gc, int mx, int my, T &c)
	{
		int x[4];
		int y[4];
		int z[4];

		x[0] = TILEX * (mx + 1);
		y[0] = TILEY * (my + 1);
		z[0] = 0;

		x[1] = TILEX * mx;
		y[1] = TILEY * (my + 1);
		z[1] = 0;

		x[2] = TILEX * mx;
		y[2] = TILEY * (my + 1);
		z[2] = TILEZ * (c.z + c.dzleft);

		x[3] = TILEX * (mx + 1);
		y[3] = TILEY * (my + 1);
		z[3] = TILEZ * (c.z + c.dzbot);

		ALLEGRO_COLOR color = litColor (al_map_rgb (192, 192, 192),
					surfaceLighting (0, 1, 0, 0, 0, 1 ));

		drawIsoPoly(gc, 4, x, y, z, color);
	}

	void drawRightWall(GraphicsContext *gc, int mx, int my, T &c)
	{
		int x[4];
		int y[4];
		int z[4];

		x[0] = TILEX * (mx + 1);
		y[0] = TILEY * my;
		z[0] = 0;

		x[1] = TILEX * (mx + 1);
		y[1] = TILEY * (my + 1);
		z[1] = 0;

		x[2] = TILEX * (mx + 1);
		y[2] = TILEY * (my + 1);
		z[2] = TILEZ * (c.z + c.dzbot);

		x[3] = TILEX * (mx + 1);
		y[3] = TILEY * my;
		z[3] = TILEZ * (c.z + c.dzright);

		int color = litColor (al_map_rgb (192, 192, 192),
					surfaceLighting (0, 0, 1, -1, 0, 0) );

		drawIsoPoly(gc, 4, x, y, z, color);
	}

	void drawIsoPoly (GraphicsContext *gc, int num, int x[], int y[], int z[], int color)
	{
		const int BUF_SIZE = 20; // max 20 points
		assert (num <= BUF_SIZE);

		ALLEGRO_VERTEX coord[BUF_SIZE]; // hold actual objects
		ALLEGRO_VERTEX *pcoord[BUF_SIZE]; // hold array of pointers

		// initialize pointers to point to objects
		for (int i = 0; i < BUF_SIZE; ++i) { pcoord[i] = &coord[i]; }

		for (int i = 0; i < num; ++i)
		{
			canvasFromIso_f (x[i], y[i], z[i], coord[i].x, coord[i].y);
			coord[i].x -= gc->xofst;
			coord[i].y -= gc->yofst;
			coord[i].color = color;
		}

		al_set_target_bitmap (gc->buffer);
		al_draw_prim (pcoord, NULL, NULL, 0, num, /* POLYTYPE_FLAT */ ALLEGRO_PRIM_POINT_LIST);
	}


};

/**
	draw wire model
	to help testing

	the wireFrame fills the cube from iso (0,0,0) to iso (sx, sy, sz),
	with iso (0, 0, 0) positioned at screen (rx, ry)
*/
void drawWireFrame (int rx, int ry, int sx, int sy, int sz, ALLEGRO_COLOR color);

void drawIsoPoly (GraphicsContext *gc, int num, int x[], int y[], int z[], int color);

// assume z == 0
void screenToIso (int rx, int ry, float &x, float &y);

/* transform x, y and z from isometric to screen coordinates and put them in rx, ry */
void isoToScreen (float x, float y, float z, int &rx, int &ry);

int isoToScreenX (float x, float y, float z);
int isoToScreenY (float x, float y, float z);


/* same as isoToScreen but with floats */
void isoToScreen_f (float x, float y, float z, float &rx, float &ry);

template <class T> void drawMap(GraphicsContext *gc, Map<T> *map)
{
	for (int mx = 0; mx < map->getSizeX(); ++mx)
		for (int my = 0; my < map->getSizeY(); ++my)
		{
			T &c = map->get(mx, my);

			map->drawSurface (gc, mx, my, c);
			map->drawLeftWall (gc, mx, my, c);
			map->drawRightWall (gc, mx, my, c);
		}

}

#endif /* ISOMETRIC_H_ */

