#ifndef ISOCANVAS_H_
#define ISOCANVAS_H_

#include "isometric.h"
#include "color.h"
#include "component.h"

/*
 * implements coordinate system for a isometric grid system
 *
 * does not implement draw method, does not contain map.
 * but has handy transformation methods.
 *
 * Also has many drawing primitives
 */
class IsoGrid : public IComponent
{
protected:
	int sizex;
	int sizey;
	int sizez;

	int TILEX;
	int TILEY;
	int TILEZ;

	int rx0; // location of origin
	int ry0;

	/*
	 *
	 * coordinate systems:
	 *
	 *    rx, ry     = screen 2D coordinates
	 *    ox, oy     = rx, ry shifted to origin
	 *    ix, iy, iz = isometric pixel coordinates
	 *    cx, cy, cz = isometric grid cell coordinates
	 *    px, py, pz = isometric pixel delta against grid
	 *
	 *
	 *    xs, ys, zs = x-size, y-size, z-size
	 *
	 */

public:
	IsoGrid(int sizex, int sizey, int sizez, int tilesizexy, int tilesizez) : sizex(sizex), sizey(sizey), sizez(sizez), TILEX(tilesizexy), TILEY(tilesizexy), TILEZ(tilesizez)
	{
		rx0 = 384; //TODO - hardcoded. should be calculated from width / height of component after component resize.
		ry0 = 216;
	}

	int getSize_ix() { return sizex * TILEX; }
	int getSize_iy() { return sizey * TILEY; }
	int getSize_iz() { return sizez * TILEZ; }

	virtual ~IsoGrid() {}

	void isoDrawWireFrame (int rx, int ry, int ixs, int iys, int izs, ALLEGRO_COLOR color)
	{
		drawWireFrame (rx + rx0, ry + ry0, ixs, iys, izs, color);
	}

	/**
	 * given a certain screen coordinate and a isometric z-level,
	 * calculate the isometric x, y
	 *
	 * Because a given point on the screen corresponds to many possible x,y,z points in isometric space,
	 * the caller has to choose the z coordinate (usually, in a loop trying possibilities until a good fit comes up)
	 */
	void screenToIso (float iz, int rx, int ry, float &ix, float &iy)
	{
		int ox = rx - rx0;
		int oy = ry - ry0;

		ix = oy + (ox / 2) + (iz / 2);
		iy = oy - (ox / 2) + (iz / 2);
	}

	int isoToScreenX (float ix, float iy, float iz)
	{
	 	int ox = (int)(ix - iy);
	 	return ox + rx0;
	}

	int isoToScreenY (float ix, float iy, float iz)
	{
		int oy = (int)(ix * 0.5 + iy * 0.5 - iz);
		return oy + ry0;
	}

	/**
	 * Useful for z-ordering sprites
	 */
	int isoToScreenZ (float ix, float iy, float iz)
	{
		return (int)(ix + iy + iz);
	}

	/**
	 * Check that a given grid coordinate is within bounds
	 */
	bool cellInBounds(int cx, int cy, int cz)
	{
		return
				cx >= 0 && cx < sizex &&
				cy >= 0 && cy < sizey &&
				cz >= 0 && cz < sizez;
	}

	int getXorig () const { return rx0; }

	/** distance from the cell at 0,0 to the edge of the virtual screen */
	int getYorig () const { return ry0; }

	void canvasFromIso_f (float x, float y, float z, float &rx, float &ry)
	{
		rx = getXorig() + (x - y);
		ry = getYorig() + (x * 0.5 + y * 0.5 - z);
	}

	void drawSurface(const GraphicsContext &gc, int mx, int my, int mz)
	{
		ALLEGRO_VERTEX coord[4]; // hold memory for coordinates
		ALLEGRO_VERTEX *surf1[3] = { &coord[0], &coord[1], &coord[3] };
		ALLEGRO_VERTEX *surf2[3] = { &coord[1], &coord[2], &coord[3] };

		ALLEGRO_COLOR baseColor = al_map_rgb (192, 255, 192);

		canvasFromIso_f (	TILEX * mx,
						TILEY * my,
						TILEZ * mz,
						coord[0].x, coord[0].y);

		canvasFromIso_f (	TILEX * (mx + 1),
						TILEY * my,
						TILEZ * (mz /* + c.dzright */),
						coord[1].x, coord[1].y);

		canvasFromIso_f (	TILEX * (mx + 1),
						TILEY * (my + 1),
						TILEZ * (mz /* + c.dzbot */),
						coord[2].x, coord[2].y);

		canvasFromIso_f (	TILEX * mx,
						TILEY * (my + 1),
						TILEZ * (mz /* + c.dzleft */),
						coord[3].x, coord[3].y);

		ALLEGRO_COLOR color = litColor (baseColor,
				surfaceLighting (1, 0, 0,
								0, 1, 0) );

		for (int i = 0; i < 4; ++i)
		{
			coord[i].x -= gc.xofst;
			coord[i].y -= gc.yofst;
			coord[i].color = color;
		}


		al_draw_prim(surf1, NULL, NULL, 0, 3, /* POLYTYPE_FLAT */ ALLEGRO_PRIM_POINT_LIST);

		color = litColor (baseColor,
					surfaceLighting (0, -1, 0, -1, 0, 0) );

		for (int i = 0; i < 4; ++i)
		{
			coord[i].color = color;
		}

		al_draw_prim(surf2, NULL, NULL, 0, 3, /* POLYTYPE_FLAT */ ALLEGRO_PRIM_POINT_LIST);
	}

	void drawLeftWall(const GraphicsContext &gc, int mx, int my, int mz)
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
		z[2] = TILEZ * (mz /* c.z + c.dzleft */);

		x[3] = TILEX * (mx + 1);
		y[3] = TILEY * (my + 1);
		z[3] = TILEZ * (mz /* c.z + c.dzbot */);

		ALLEGRO_COLOR color = litColor (al_map_rgb (192, 192, 192),
					surfaceLighting (0, 1, 0, 0, 0, 1 ));

		drawIsoPoly(gc, 4, x, y, z, color);
	}

	void drawRightWall(const GraphicsContext &gc, int mx, int my, int mz)
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
		z[2] = TILEZ * (mz /* c.z + c.dzbot */);

		x[3] = TILEX * (mx + 1);
		y[3] = TILEY * my;
		z[3] = TILEZ * (mz /* c.z + c.dzright */);

		ALLEGRO_COLOR color = litColor (al_map_rgb (192, 192, 192),
					surfaceLighting (0, 0, 1, -1, 0, 0) );

		drawIsoPoly(gc, 4, x, y, z, color);
	}

	void drawIsoPoly (const GraphicsContext &gc, int num, int x[], int y[], int z[], ALLEGRO_COLOR color)
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
			coord[i].x -= gc.xofst;
			coord[i].y -= gc.yofst;
			coord[i].color = color;
			coord[i].z = 0;
			coord[i].u = 0;
			coord[i].v = 0;
		}

		al_draw_prim (pcoord, NULL, NULL, 0, num, /* POLYTYPE_FLAT */ ALLEGRO_PRIM_POINT_LIST);
	}

	void drawSurfaceWire (const GraphicsContext &gc, int mx, int my, int mz, int mxs, int mys, ALLEGRO_COLOR color)
	{
		float rx1, ry1, rx2, ry2, rx3, ry3, rx4, ry4;

		canvasFromIso_f (TILEX * mx,
						TILEY * my,
						TILEZ * mz,
						rx1, ry1);

		canvasFromIso_f (TILEX * (mx + mxs),
						TILEY * (my),
						TILEZ * mz,
						rx3, ry3);

		canvasFromIso_f (TILEX * (mx),
						TILEY * (my + mys),
						TILEZ * mz,
						rx4, ry4);

		canvasFromIso_f (TILEX * (mx + mxs),
						TILEY * (my + mys),
						TILEZ * mz,
						rx2, ry2);

		al_draw_line (rx1, ry1, 		rx3, ry3,	color, 1.0);
		al_draw_line (rx3, ry3, 		rx2, ry2,	color, 1.0);
		al_draw_line (rx2, ry2,			rx4, ry4,	color, 1.0);
		al_draw_line (rx4, ry4,			rx1, ry1,	color, 1.0);
	}

};

#endif /* ISOCANVAS_H_ */
