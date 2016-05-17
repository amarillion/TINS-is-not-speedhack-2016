#include "viewport.h"
#include "component.h"

using namespace std;

void ViewPort::update()
{
	int dx = destx - basex;
	int dy = desty - basey;

	// if close by, move directly
	if ((dx * dx + dy * dy) < 20.0)
	{
		basex = destx;
		basey = desty;
	}
	else
	{
		// move there in increasingly smaller steps
		basex += dx / 12;
		basey += dy / 12;
	}

	xofst = basex + quake.getdx(counter);
	yofst = basey + quake.getdy(counter);

	IContainer::update();
}

void ViewPort::moveTo(int _xofst, int _yofst)
{
	destx = _xofst;
	desty = _yofst;
}

void ViewPort::tremble(float _ampl)
{
	quake.start(_ampl, counter);
}

void ViewPort::draw(const GraphicsContext &gc)
{
	GraphicsContext gc2 = GraphicsContext();
	gc2.buffer = gc.buffer;
	gc2.xofst = getXofst();
	gc2.yofst = getYofst();

	IContainer::draw(gc2);
}
