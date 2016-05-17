module Scopes;
import allegro5.allegro;

struct Clip
{
	int sx, sy, sw, sh;

	this(int x1, int y1, int x2, int y2) {
		al_get_clipping_rectangle(&sx, &sy, &sw, &sh);
		if (x1 < sx) x1 = sx;
		if (y1 < sy) y1 = sy;
		if (x2 > sx + sw) x2 = sx + sw;
		if (y2 > sy + sh) y2 = sy + sh;
		al_set_clipping_rectangle(x1, y1, x2-x1, y2-y1);
	}

	~this() {
		al_set_clipping_rectangle(sx, sy, sw, sh);
	}
}
