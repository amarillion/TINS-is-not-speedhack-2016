#ifndef VIEWPORT_H
#define VIEwPORT_H

#include "motionimpl.h"
#include "IContainer.h"

/**
 * A viewport is a container that defines a different coordinate system for its children.
 * The coordinate system is passed in the draw method through GraphicsContext.
 *
 * The viewport location can be animated for different effects.
 */
class ViewPort : public IContainer
{
private:
	int xofst;
	int yofst;

	// base offset without tremble
	int basex;
	int basey;

	// destination for movement
	int destx;
	int desty;

	// TODO: try not to use this in public header
	Quake quake;
public:
	ViewPort () : xofst (0), yofst (0), basex(0), basey(0), destx (0),
		desty (0), quake() {}

	// set directly
	void setOfst (int _xofst, int _yofst) { xofst = _xofst; yofst = _yofst; basex = xofst; basey = yofst; destx = xofst; desty = yofst; }
	void moveTo (int _xofst, int _yofst);
	void tremble (float ampl);

	virtual ~ViewPort() {}

	virtual void update() override;
	virtual void draw (const GraphicsContext &gc) override;

	virtual int getXofst() { return xofst; }
	virtual int getYofst() { return yofst; }
};

#endif
