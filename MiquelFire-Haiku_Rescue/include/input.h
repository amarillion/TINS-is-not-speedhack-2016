#ifndef INPUT_H
#define INPUT_H

#include <allegro5/allegro.h>

class Input {
public:
	Input();

	// Setters
	void keyDown(ALLEGRO_EVENT *event);
	void keyUp(ALLEGRO_EVENT *event);
	void mouseMove(ALLEGRO_EVENT *event);
	void mouseButDown(ALLEGRO_EVENT *event);
	void mouseButUp(ALLEGRO_EVENT *event);
	void resetChanged();

	// Keyboard Getters
	bool upPressed();
	bool upDown();
	bool downPressed();
	bool downDown();
	bool leftPressed();
	bool leftDown();
	bool rightPressed();
	bool rightDown();
	bool firePressed();
	bool pausePressed();

	// Mouse Getters
	int mouseX() { return mX; };
	int mouseY() { return mY; };
	bool mouseButtonDown(int but);
	bool mouseButtonPressed(int but);
	
private:
	int up, down, left, right, fire, pause;
	int mX, mY, mB[5];
};

#endif // INPUT_H
