#include "particle.h"
#include <iostream>
#include <allegro5/allegro_primitives.h>
#include "util.h"

using namespace std;

int random (int i)
{
	return rand() % i;
}

class ParticleRemover
{
public:
	bool operator()(Particle &p)
	{
		if (!p.alive)
		{
			return true;
		}
		return false;
	}
};

void Particles::update()
{
	if (particles.size() < snowForce)
	{
		// new ones at top of screen
		Particle particle;
		particle.alive = true;
		particle.x = random(grid->getSize_ix());
		particle.y = random(grid->getSize_iy());
		particle.z = grid->getSize_iz();
		particles.push_back(particle);
	}

	// update existing
	for (list<Particle>::iterator p = particles.begin(); p != particles.end(); ++p)
	{
		if (!p->alive) continue;

		// flurry. random movement downwards
		double dx = (double)(random(5) - 2) / 2.0;
		double dy = (double)(random(5) - 2) / 2.0;
		double dz = - (double)(random(5)) / 2.0;
		p->x += dx;
		p->y += dy;
		p->z += dz;

		if (p->z < 0) { p->alive = false; }
	}

	// remove all that are not alive!
	particles.remove_if (ParticleRemover());
}


Particles::Particles(IsoGrid *grid) : grid(grid), particles(), snowForce(0)
{
}

void Particles::draw(const GraphicsContext &gc)
{
	for (list<Particle>::iterator p = particles.begin(); p != particles.end(); ++p)
	{
		if (!p->alive) continue;

		float rx, ry;
		grid->canvasFromIso_f(p->x, p->y, p->z, rx, ry);
		al_draw_filled_circle(rx, ry, 2.0, WHITE);
	}
}


Particles::~Particles() {}
