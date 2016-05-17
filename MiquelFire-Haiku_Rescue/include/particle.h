#ifndef PARTICLE_H
#define PARTICLE_H

#include <allegro5/allegro.h>
#include "vector2.h"

class Particle {
public:
	Particle();
	void init(Vector2 pos, Vector2 speed, ALLEGRO_BITMAP *img, float scale);

	void logic();
	void changeScale(float scale);
	bool isActive() { return active; };
	void kill() { active = false; };
	void draw();

private:
	bool active;
	Vector2 pos;
	Vector2 speed;
	ALLEGRO_BITMAP *flake;
	float scale = 1.0;
	float angle = 0;
};

#endif // PARTICLE_H
