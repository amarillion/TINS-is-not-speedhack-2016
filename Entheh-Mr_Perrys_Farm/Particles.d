module Particles;
import allegro5.allegro;
import std.math;
import std.random;
import Main;
import LevelModule;

struct ExplosionParticle {
	//Polar coordinates
	float r, dr;
	float a, da;
	//Rotation
	float angle;
}

enum numExplosionParticles = 128;
enum explosionDuration = 0.5f;

class Explosion {
	ExplosionParticle[numExplosionParticles] particles;
	float x, y;
	double endTime;

	this(float x, float y) {
		this.x=x; this.y=y;
		endTime = time + explosionDuration;
		foreach (ref particle; particles) {
			particle.r = sqrt(uniform(0f, 4.0f*4.0f));
			particle.dr = uniform(-1f, +1f) * Level.tw;
			particle.a = uniform(0f, 2*PI);
			particle.da = uniform(-3f, +3f) - particle.r*(4-particle.r);
			particle.angle = uniform(0f, 2*PI);
			particle.r *= Level.tw;
		}
	}
}

void updateParticles(ref Explosion[] explosions) {
	int writePos = 0;
	foreach (readPos, ref explosion; explosions) {
		if (time >= explosion.endTime) {
			delete explosion;
			continue;
		}
		foreach (ref particle; explosion.particles) {
			particle.r += particle.dr * deltaTime;
			particle.a += particle.da * deltaTime;
			particle.angle += particle.da * deltaTime;
		}
		explosions[writePos++] = explosion;
	}
	explosions.length = writePos;
}

void drawParticles(ref Explosion[] explosions) {
	al_hold_bitmap_drawing(true);
	float sw = al_get_bitmap_width(fireBall);
	float sh = al_get_bitmap_height(fireBall);
	foreach (ref explosion; explosions) {
		float scale = (explosion.endTime - time) / explosionDuration;
		foreach (ref particle; explosion.particles) {
			float x = explosion.x + particle.r * sin(particle.a);
			float y = explosion.y - particle.r * cos(particle.a);
			al_draw_scaled_rotated_bitmap(fireBall, sw/2, sh/2, x, y, scale, scale, particle.angle, 0);
		}
	}
	al_hold_bitmap_drawing(false);
}

//Note: I re-used this struct for weather particles (although they're in separate arrays).
//Ideally I'd rename this struct now, but time is so short!!!
struct BloodParticle {
	float x, y;
	float dx, dy;
	float scale;
	double landingTime;
}

enum numBloodParticles = 64;
enum maxBloodDistance = 40*4;
enum maxBloodTime = 0.5f;

void createBloodParticles(ref BloodParticle[] particles, float x, float y, float scaleRange) {
	foreach (i; 0..numBloodParticles) {
		BloodParticle particle;
		float dr = uniform(0f, maxBloodDistance / maxBloodTime);
		float a = uniform(0f, 2*PI);
		particle.dx = dr*sin(a);
		particle.dy = dr*cos(a);
		float headstart = uniform(0f, deltaTime);
		particle.x = x + particle.dx*headstart;
		particle.y = y + particle.dy*headstart;
		particle.scale = pow(2,uniform!"[]"(-scaleRange,0f));
		particle.landingTime = time + uniform(0f, maxBloodTime);
		particles ~= particle;
	}
}

enum maxWeatherTime = 1f;

void createWeatherParticle(ref BloodParticle[] particles, float x, float y, float scale) {
	BloodParticle particle;
	float dr = uniform(0f, 10 / maxWeatherTime);
	float a = uniform(0f, 2*PI);
	particle.dx = dr*sin(a);
	particle.dy = dr*cos(a);
	float headstart = uniform(0f, deltaTime);
	particle.x = x + particle.dx*headstart;
	particle.y = y + particle.dy*headstart;
	particle.scale = scale;
	particle.landingTime = time + uniform(maxWeatherTime/2, maxWeatherTime);
	particles ~= particle;
}

void updateParticles(ref BloodParticle[] particles, ALLEGRO_BITMAP* decalSprite) {
	int dw = al_get_bitmap_width(bloodSplat);
	int dh = al_get_bitmap_height(bloodSplat);
	int writePos = 0;
	foreach (readPos, ref particle; particles) {
		float dt = deltaTime + particle.landingTime - time;
		if (dt > deltaTime) dt = deltaTime;
		particle.x = (particle.x + particle.dx * dt) % Level.gameWidth;
		particle.y = (particle.y + particle.dy * dt) % Level.gameHeight;
		if (particle.x < 0) particle.x += Level.gameWidth;
		if (particle.y < 0) particle.y += Level.gameHeight;
		if (time >= particle.landingTime) {
			if (decalSprite) Level.decals ~= Decal(decalSprite, cast(int)particle.x - dw/2, cast(int)particle.y - dh/2, uniform(0,4), particle.scale, uniform(0f,2*PI));
			continue;
		}
		particles[writePos++] = particle;
	}
	particles.length = writePos;
}

void drawParticles(ref BloodParticle[] particles, ALLEGRO_BITMAP* sprite) {
	al_hold_bitmap_drawing(true);
	float sw = al_get_bitmap_width(sprite);
	float sh = al_get_bitmap_height(sprite);
	foreach (ref particle; particles) {
		al_draw_scaled_rotated_bitmap(sprite, sw/2, sh/2, particle.x, particle.y, particle.scale/2, particle.scale/2, 0, 0);
	}
	al_hold_bitmap_drawing(false);
}
