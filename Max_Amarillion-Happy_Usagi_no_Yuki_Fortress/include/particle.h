#ifndef PARTICLE_H
#define PARTICLE_H

#include <list>
#include "isogrid.h"

struct Particle
{
	double x;
	double y;
	double z;

	double dx;
	double dy;
	double dz;

	bool alive;
};

class Particles : public IComponent
{
private:
	IsoGrid *grid;
	std::list<Particle> particles;
	int snowForce;
public:
	void setSnowForce(int val) { snowForce = val; }
	Particles(IsoGrid *grid);
	virtual void update();
	virtual void draw(const GraphicsContext &gc);
	virtual ~Particles();
};

#endif
