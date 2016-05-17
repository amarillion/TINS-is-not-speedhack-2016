#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <math.h>
#include <vector>
#include <deque>

// The following were for path finding stuff
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <utility>
#include <queue>
#include <algorithm>

using std::unordered_map;
using std::unordered_set;
using std::array;
using std::vector;
using std::queue;
using std::priority_queue;
using std::pair;
using std::tuple;
using std::tie;

namespace std {
	template <>
	struct hash<tuple<int, int> > {
		inline size_t operator()(const tuple<int, int>& location) const {
			int x, y;
			tie(x, y) = location;
			return x * 1812433253 + y;
		}
	};
}

struct SquareGrid {
	typedef tuple<int, int> Location;
	static array<Location, 4> DIRS;

	int width, height;
	unordered_set<Location> walls;

	SquareGrid(int width_, int height_)
		: width(width_), height(height_) {}

	inline bool in_bounds(Location id) const {
		int x, y;
		tie(x, y) = id;
		return 0 <= x && x < width && 0 <= y && y < height;
	}

	inline bool passable(Location id) const {
		return !walls.count(id);
	}

	vector<Location> neighbors(Location id) const {
		int x, y, dx, dy;
		tie(x, y) = id;
		vector<Location> results;

		for (auto dir : DIRS) {
			tie(dx, dy) = dir;
			Location next(x + dx, y + dy);
			if (in_bounds(next) && passable(next)) {
				results.push_back(next);
			}
		}

		if ((x + y) % 2 == 0) {
			// aesthetic improvement on square grids
			std::reverse(results.begin(), results.end());
		}

		return results;
	}
};

array<SquareGrid::Location, 4> SquareGrid::DIRS{ Location{ 1, 0 }, Location{ 0, -1 }, Location{ -1, 0 }, Location{ 0, 1 } };

// TODO: Support multiple goals
template<typename Graph>
unordered_map<typename Graph::Location, typename Graph::Location>
breadth_first_search(Graph graph, typename Graph::Location start) {
	typedef typename Graph::Location Location;
	queue<Location> frontier;
	frontier.push(start);

	unordered_map<Location, Location> came_from;
	came_from[start] = start;

	while (!frontier.empty()) {
		auto current = frontier.front();
		frontier.pop();

		for (auto next : graph.neighbors(current)) {
			if (!came_from.count(next)) {
				frontier.push(next);
				came_from[next] = current;
			}
		}
	}
	return came_from;
}


#include "main.h"
#include "game.h"
#include "rng.h"
#include "resscale.h"
#include "particle.h"
#include "input.h"

struct Haiku {
	ALLEGRO_USTR *haiku_line;
	ALLEGRO_BITMAP *bmp; // The combined piece of paper
	char line1[255], line2[255], line3[255];
};

struct Piece {
	ALLEGRO_BITMAP *bmp;
	int segment;
	bool collected = false;
};

struct PieceGroup {
	Piece *pieces;
	Haiku *rootHaiku;
	bool read = false;
	int num_pieces;
};

struct Map {
	/*
		0 = empty
		1-8 = impassable object (rock, tree, etc)
			1
		1xy = a piece of a haiku, x being the group, and y being the piece
	*/
	int **map;
	int mw, mh;
	PieceGroup *groups = NULL;
	int num_groups;
	float pm = 0; // how much off the tile the player is
	/*
		0 is up
		1 is right
		2 is down
		3 is left
	*/
	int pd;
	int px = 0, py = 0, ptx, pty;
	bool moving = false;
};

void create_map(rng *r, Map *map, std::vector<Haiku> &haikus, int level = 0) {
	int i, j, x, y, m;
	bool placing = false;
	Piece *pieces;
	bool do_pieces = true;

	if (!map->groups && !do_pieces) {
		do_pieces = true;
	}
	if (map->groups && do_pieces) {
		for (i = 0; i < map->num_groups; i++) {
			for (j = map->groups[i].num_pieces - 1; j > -1; j--) {
				al_destroy_bitmap(map->groups[i].pieces[j].bmp);
			}
			delete[] map->groups[i].pieces;
		}
		delete[] map->groups;
	}
	for (i = 0; i < map->mw; i++) {
		for (j = 0; j < map->mh; j++) {
			if (!do_pieces) {
				if ((map->map[i][j] & 0xff00) != 0x0100) {
					map->map[i][j] = 0;
				}
			}
			else {
				map->map[i][j] = 0;
			}
		}
	}

	if (do_pieces) {
		map->num_groups = level + 1;
		map->groups = new PieceGroup[map->num_groups];
		for (i = 0; i < map->num_groups; i++) {
			map->groups[i].rootHaiku = &(haikus[i]);
			map->groups[i].num_pieces = 3;
			pieces = map->groups[i].pieces = new Piece[map->groups[i].num_pieces];
			for (j = 0; j < map->groups[i].num_pieces; j++) {
				pieces[j].segment = j;
				// TODO: Generate the bitmap for how it appears in the bag
				pieces[j].bmp = NULL;
			}
		}

		for (i = 0; i < map->num_groups; i++) {
			for (j = 0; j < map->groups[i].num_pieces; j++) {
				m = 0x0100 + (i << 4) + j;
				placing = true;
				while (placing) {
					x = r->rand() % map->mw;
					y = r->rand() % map->mh;
					if (!map->map[x][y] && x != (int)map->px && y != (int)map->py) {
						map->map[x][y] = m;
						placing = false;
					}
				}
			}
		}
	}

	// TODO: place objects on the board, not blocking a piece, or the player, and keep a path to the pieces
	for (j = 0; j < 1; j++) {
		for (i = 1; i <= 5; i++) {
			placing = true;
			while (placing) {
				x = r->rand() % map->mw;
				y = r->rand() % map->mh;
				if (!map->map[x][y] && x != (int)map->px && y != (int)map->py) {
					map->map[x][y] = i;
					placing = false;
				}
			}
		}
	}
}

void play_game(void) {
	ALLEGRO_FILE *file;
	ALLEGRO_USTR *ustr;
	ALLEGRO_FONT *font;
	ALLEGRO_EVENT event;
	ALLEGRO_COLOR color;
	ALLEGRO_TRANSFORM def, scaled, mscaled;
	ALLEGRO_BITMAP *tiles, *groundTile[4], *rockTile[4], *treeTile[4], *bushTile[4], *moundTile[4], *stoneTile[4],
		*bagTile, *piecesTile[3], *snowFlakeTile[8], *playerTile[4], *craftTile, *quitTile;

	std::vector<Haiku> haikus;
	Haiku haiku;
	Input input;
	ResScale scale(640, 480);
	rng r;
	std::deque<Particle> particles;
	Particle particle;
	Map map;

	int tx, ty, tw, th,
		moff, m,
		/*
			bit 0: pieces on map
			bit 1: bag open (Can be combined with bit 0)
			bit 2: regenerating map (bit 0 set to 1, bit 1 set to 0)
			bit 3: displaying haiku (Only when bit 1 is set)
			bit 8: Game over screen (Other bits off)
		*/
		state = 0,
		level = 0, // Up to 3 before game over
		counter,
		flakeCounter = 0,
		snowCounter = 0, // used for figuring out if a row of snow should appear on the line or not
		x, y,
		haikuCollected = 0,
		i, j;
	bool running = true, redraw = false, quitHightlight = false, allRead = false;
	float mx, my, sx, sy, sw, sh;

	// define the map
	tw = (int)(scale.getOffsetx() * 2) + 634;
	map.mw = tw / 64;
	map.mh = 6;
	map.ptx = map.pty = map.px = map.py = 0;
	map.pd = 2;
	map.num_groups = 1;
	moff = (tw - (map.mw * 64)) / 2;
	map.map = new int*[map.mw];
	for (i = 0; i < map.mw; i++) {
		map.map[i] = new int[map.mh];
		for (j = 0; j < map.mh; j++) {
			map.map[i][j] = 0;
		}
	}

	// Load up haiku.txt file
	font = al_load_ttf_font("jap.otf", -30, ALLEGRO_TTF_MONOCHROME);
	file = al_fopen("haiku.txt", "r");
	while (!al_feof(file)) {
		// Read the Japanese text
		ustr = al_fget_ustr(file);
		if (!ustr) break;
		if (!al_ustr_trim_ws(ustr)) {
			al_ustr_free(ustr);
			break;
		}
		haiku.haiku_line = ustr;
		al_get_ustr_dimensions(font, ustr, &tx, &ty, &tw, &th);
		haiku.bmp = al_create_bitmap(tw + 12, th + 12);
		al_set_target_bitmap(haiku.bmp);
		al_draw_filled_rounded_rectangle(1, 1, tw + 11, th + 11, 6, 6, al_map_rgb(255, 255, 255));
		al_draw_rounded_rectangle(1, 1, tw + 11, th + 11, 6, 6, al_map_rgb(15, 15, 15), 1);
		al_draw_ustr(font, al_map_rgb(0, 0, 0), 6, 6, 0, haiku.haiku_line);

		// Read Line 1
		ustr = al_fget_ustr(file);
		if (!ustr) {
			al_ustr_free(haiku.haiku_line);
			break;
		}
		if (!al_ustr_trim_ws(ustr)) {
			al_ustr_free(haiku.haiku_line);
			al_ustr_free(ustr);
			break;
		}
		al_ustr_to_buffer(ustr, haiku.line1, 255);
		al_ustr_free(ustr);

		// Read Line 2
		ustr = al_fget_ustr(file);
		if (!ustr) {
			al_ustr_free(haiku.haiku_line);
			break;
		}
		if (!al_ustr_trim_ws(ustr)) {
			al_ustr_free(haiku.haiku_line);
			al_ustr_free(ustr);
			break;
		}
		al_ustr_to_buffer(ustr, haiku.line2, 255);
		al_ustr_free(ustr);

		// Read Line 3
		ustr = al_fget_ustr(file);
		if (!ustr) {
			al_ustr_free(haiku.haiku_line);
			break;
		}
		if (!al_ustr_trim_ws(ustr)) {
			al_ustr_free(haiku.haiku_line);
			al_ustr_free(ustr);
			break;
		}
		al_ustr_to_buffer(ustr, haiku.line3, 255);
		al_ustr_free(ustr);

		haikus.push_back(haiku);
	}
	al_set_target_bitmap(al_get_backbuffer(display));

	// Load tiles
	tiles = al_load_bitmap("tiles.png");
	groundTile[0] = al_create_sub_bitmap(tiles, 0, 0, 16, 16);
	groundTile[1] = al_create_sub_bitmap(tiles, 0, 16, 16, 16);
	groundTile[2] = al_create_sub_bitmap(tiles, 0, 32, 16, 16);
	groundTile[3] = al_create_sub_bitmap(tiles, 0, 48, 16, 16);

	rockTile[0] = al_create_sub_bitmap(tiles, 32, 0, 16, 16);
	rockTile[1] = al_create_sub_bitmap(tiles, 32, 16, 16, 16);
	rockTile[2] = al_create_sub_bitmap(tiles, 32, 32, 16, 16);
	rockTile[3] = al_create_sub_bitmap(tiles, 32, 48, 16, 16);

	treeTile[0] = al_create_sub_bitmap(tiles, 80, 0, 16, 16);
	treeTile[1] = al_create_sub_bitmap(tiles, 80, 16, 16, 16);
	treeTile[2] = al_create_sub_bitmap(tiles, 80, 32, 16, 16);
	treeTile[3] = al_create_sub_bitmap(tiles, 80, 48, 16, 16);

	bushTile[0] = al_create_sub_bitmap(tiles, 48, 0, 16, 16);
	bushTile[1] = al_create_sub_bitmap(tiles, 48, 16, 16, 16);
	bushTile[2] = al_create_sub_bitmap(tiles, 48, 32, 16, 16);
	bushTile[3] = al_create_sub_bitmap(tiles, 48, 48, 16, 16);

	moundTile[0] = al_create_sub_bitmap(tiles, 16, 0, 16, 16);
	moundTile[1] = al_create_sub_bitmap(tiles, 16, 16, 16, 16);
	moundTile[2] = al_create_sub_bitmap(tiles, 16, 32, 16, 16);
	moundTile[3] = al_create_sub_bitmap(tiles, 16, 48, 16, 16);

	stoneTile[0] = al_create_sub_bitmap(tiles, 64, 0, 16, 16);
	stoneTile[1] = al_create_sub_bitmap(tiles, 64, 16, 16, 16);
	stoneTile[2] = al_create_sub_bitmap(tiles, 64, 32, 16, 16);
	stoneTile[3] = al_create_sub_bitmap(tiles, 64, 48, 16, 16);

	bagTile = al_create_sub_bitmap(tiles, 0, 112, 16, 16);

	piecesTile[0] = al_create_sub_bitmap(tiles, 16, 112, 16, 16);
	piecesTile[1] = al_create_sub_bitmap(tiles, 32, 112, 16, 16);
	piecesTile[2] = al_create_sub_bitmap(tiles, 48, 112, 16, 16);

	quitTile = al_create_sub_bitmap(tiles, 112, 112, 16, 16);
	craftTile = al_create_sub_bitmap(tiles, 96, 112, 16, 16);

	for (i = 0; i < 8; i++) {
		snowFlakeTile[i] = al_create_sub_bitmap(tiles, (i % 4) * 16, 64, 16, 16);
	}

	for (i = 0; i < 4; i++) {
		playerTile[i] = al_create_sub_bitmap(tiles, (i % 1) * 16, 80, 16, 16);
	}

	// Transform stuffs
	al_identity_transform(&def);
	scale.getTransform(&scaled);
	al_copy_transform(&mscaled, &scaled);
	al_invert_transform(&mscaled);

	// Set up initial snow flakes
	flakeCounter = 0;
	for (j = (map.mh + 1) * 64 + 14; j; j--) {
		if ((snowCounter++ % 20) == 0) {
			// Create the row of snow
			x = 0;
			while (x < map.mw * 64 + moff + moff) {
				sy = (float)(r.rand() % 200 - 100) / 100.0;
				sw = (float)(r.rand() % 50) / 100.0 + 0.5;
				particle.init(Vector2(x, j), Vector2(sy, 1.0), snowFlakeTile[flakeCounter++], sw);
				flakeCounter %= 8;
				particles.push_back(particle);
				x += (r.rand() % 20) + 10;
			}
		}
	}

	// Create loop
	al_start_timer(timer);
	create_map(&r, &map, haikus);
	while (running) {
		al_wait_for_event(queue, NULL);
		while (!al_event_queue_is_empty(queue)) {
			al_get_next_event(queue, &event);

			switch (event.type) {
			case ALLEGRO_EVENT_KEY_DOWN: {
				input.keyDown(&event);
				break;
			}
			case ALLEGRO_EVENT_KEY_UP: {
				input.keyUp(&event);
				break;
			}
			case ALLEGRO_EVENT_MOUSE_AXES: {
				input.mouseMove(&event);
				break;
			}
			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN: {
				input.mouseButDown(&event);
				break;
			}
			case ALLEGRO_EVENT_MOUSE_BUTTON_UP: {
				input.mouseButDown(&event);
				break;
			}
			case ALLEGRO_EVENT_DISPLAY_CLOSE: {
				running = false;
				break;
			}
			case ALLEGRO_EVENT_TIMER: {
				mx = input.mouseX();
				my = input.mouseY();
				al_transform_coordinates(&mscaled, &mx, &my);

				if (input.pausePressed()) {
					// Pause menu?
					running = false;
				}
				// 3, 413, 64, 64
				if (mx >= 3 && mx <= 67 && my >= 413 && my <= 477) {
					quitHightlight = true;
					if (input.mouseButtonPressed(0)) {
						running = false;
					}
				}
				else {
					quitHightlight = false;
				}

				// Snow update
				for (std::deque<Particle>::iterator it = particles.begin(); it != particles.end();) {
					it->logic();
					if (!it->isActive()) {
						it = particles.erase(it);
					}
					else {
						it++;
					}
				}
				if ((snowCounter++ % 20) == 0) {
					// Create the row of snow
					x = 0;
					while (x < map.mw * 64 + moff + moff) {
						sy = (float)(r.rand() % 200 - 100) / 100.0;
						sw = (float)(r.rand() % 50) / 100.0 + 0.5;
						particle.init(Vector2(x, 0), Vector2(sy, 1.0), snowFlakeTile[flakeCounter++], sw);
						flakeCounter %= 8;
						particles.push_back(particle);
						x += (r.rand() % 20) + 10;
					}
				}

				// On the field
				if ((state & 0x2) == 0) {
					// Set target
					x = mx - moff;
					y = my - 14;
					tx = x /= 64;
					ty = y /= 64;
			
					if (input.mouseButtonPressed(0)) {
						if (x < map.mw && y < map.mh) {
							if (map.map[x][y] == 0 || map.map[x][y] > 7) {
								map.ptx = x;
								map.pty = y;
							}
						}
					}

					// Move player
					if (map.moving) {
						// player is already moving to a tile
						if (map.pm > 1.0) {
							// player is at the new tile, reset stats
							map.pm = 0;
							map.moving = false;
							switch (map.pd) {
							case 0:
								map.py--;
								break;
							case 1:
								map.px++;
								break;
							case 2:
								map.py++;
								break;
							case 3:
								map.px--;
								break;
							}
						}
						else {
							map.pm += 0.10;
						}
					}
					if (!map.moving) {
						// player is on a tile, check if they reached their target
						if (map.px == map.ptx && map.py == map.pty) {
							// Player reached their target, do whatever if there's something to do
							if ((map.map[map.px][map.py] & 0x0f00) == 0x0100) {
								// Pick up the piece
								m = map.map[map.px][map.py] & 0xff;
								x = (m & 0xf0) >> 4;
								y = m & 0x0f;
								map.groups[x].pieces[y].collected = true;
								map.map[map.px][map.py] = 0;
								if (map.groups[x].pieces[0].collected && map.groups[x].pieces[1].collected && map.groups[x].pieces[2].collected) {
									map.groups[x].read = true;
									haikuCollected = x;
									state |= 0x2;
								}
							}
						}
						else {
							// player not at target yet
							if (map.px != map.ptx) {
								// they need to move left or right still
								if (map.px > map.ptx) {
									// Need to move left
									if (map.map[map.px - 1][map.py] == 0 || map.map[map.px - 1][map.py] > 7) {
										map.pd = 3;
										map.moving = true;
									}
								}
								else {
									// Need to move right
									if (map.map[map.px + 1][map.py] == 0 || map.map[map.px + 1][map.py] > 7) {
										map.pd = 1;
										map.moving = true;
									}
								}
							}
							if (!map.moving && map.py != map.pty) {
								// they need to move up or down still
								if (map.py > map.pty) {
									// Need to move up
									if (map.map[map.px][map.py - 1] == 0 || map.map[map.px][map.py - 1] > 7) {
										map.pd = 0;
										map.moving = true;
									}
								}
								else {
									// Need to move down
									if (map.map[map.px][map.py + 1] == 0 || map.map[map.px][map.py + 1] > 7) {
										map.pd = 2;
										map.moving = true;
									}
								}
							}
						}
					}
				}
				else {
					if (input.mouseButtonPressed(0)) {
						// Check if all the haikus were read. If so, regen the map first
						allRead = true;
						for (i = 0; i < map.num_groups; i++) {
							if (!map.groups[i].read) {
								allRead = false;
							}
						}
						if (allRead) {
							level++;
							if (level == haikus.size() || level == 4) {
								// TODO: Game Over screen!
								running = false;
								break;
							}
							else {
								create_map(&r, &map, haikus, level);
							}
						}
						state &= ~0x2;
					}
				}

				input.resetChanged();
			}
			case ALLEGRO_EVENT_DISPLAY_EXPOSE:
				redraw = true;
				break;
			}
		}

		if (redraw) {
			al_clear_to_color(al_map_rgb(0, 0, 0));
			al_use_transform(&scaled);

			if ((state & 0x2) == 0) {
				sx = moff;
				sy = 14;
				sw = map.mw * 64;
				sh = map.mh * 64;
				al_transform_coordinates(&scaled, &sx, &sy);
				al_transform_coordinates(&scaled, &sw, &sh);
				al_set_clipping_rectangle(sx, sy, sw, sh);
				for (i = 0; i < map.mh; i++) {
					for (j = 0; j < map.mw; j++) {
						al_draw_scaled_bitmap(groundTile[level], 0, 0, 16, 16, (j * 64) + moff, (i * 64) + 14, 64, 64, 0);
						// Draw whatever is on top of the map at this point
						switch (map.map[j][i]) {
						case 0:
							break;
						case 1: {
							al_draw_scaled_bitmap(moundTile[level], 0, 0, 16, 16, (j * 64) + moff, (i * 64) + 14, 64, 64, 0);
							break;
						}
						case 2: {
							al_draw_scaled_bitmap(stoneTile[level], 0, 0, 16, 16, (j * 64) + moff, (i * 64) + 14, 64, 64, 0);
							break;
						}
						case 3: {
							al_draw_scaled_bitmap(bushTile[level], 0, 0, 16, 16, (j * 64) + moff, (i * 64) + 14, 64, 64, 0);
							break;
						}
						case 4: {
							al_draw_scaled_bitmap(rockTile[level], 0, 0, 16, 16, (j * 64) + moff, (i * 64) + 14, 64, 64, 0);
							break;
						}
						case 5: {
							al_draw_scaled_bitmap(treeTile[level], 0, 0, 16, 16, (j * 64) + moff, (i * 64) + 14, 64, 64, 0);
							break;
						}
						case 6: {
							al_draw_scaled_bitmap(moundTile[level], 0, 0, 16, 16, (j * 64) + moff, (i * 64) + 14, 64, 64, 0);
							break;
						}
						case 7: {
							al_draw_scaled_bitmap(moundTile[level], 0, 0, 16, 16, (j * 64) + moff, (i * 64) + 14, 64, 64, 0);
							break;
						}
						default:{
							if ((map.map[j][i] & 0xff00) == 0x0100) {
								m = map.map[j][i] & 0xff;
								x = (m & 0xf0) >> 4;
								y = m & 0x0f;
								if (map.groups[x].pieces[y].segment == 0) {
									al_draw_scaled_bitmap(piecesTile[0], 0, 0, 16, 16, (j * 64) + moff, (i * 64) + 14, 64, 64, 0);
								}
								else if (map.groups[x].pieces[y].segment == map.groups[x].num_pieces - 1) {
									al_draw_scaled_bitmap(piecesTile[2], 0, 0, 16, 16, (j * 64) + moff, (i * 64) + 14, 64, 64, 0);
								}
								else {
									al_draw_scaled_bitmap(piecesTile[1], 0, 0, 16, 16, (j * 64) + moff, (i * 64) + 14, 64, 64, 0);
								}
							}
						}
						}
					}
				}

				// Player
				switch (map.pd) {
				case 0:
					sx = map.px;
					sy = map.py - map.pm;
					break;
				case 1:
					sx = map.px + map.pm;
					sy = map.py;
					break;
				case 2:
					sx = map.px;
					sy = map.py + map.pm;
					break;
				case 3:
					sx = map.px - map.pm;
					sy = map.py;
					break;
				}
				al_draw_scaled_bitmap(playerTile[map.pd], 0, 0, 16, 16, (sx * 64) + moff, (sy * 64) + 14, 64, 64, 0);

				// Snow!
				for (std::deque<Particle>::iterator it = particles.begin(); it != particles.end(); ++it) {
					it->draw();
				}

				// Highlight square
				if (tx < map.mw && ty < map.mh) {
					if (map.map[tx][ty] == 0 || map.map[tx][ty] > 7) {
						color = al_map_rgb(0, 0, 255);
					}
					else {
						color = al_map_rgb(255, 0, 0);
					}

					al_draw_rectangle(tx * 64 + moff, ty * 64 + 14, (tx + 1) * 64 + moff - 1, (ty + 1) * 64 + 14, color, 1.0);
				}

				al_reset_clipping_rectangle();

				// Pieces picked up
				x = 0;
				y = 0;
				for (j = 0; j < map.num_groups; j++) {
					if (!map.groups[j].read) {
						for (i = 0; i < map.groups[j].num_pieces; i++) {
							if (map.groups[j].pieces[i].collected) {
								if (i == 0) {
									al_draw_scaled_bitmap(piecesTile[0], 0, 0, 16, 16, 81 + x, 395 + y, 64, 64, 0);
								}
								else if (map.groups[j].pieces[i].segment == map.groups[j].num_pieces - 1) {
									al_draw_scaled_bitmap(piecesTile[2], 0, 0, 16, 16, 197 + x, 395 + y, 64, 64, 0);
								}
								else {
									al_draw_scaled_bitmap(piecesTile[1], 0, 0, 16, 16, 139 + x, 395 + y, 64, 64, 0);
								}
							}
						}
						x += 5;
						y += 10;
					}
				}
			}
			else {
				x = ((640 + scale.getOffsetx() * 2) - al_get_bitmap_width(haikus[haikuCollected].bmp)) / 2;
				y = 124 - al_get_bitmap_height(haikus[haikuCollected].bmp) ;
				al_draw_bitmap(haikus[haikuCollected].bmp, x, y, 0);
				al_get_text_dimensions(font, haikus[haikuCollected].line1, &tx, &ty, &tw, &th);
				al_draw_text(font, al_map_rgb(255, 255, 255), ((640 + scale.getOffsetx() * 2) - tw) / 2, 157, 0, haikus[haikuCollected].line1);
				al_get_text_dimensions(font, haikus[haikuCollected].line2, &tx, &ty, &tw, &th);
				al_draw_text(font, al_map_rgb(255, 255, 255), ((640 + scale.getOffsetx() * 2) - tw) / 2, 190, 0, haikus[haikuCollected].line2);
				al_get_text_dimensions(font, haikus[haikuCollected].line3, &tx, &ty, &tw, &th);
				al_draw_text(font, al_map_rgb(255, 255, 255), ((640 + scale.getOffsetx() * 2) - tw) / 2, 223, 0, haikus[haikuCollected].line3);
			}

			// quitbutton
			if (quitHightlight) al_draw_filled_rectangle(3, 413, 67, 477, al_map_rgb(255, 255, 255));
			al_draw_scaled_bitmap(quitTile, 0, 0, 16, 16, 3, 413, 64, 64, 0);

			al_use_transform(&def);
			al_flip_display();
			redraw = false;
		}
	}

	// Clean up
	for (i = 0; i < 4; i++) {
		al_destroy_bitmap(playerTile[i]);
		al_destroy_bitmap(groundTile[i]);
		al_destroy_bitmap(rockTile[i]);
		al_destroy_bitmap(treeTile[i]);
		al_destroy_bitmap(bushTile[i]);
		al_destroy_bitmap(moundTile[i]);
		al_destroy_bitmap(stoneTile[i]);
	}

	for (i = 0; i < 8; i++) {
		al_destroy_bitmap(snowFlakeTile[i]);
	}

	for (i = 0; i < 3; i++) {
		al_destroy_bitmap(piecesTile[i]);
	}

	al_destroy_bitmap(bagTile);
	al_destroy_bitmap(quitTile);
	al_destroy_bitmap(craftTile);

	al_destroy_bitmap(tiles);

	for (x = 0; x < map.mh; x++) {
		delete[] map.map[x];
	}
	delete[] map.map;

	for (x = 0; x < map.num_groups; x++) {
		for (y = 0; y < map.groups[x].num_pieces; y++) {
			al_destroy_bitmap(map.groups[x].pieces[y].bmp);
		}
		delete[] map.groups[x].pieces;
	}
	delete[] map.groups;
	
	while (haikus.size()) {
		haiku = haikus.back();
		al_destroy_bitmap(haiku.bmp);
		al_ustr_free(haiku.haiku_line);
		haikus.pop_back();
	}

	al_destroy_font(font);

	al_stop_timer(timer);
	al_flush_event_queue(queue);

	return;
}
