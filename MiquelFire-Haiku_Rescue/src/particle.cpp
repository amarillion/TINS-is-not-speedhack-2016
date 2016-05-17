#include "particle.h"

Particle::Particle() {
	active = false;
}

void Particle::init(Vector2 pos, Vector2 speed, ALLEGRO_BITMAP *img, float scale) {
	active = true;
	this->pos = pos;
	this->speed = speed;
	this->flake = img;
	this->scale = scale;
}

void Particle::logic() {
	if (!active) return;
	pos += speed;
	if (pos.x < 0) {
		active = false;
	}
	if (pos.y > 480) {
		active = false;
	}
	angle += 0.10;
}

void Particle::draw() {
	if (active) {
		al_draw_scaled_rotated_bitmap(flake, 8, 8, pos.x, pos.y, scale, scale, angle, 0);
	}
}
