#include <assert.h>
#include "engine.h"
#include "color.h"
#include <math.h>
#include <list>
#include <algorithm>
#include "game.h"
#include "engine.h"
#include "anim.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "util.h"
#include <map>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <isogrid.h>
#include "text.h"
#include "control.h"
#include "resources.h"
#include "mainloop.h"
#include <math.h>
#include "particle.h"

using namespace std;

const int DIMXY = 12;
const int DIMZ = 8;
const int CELLNUM = DIMXY * DIMXY * DIMZ;

const int TILEXY = 32;
const int TILEH = 32;

const int MAX_SNOW_TILES = 5;

enum PathType { N, E, S, W, NU, EU, SU, WU, ND, ED, SD, WD, MAX };

enum Action { DELETE_BLOCK = 900, BUY_BUNNY, BUY_REFILL, TOGGLE_DEBUGMODE, CANCEL, PAUSE_MENU, QUIT, RESUME };

// globals
bool debugMode = false;
ALLEGRO_FONT *smallFont = NULL;

struct PathInfo
{
	int dx;
	int dy;
	int dz;
	int dir; // animation direction
	double amplitude;
	double period;
};

const int NUM_PATH_TYPES = 12;

PathInfo pathInfo[NUM_PATH_TYPES] = {
		{  0, -1,  0, 0,  5.0, 2.0 }, // N,
		{  1,  0,  0, 1,  5.0, 2.0 }, // E,
		{  0,  1,  0, 2,  5.0, 2.0 }, // S,
		{ -1,  0,  0, 3,  5.0, 2.0 }, // W

		{  0, -1,  1, 0, 20.0, 1.0 }, // NU,
		{  1,  0,  1, 1, 20.0, 1.0 }, // EU,
		{  0,  1,  1, 2, 20.0, 1.0 }, // SU,
		{ -1,  0,  1, 3, 20.0, 1.0 }, // WU

		{  0, -1, -1, 0, 20.0, 1.0 }, // ND,
		{  1,  0, -1, 1, 20.0, 1.0 }, // ED,
		{  0,  1, -1, 2, 20.0, 1.0 }, // SD,
		{ -1,  0, -1, 3, 20.0, 1.0 }, // WD
};

struct BlockInfo
{
	string name;
	int cost;
	Anim *anim;
	const char *animName;
	int csizeX;
	int csizeY;
	int csizeZ;
	bool isSolid;
	bool refillable;
};

const int NUM_BLOCK_TYPES = 5;
enum BlockType { BLOCK = 1200, BIG_BLOCK, FOOD_BOWL, CARROTS, HAY };
BlockInfo blockInfo[NUM_BLOCK_TYPES] = {

 	{ string("SMALL BLOCK"), 10, NULL, "smallblock", 1, 1, 1, true, false  },
	{ string("BIG BLOCK"),   40, NULL, "bigbox",     2, 2, 2, true, false  },
	{ string("FOOD BOWL"),  350, NULL, "foodbowl",   1, 1, 1, false, true },
	{ string("VEG PLATE"),  350, NULL, "carrots",    1, 1, 1, false, true },
	{ string("HAY RACK"),   350, NULL, "hay",        1, 1, 1, false, true },
};
const int START_MONEY = 300;
const int BUN_COST_START = 500;
const int BUN_COST_INCREASE = 250;
const int REFILL_COST = 25;
const int BINKY_PROFIT = 40;
const int HOP_PROFIT = 10;
const int FREE_MONEY_PERIOD = 100;

BlockInfo *getBlockInfo(BlockType bt)
{
	unsigned int blockInfoIdx = bt - BLOCK;
	assert (blockInfoIdx < NUM_BLOCK_TYPES);
	return blockInfo + blockInfoIdx;
}

class IntCounter : public IComponent
{
	function<int()> func;
	ALLEGRO_COLOR color;
public:
	IntCounter (ALLEGRO_COLOR color, function<int()> func) : func(func), color(color) {}
	virtual ~IntCounter() {}

	virtual void draw(const GraphicsContext &gc)
	{
		int value = func();
		al_draw_textf(sfont, color, x, y, ALLEGRO_ALIGN_LEFT, "%i", value);
	}

	static ComponentBuilder build(ALLEGRO_COLOR color, function<int()> func)
	{
		return ComponentBuilder(make_shared<IntCounter> (color, func));
	}
};

class Sprite;

class Trixel
{
public:
	Sprite *occupied;
	bool solid; // support if occupied by a Solid block

	int foodLeft; // 100 = max food
	int snow; // MAX_SNOW_TILES = max snow
	double smell; // 100 = max smell;

	bool hasPath(PathType path) { return true; }

	Trixel() : occupied(NULL), solid(false), foodLeft(0), snow(0), smell(0) { }
};

class IsoCanvas;

class Sprite
{
public:
	IsoCanvas *grid;

	int cx; // cell-x
	int cy;
	int cz;

	double px; // pixel-x (delta)
	double py;
	double pz;

	bool alive;
	bool visible;

public:
	void setPosition (int _cx, int _cy, int _cz) { cx = _cx; cy = _cy; cz = _cz; }

	// convert iso coordinates into real coordinates
	int getRx();
	int getRy();
	int getRz(); // for z-ordering

	virtual void draw(const GraphicsContext &gc) {};
	virtual void update() {};
	Sprite(IsoCanvas *parent) : grid(parent), cx(0), cy(0), cz(0), px(0), py(0), pz(0), alive(true), visible(true) {}
	virtual ~Sprite() {}
};

class Score : public Sprite
{
private:
	int score;
	int lifeTimer;
public:
	Score(IsoCanvas *parent, int score) : Sprite(parent), score(score), lifeTimer(0) { }
	virtual void draw(const GraphicsContext &gc) override
	{
		al_draw_textf (smallFont, BLACK, getRx(), getRy(), ALLEGRO_ALIGN_CENTER, "%i", score);
	}
	virtual void update() override
	{
		lifeTimer++;
		if (lifeTimer > 100)
		{
			alive = false;
		}
		pz += 0.5;
	}
};

class Block : public Sprite
{
	BlockType blockType;
	BlockInfo *info;
public:
	Block(IsoCanvas *grid) : Sprite(grid), blockType(BLOCK), info(blockInfo) {}

	void applyCellRange(const function<void(Trixel *)> &func);

	BlockType getBlockType() { return blockType; }
	BlockInfo *getInfo() { return getBlockInfo(blockType); }
	void setBlockType(BlockType bt)
	{
		info = getBlockInfo (bt);
		blockType = bt;
	}
	virtual void draw(const GraphicsContext &gc) override;

};

class WeatherControl
{
	int counter;
	int snowTileCounter;
	int snowForce;
	IsoCanvas *canvas;
	Particles *particles;
public:
	WeatherControl (IsoCanvas *canvas, Particles *particles) : counter (1000), snowTileCounter(0), snowForce(0), canvas(canvas), particles(particles) {}
	void update();
};

class GameImpl;

class IsoCanvas : public IsoGrid
{
public:
	GameImpl * game;
	list<shared_ptr<Sprite>> sprites;
	shared_ptr<WeatherControl> weather;

	static ALLEGRO_BITMAP *floorTile;
	static ALLEGRO_BITMAP *wallTile;
	static ALLEGRO_BITMAP *snowTile[MAX_SNOW_TILES];
	static ALLEGRO_MOUSE_CURSOR *cursors[3];
private:
	int last_mouse_x, last_mouse_y;
	Trixel cells[CELLNUM];

	int cellIndex (int cx, int cy, int cz)
	{
		return cx + (cy * sizex) + (cz * sizey * sizey);
	}


public:
	enum Mode { BUILDING, REFILLING, DELETING, NONE };
private:
	Mode mode;
	shared_ptr<Particles> particles;
public:
	bool cursorValid;
	Block cursor_block;
	IsoCanvas(GameImpl* parent) : IsoGrid (DIMXY, DIMXY, DIMZ, TILEXY, TILEH), game(parent), sprites(), weather(),
			last_mouse_x(0), last_mouse_y(0), cells(), mode(NONE), cursorValid(false), cursor_block(this)
	{
		particles = make_shared<Particles>(this);
		particles->setLocation(0, 0, 768, 600);
		particles->setSnowForce(0);

		weather = make_shared<WeatherControl>(this, particles.get());
	};

	Mode getMode()
	{
		return mode;
	}

	void setMode(Mode value)
	{
		if (value == mode) return;

		mode = value;
		switch (mode)
		{
		case NONE:
			al_set_system_mouse_cursor(al_get_current_display(), ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
			break;
		case REFILLING:
			al_set_mouse_cursor(al_get_current_display(), cursors[2]);
			break;
		case DELETING:
			al_set_mouse_cursor(al_get_current_display(), cursors[1]);
			break;
		case BUILDING:
			al_set_mouse_cursor(al_get_current_display(), cursors[0]);
			break;
		}

	}

	virtual void draw(const GraphicsContext &gc) override;

	Trixel *getCell(int cx, int cy, int cz)
	{
		if (!cellInBounds(cx, cy, cz)) return NULL;
		return &(cells[cellIndex(cx, cy, cz)]);
	}

	virtual bool canPutBlock(int cx, int cy, int cz, BlockType blockType)
	{
		// check if the grid is blocked
		BlockInfo *info = getBlockInfo(blockType);
		for (int x = 0; x < info->csizeX; ++x)
			for (int y = 0; y < info->csizeY; ++y)
				for (int z = 0; z < info->csizeZ; ++z)
				{
					Trixel *cell = getCell(cx +x , cy + y, cz + z);
					if (cell == NULL) return false;
					if (cell->occupied) return false;
				}

		if (cz == 0) return true; // block on floor is ok.

		Trixel *below = getCell(cx, cy, cz - 1);
		assert (below != NULL); // shouldn't happen as we aleady checked floor

		return (below->solid);
	}

private:
	void adjustCursorPosition (ALLEGRO_EVENT &event)
	{
		// update block position
		float cursor_ix, cursor_iy, cursor_iz;


		screenToIso (0, event.mouse.x, event.mouse.y, cursor_ix, cursor_iy);
		int cx = cursor_ix / TILEXY;
		int cy = cursor_iy / TILEXY;
		int cz = 0;
		if (cellInBounds(cx, cy, cz))
		{
			while (getCell(cx, cy, cz)->solid && cz < (DIMZ - 1))
			{
				cz++;
			}

			cursor_block.cx = cx;
			cursor_block.cy = cy;
			cursor_block.cz = cz;

			cursorValid = canPutBlock(cx, cy, cz, cursor_block.getBlockType());
		}

		last_mouse_x = event.mouse.x;
		last_mouse_y = event.mouse.y;
	}

	shared_ptr<Block> getBlockAt(int mrx, int mry)
	{
		//sort by z-order
		sprites.sort ([](shared_ptr<Sprite> &a, shared_ptr<Sprite> &b){ return a->getRz() < b->getRz(); });

		for (auto i = sprites.rbegin(); i != sprites.rend(); ++i)
		{
			shared_ptr<Block> block = dynamic_pointer_cast<Block>(*i);
			if (!block) continue;

			double drx = (*i)->getRx() - mrx;
			double dry = (*i)->getRy() - mry;

			// pythagoras distance
			if (drx * drx + dry * dry < 32 * 32)
			{
				return block;
			}
		}

		return shared_ptr<Block>();
	}

	void handleMouseClick (ALLEGRO_EVENT &event);
public:
	virtual void handleEvent(ALLEGRO_EVENT &event) override;

	void clearCells()
	{
		for (int x = 0; x < sizex; ++x)
			for (int y = 0; y < sizey; ++y)
				for (int z = 0; z < sizez; ++z)
					cells[cellIndex(x, y, z)] = Trixel();
	}

	/** create new block object and update map */
	void addBlock (BlockType bt, int cx, int cy, int cz)
	{
		assert (cellInBounds(cx, cy, cz));

		assert (cellInBounds(
				cx + blockInfo[bt - BLOCK].csizeX - 1,
				cy + blockInfo[bt - BLOCK].csizeY - 1,
				cz + blockInfo[bt - BLOCK].csizeZ - 1));

		auto block = make_shared<Block>(this);
		block->setBlockType(bt);
		block->setPosition(cx, cy, cz);

		bool isSolid = blockInfo[bt - BLOCK].isSolid;
		block->applyCellRange([=](Trixel* cell) -> void { cell->occupied = block.get(); cell->solid = isSolid; cell->snow = 0; });

		sprites.push_back(block);
	}

	void addBun ();

	/** remove block object and update map */
	void destroyBlock (shared_ptr<Block> &block)
	{
		block->alive = false;
		block->applyCellRange([=](Trixel* cell) -> void { cell->occupied = NULL; cell->solid = false; cell->foodLeft = 0; });
	}

	virtual void onUpdate () override
	{
		for (auto i = sprites.begin(); i != sprites.end(); ++i)
		{
			if ((*i)->alive) (*i)->update();
		}

		particles->update();

		diffusion();

		sprites.remove_if ([](shared_ptr<Sprite> i) {
			return !(i->alive); });

		weather->update();
	}

	void avg (double *temp, int cx, int cy, int cz)
	{
		double sum = 0;
		int n = 0;

		n++; sum += temp[cellIndex(cx, cy, cz)];

		if (cx > 0)
		{ n++; sum += temp[cellIndex(cx - 1, cy, cz)]; }
		if (cx < DIMXY-1)
		{ n++; sum += temp[cellIndex(cx + 1, cy, cz)]; }

		if (cy > 0)
		{ n++; sum += temp[cellIndex(cx, cy - 1, cz)]; }
		if (cy < DIMXY-1)
		{ n++; sum += temp[cellIndex(cx, cy + 1, cz)]; }

		if (cz > 0)
		{ n++; sum += temp[cellIndex(cx, cy, cz - 1)]; }
		if (cz < DIMZ-1)
		{ n++; sum += temp[cellIndex(cx, cy, cz + 1)]; }

		// set average
		double avg = (sum / n);

		// decay
		cells[cellIndex(cx, cy, cz)].smell = max_f (0, avg - 0.15);
	}

	void diffusion()
	{
		double temp [CELLNUM];

		// seed new
		for (int x = 0; x < DIMXY; ++x)
			for (int y = 0; y < DIMXY; ++y)
				for (int z = 0; z < DIMZ; ++z)
				{
					int idx = cellIndex(x, y, z);
					temp[idx] = cells[idx].smell + cells[idx].foodLeft;
				}

		// calculate averages & fade, and put back
		for (int x = 0; x < DIMXY; ++x)
			for (int y = 0; y < DIMXY; ++y)
				for (int z = 0; z < DIMZ; ++z)
				{
					avg (temp, x, y, z);
				}

	}

};

ALLEGRO_BITMAP *IsoCanvas::floorTile = NULL;
ALLEGRO_BITMAP *IsoCanvas::wallTile = NULL;
ALLEGRO_BITMAP *IsoCanvas::snowTile[MAX_SNOW_TILES] = { NULL, NULL, NULL };
ALLEGRO_MOUSE_CURSOR *IsoCanvas::cursors[3] = { NULL, NULL, NULL };

class Bun;

class BunAction
{
public:
	bool finished;
	Bun *bun;

	BunAction (Bun *bun) : finished(false), bun(bun) {}
	virtual ~BunAction() {}
	virtual void update() = 0;
};


class Bun : public Sprite
{
private:
	int state;
public:
	static Anim *anim;
	static ALLEGRO_SAMPLE *samBinky;
	static ALLEGRO_SAMPLE *samHop;

	shared_ptr<BunAction> action;
	int dir;
	int w, h;
	int animStart;

	int hungry; // 0 to 100
	int happy; // 0 to 100

	Bun(IsoCanvas *grid) : Sprite(grid), state(0), action(), dir(0), w(64), h(64), animStart(0), hungry(50), happy(50) {}

	void setState(int value)
	{
		state = value; animStart = MainLoop::getMainLoop()->getMsecCounter();
	}

public:
	virtual void update() override
	{
		if (action)
		{
			action->update();
			if (action->finished)
				action = 0;
		}

		if (!action)
		{
			determineNextAction();
		}

	}

	void determineNextAction();

	// should be tried with either S or E
	bool canEat(int dir)
	{
		Trixel *adjacent = grid->getCell(cx + pathInfo[dir].dx, cy + pathInfo[dir].dy, cz);
		return (adjacent != NULL && adjacent->foodLeft > 0);
	}

	bool canEat()
	{
		return canEat(S) || canEat(E) || canEat(N) || canEat (W);
	}

	bool canMove(PathType newPath)
	{
		assert (newPath  >= 0);
		assert (newPath < NUM_PATH_TYPES);

		int dx = pathInfo[newPath].dx;
		int dy = pathInfo[newPath].dy;
		int dz = pathInfo[newPath].dz;

		int ncx = cx + dx;
		int ncy = cy + dy;
		int ncz = cz + dz;

		if (!grid->cellInBounds(ncx, ncy, ncz)) return false;

		Trixel *cell = grid->getCell(ncx, ncy, ncz);
		if (cell->occupied != NULL) return false;

		// check if new location has solid ground
		if (ncz > 0)
		{
			Trixel *below = grid->getCell(ncx, ncy, ncz - 1);
			if (!below->solid) return false;
		}

		return true;
	}

	virtual void draw(const GraphicsContext &gc) override
	{
		int x = getRx() - gc.xofst;
		int y = getRy() - gc.yofst;

		int msec = MainLoop::getMainLoop()->getMsecCounter();
		anim->drawFrame (gc.buffer, state, dir, msec - animStart, x, y);

		if (debugMode)
			al_draw_textf(smallFont, YELLOW, x, y, ALLEGRO_ALIGN_LEFT, "hungry %i happy %i", hungry, happy);
	}

	void addMoneyAndScore(int profit);

};

Anim *Bun::anim = NULL;
ALLEGRO_SAMPLE *Bun::samBinky = NULL;
ALLEGRO_SAMPLE *Bun::samHop = NULL;

class CostButton : public Button
{
	string baseText;
	int cost;
public:
	CostButton(int action, std::string _text, ALLEGRO_BITMAP* icon, int cost) : Button (action, _text, icon), cost(cost)
	{
		baseText = _text;
		text = baseText + " $" + to_string(cost);
	}

	int getCost() { return cost; }
	void setCost(int val)
	{
		cost = val;
		text = baseText + " $" + to_string(cost);
	}
};

class GameImpl : public Game
{
	int money;
	int BUN_COST;
	int freeMoneyTimer;

	shared_ptr<IsoCanvas> canvas;

	friend class IsoCanvas;
	Engine *parent;

	list<shared_ptr<CostButton>> costButtons;

	shared_ptr<CostButton> btnBuyBun;
public:
		GameImpl(Engine *parent) : money(0), freeMoneyTimer(0), canvas(), parent(parent), costButtons()
	{
			BUN_COST = BUN_COST_START;
	}

	virtual ~GameImpl()
	{
	}

	int getMoney()
	{
		return money;
	}
	void setMoney(int value) { money = value;  updateButtons(); /* TODO: feedback */ }
	void addMoney(int value) { money += value; updateButtons(); /* TODO: feedback */ }

	virtual void updateButtons()
	{
		for (auto i = costButtons.begin(); i != costButtons.end(); ++i)
		{
			(*i)->setEnabled(money >= (*i)->getCost());
		}
	}

	virtual void init() override
	{
		Resources *res = parent->getResources();

		add (ClearScreen::build(BLACK).get());
		add (make_shared<Input>(ALLEGRO_KEY_F5, 0, TOGGLE_DEBUGMODE));
		add (make_shared<Input>(ALLEGRO_KEY_ESCAPE, 0, CANCEL));
		add (make_shared<Input>(ALLEGRO_KEY_SPACE, ALLEGRO_KEY_ENTER, PAUSE_MENU));

		canvas = make_shared<IsoCanvas>(this);
		canvas->setLocation(0, 0, 768, 600);
		add (canvas);

		const int buttonw = 160;
		const int buttonh = 24;
		const int margin = 10;
		const int panelw = buttonw + 2 * margin;
		const int buttonNum = 8;

		int yco = 0;
		int xco = 800 - panelw;

		auto pnlButtons = make_shared<IContainer>();
		pnlButtons->setLocation(xco, yco, panelw, ((buttonh + margin) * buttonNum) + margin);
		xco += margin;
		yco += margin;

//		pnlButtons->setBorder(Border::create()); //TODO
		add (pnlButtons);

		auto b = make_shared<CostButton>(BIG_BLOCK, "Big Block", (ALLEGRO_BITMAP*)NULL, getBlockInfo(BIG_BLOCK)->cost);
		pnlButtons->add(b); costButtons.push_back(b); b->setLocation(xco, yco, buttonw, buttonh);	yco += buttonh + margin;
		b = make_shared<CostButton>(BLOCK, "Small Block", (ALLEGRO_BITMAP*)NULL, getBlockInfo(BLOCK)->cost);
		pnlButtons->add(b); costButtons.push_back(b); b->setLocation(xco, yco, buttonw, buttonh);	yco += buttonh + margin;
		b = make_shared<CostButton>(CARROTS, "Veg Plate", (ALLEGRO_BITMAP*)NULL, getBlockInfo(CARROTS)->cost);
		pnlButtons->add(b); costButtons.push_back(b); b->setLocation(xco, yco, buttonw, buttonh);	yco += buttonh + margin;
		b = make_shared<CostButton>(FOOD_BOWL, "Food Bowl", (ALLEGRO_BITMAP*)NULL, getBlockInfo(FOOD_BOWL)->cost);
		pnlButtons->add(b); costButtons.push_back(b); b->setLocation(xco, yco, buttonw, buttonh);	yco += buttonh + margin;
		b = make_shared<CostButton>(HAY, "Hay Rack", (ALLEGRO_BITMAP*)NULL, getBlockInfo(HAY)->cost);
		pnlButtons->add(b); costButtons.push_back(b); b->setLocation(xco, yco, buttonw, buttonh);	yco += buttonh + margin;
		auto c = make_shared<Button>(DELETE_BLOCK, "Delete", (ALLEGRO_BITMAP*)NULL);
		pnlButtons->add(c); c->setLocation(xco, yco, buttonw, buttonh);	yco += buttonh + margin;
		b = make_shared<CostButton>(BUY_REFILL, "Refill", (ALLEGRO_BITMAP*)NULL, REFILL_COST);
		pnlButtons->add(b); costButtons.push_back(b); b->setLocation(xco, yco, buttonw, buttonh);	yco += buttonh + margin;
		btnBuyBun = make_shared<CostButton>(BUY_BUNNY, "ウサギ",  (ALLEGRO_BITMAP*)NULL, BUN_COST);
		pnlButtons->add(btnBuyBun); costButtons.push_back(btnBuyBun); btnBuyBun->setLocation(xco, yco, buttonw, buttonh);	yco += buttonh + margin;

		auto pnlButtons2 = make_shared<IContainer>();
		xco = 0;
		yco = 0;
		pnlButtons2->setLocation(xco, yco, panelw, ((buttonh + margin) * buttonNum) + margin);
		xco += margin;
		yco += margin;
		c =  make_shared<Button>(QUIT, "Main Menu");
		pnlButtons2->add(c); c->setLocation(xco, yco, buttonw, buttonh);	yco += buttonh + margin;
		add (pnlButtons2);

		add (TextComponent::build(CYAN, ALLEGRO_ALIGN_LEFT, "Money").xywh(10, yco, 60, 16).get());
		add (IntCounter::build(CYAN, bind (&GameImpl::getMoney, this)).xywh(70, yco, 40, 16).get());

		Bun::anim = res->getAnim("Bun");
		Bun::samBinky = res->getSample("binky");
		Bun::samHop = res->getSample("jump1");
		smallFont = al_create_builtin_font();

		IsoCanvas::floorTile = res->getBitmap("Tile0000");
		IsoCanvas::wallTile = res->getBitmap("Castle0000");

		IsoCanvas::snowTile[0] = res->getBitmap("Snow0004");
		IsoCanvas::snowTile[1] = res->getBitmap("Snow0003");
		IsoCanvas::snowTile[2] = res->getBitmap("Snow0001");
		IsoCanvas::snowTile[3] = res->getBitmap("Snow0002");
		IsoCanvas::snowTile[4] = res->getBitmap("Snow0000");

		// TODO: need to be destroyed as well...
		IsoCanvas::cursors[0] = al_create_mouse_cursor(res->getBitmap("Mouse0000"), 16, 0);
		IsoCanvas::cursors[1] = al_create_mouse_cursor(res->getBitmap("Remove0000"), 16, 16);
		IsoCanvas::cursors[2] = al_create_mouse_cursor(res->getBitmap("Feedme0000"), 16, 32);

		// initialise anims
		for (int i = 0; i < NUM_BLOCK_TYPES; ++i)
		{
			if (blockInfo[i].animName != NULL)
			{
				blockInfo[i].anim = res->getAnim(blockInfo[i].animName);
			}
		}

		initGame();
		updateButtons();
	}

	virtual bool onHandleMessage(int event) override
	{
		switch (event)
		{
		case QUIT:
			canvas->setMode(IsoCanvas::NONE); // make sure to reset cursor
			pushMsg(Engine::MSG_MAIN_MENU);
			break;
		case CANCEL:
			if (canvas->getMode() != IsoCanvas::NONE)
				canvas->setMode(IsoCanvas::NONE);
			break;
		case TOGGLE_DEBUGMODE:
			debugMode = !debugMode;
//			if (debugMode) addMoney (1000);
			break;
		case BIG_BLOCK:
		case BLOCK:
		case FOOD_BOWL:
		case HAY:
		case CARROTS:
		{
			// build mode
			canvas->setMode(IsoCanvas::BUILDING);
			canvas->cursor_block.setBlockType ((BlockType)event);
			canvas->cursor_block.setPosition(0, 0, 0);
		}
			break;
		case BUY_BUNNY:
		{
			int cost = BUN_COST;
			if (cost <= getMoney())
			{
				addMoney(-cost);
				canvas->addBun();
				BUN_COST += BUN_COST_INCREASE;
				btnBuyBun->setCost (BUN_COST);
			}
			else
			{
				//TODO: feedback not enough money
			}
		}
			break;
		case DELETE_BLOCK:
			canvas->setMode(IsoCanvas::DELETING);
			break;
		case BUY_REFILL:
			canvas->setMode(IsoCanvas::REFILLING);
			break;
		}
		return true;
	}

	virtual void onUpdate() override
	{
		freeMoneyTimer++;
		if (freeMoneyTimer > FREE_MONEY_PERIOD)
		{
			freeMoneyTimer = 0;
			addMoney(1); //
		}
	}
private:


	void initGame()
	{
		money = START_MONEY;
		BUN_COST = BUN_COST_START;

		// initialise Cells
		canvas->clearCells();

		canvas->addBlock (BIG_BLOCK, 0, 0, 0);
		canvas->addBlock (BLOCK, 2, 0, 0);
		canvas->addBlock (BLOCK, 0, 2, 0);
		canvas->addBlock (FOOD_BOWL, 0, 0, 2);

		canvas->addBlock (CARROTS, 8, 4, 0);
		canvas->addBlock (HAY, 4, 8, 0);

		// initialise Sprites
		canvas->addBun();
		canvas->addBun();
	}


};

shared_ptr<Game> Game::newInstance(Engine *parent)
{
	return make_shared<GameImpl>(parent);
}

void IsoCanvas::draw(const GraphicsContext &gc)
{
	// draw back wall
	int yco = getYorig();
	int mid = getXorig();
	int xco = 0;
	for (int i = 0; i < 9; ++i)
	{
		al_draw_bitmap (wallTile, mid + xco - 52, yco - 250, 0);
		al_draw_bitmap (wallTile, mid - xco - 44, yco - 250, ALLEGRO_FLIP_HORIZONTAL);

		xco += 40;
		yco += 20;
	}

	// draw floor
	for (int x = 0; x < DIMXY; ++x)
		for (int y = 0; y < DIMXY; ++y)
		{
			float ix, iy;
			canvasFromIso_f(x * TILEXY, y * TILEXY, 0, ix, iy);
			al_draw_bitmap (floorTile, ix - 32, iy - 32, 0);

			Trixel *cell = getCell(x, y, 0);
			if (cell->snow > 0)
			{
				assert (cell->snow <= MAX_SNOW_TILES);
				int flipFlags = (x * 3 + y * 7) % 2;
				al_draw_bitmap(snowTile[cell->snow-1], ix - 32, iy - 32, flipFlags);
			}
		}

	if (debugMode)
		for (int x = 0; x < DIMXY; ++x)
			for (int y = 0; y < DIMXY; ++y)
				for (int z = 0; z < 1; ++z)
				{
					int idx = cellIndex(x, y, z);
					float ix, iy;
					canvasFromIso_f(x * 32, y * 32, z * 32, ix, iy);

					al_draw_textf(smallFont, GREEN, ix + (7 * z), iy + (5 * z), ALLEGRO_ALIGN_LEFT, "%.1f", cells[idx].smell);
				}

	//sort by z-order
	sprites.sort ([](shared_ptr<Sprite> &a, shared_ptr<Sprite> &b){ return a->getRz() < b->getRz(); });
	for (auto i = sprites.begin(); i != sprites.end(); ++i)
	{
		(*i)->draw(gc);
	}

	if (debugMode)
		al_draw_textf(sfont, GREEN, 0, 16, ALLEGRO_ALIGN_LEFT, "mouse: (%i, %i) cursor(%i, %i, %i), #sprites %i",
				last_mouse_x, last_mouse_y, cursor_block.cx, cursor_block.cy, cursor_block.cz, sprites.size());

	if (mode == BUILDING)
	{
		cursor_block.draw(gc);
		ALLEGRO_COLOR col = cursorValid ? GREEN : RED;
		int sx = cursor_block.getInfo()->csizeX;
		int sy = cursor_block.getInfo()->csizeY;
		int sz = cursor_block.getInfo()->csizeZ;
		drawWireFrame (cursor_block.getRx(), cursor_block.getRy(), 32 * sz, 32 * sy, 32 * sz, col);
		if (cursor_block.cz > 0)
		{
			drawSurfaceWire(gc, cursor_block.cx, cursor_block.cy, 0, sx, sy, col);
		}
	}


	if (debugMode)
		isoDrawWireFrame (0, 0, 256, 256, 256, WHITE);

	particles->draw(gc);
}

int Sprite::getRx()
{
	return grid->isoToScreenX((cx * 32) + px, (cy * 32) + py, cz * 32 + pz);
}

int Sprite::getRz()
{
	return grid->isoToScreenZ((cx * 32) + px, (cy * 32) + py, cz * 32 + pz);
}

int Sprite::getRy()
{
	return grid->isoToScreenY((cx * 32) + px, (cy * 32) + py, cz * 32 + pz);
}


class BunAdder : public Sprite
{
public:
	BunAdder(IsoCanvas *parent) : Sprite(parent) {}

	virtual void update() override
	{
		if (!alive) return;

		int cx = rand() % DIMXY;
		int cy = rand() % DIMXY;
		int cz = 0;

		Trixel *cell = grid->getCell(cx, cy, cz);
		assert (cell != NULL);

		if (!cell->occupied)
		{
			auto bun = make_shared<Bun>(grid);
			cell->occupied = bun.get();
			bun->setPosition(cx, cy, cz);
			grid->sprites.push_back(bun);
			alive = false;
		}

	}
};

void IsoCanvas::addBun ()
{
	auto adder = make_shared<BunAdder>(this);
	sprites.push_back (adder);
}

void Block::applyCellRange(const function<void(Trixel *)> &func)
{
	for (int x = 0; x < info->csizeX; ++x)
		for (int y = 0; y < info->csizeY; ++y)
			for	(int z = 0; z < info->csizeZ; ++z)
			{
				Trixel *cell = grid->getCell(cx +x, cy +y, cz + z);
				assert (cell != NULL);
				func(cell);
			}
}

void Block::draw(const GraphicsContext &gc)
{
	int rx = getRx();
	int ry = getRy();

	int cxs = info->csizeX;
	int cys = info->csizeY;
	int czs = info->csizeZ;

	ALLEGRO_COLOR color;
	Anim *anim;

	anim = info->anim;

	Trixel *cell = grid->getCell(cx, cy, cz);
	assert (cell);

	if (anim == NULL)
	{
		drawWireFrame(rx, ry, cxs * TILEXY, cys * TILEXY, czs * TILEZ, BLUE);
	}
	else
	{
		anim->drawFrame (gc.buffer, 0, 0, 100 - cell->foodLeft, rx, ry);
	}
}

class BunMoveAction : public BunAction
{
public:
	double dx;
	double dy;
	double dz;

	double amplitude;
	double period;

	int maxCounter;
	int counter;

	int oldcx;
	int oldcy;
	int oldcz;

	BunMoveAction(Bun *bun, PathType newPath) : BunAction(bun), dx(0), dy(0), dz(0),
			amplitude(0), period(0), maxCounter(0), counter(0), oldcx(0), oldcy(0), oldcz(0)
	{
		oldcx = bun->cx;
		oldcy = bun->cy;
		oldcz = bun->cz;

		assert (newPath >= 0);
		assert (newPath < NUM_PATH_TYPES);

		dx = pathInfo[newPath].dx;
		dy = pathInfo[newPath].dy;
		dz = pathInfo[newPath].dz;

		if (dz != 0)
		{
			// Here is the trick - profit depends on height!
			int profit = (bun->cz + 1) * HOP_PROFIT;
			bun->addMoneyAndScore(profit);
			MainLoop::getMainLoop()->playSample(Bun::samHop);
			bun->setState(5);
		}
		else
		{
			bun->setState(0);
		}

		counter = 32; maxCounter = counter;

		amplitude = pathInfo[newPath].amplitude;
		period = pathInfo[newPath].period;

		bun->dir = pathInfo[newPath].dir;

		Trixel *cell = bun->grid->getCell(bun->cx + dx, bun->cy + dy, bun->cz);
		assert (cell);
		cell->occupied = bun;

		bun->hungry = min(100, bun->hungry + 1);
	}

	void update()
	{
		// continue current movement
		int rev = (maxCounter - counter);
		double phase = (double)counter / (double)maxCounter * period * M_PI;

		bun->px = rev * dx;
		bun->py = rev * dy;
		bun->pz = rev * dz + abs(sin(phase)) * amplitude;

		counter--;
		if (counter == 0)
		{
			bun->px = 0;
			bun->py = 0;
			bun->pz = 0;

			bun->cx += dx;
			bun->cy += dy;
			bun->cz += dz;
			endMove();
		}

	}

	void endMove()
	{
		Trixel *cell = bun->grid->getCell (oldcx, oldcy, oldcz);
		assert (cell);
		cell->occupied = NULL;
		finished = true;
	}
};

class BunRunAction : public BunAction
{
public:
	double dx;
	double dy;
	double dz;

	double amplitude;
	double period;

	int maxCounter;
	int counter;

	int oldcx;
	int oldcy;
	int oldcz;

	int stepCounter;
	bool slipping;

	PathType path;

	BunRunAction(Bun *bun, PathType newPath) : BunAction(bun), dx(0), dy(0), dz(0),
			amplitude(0), period(0), maxCounter(0), counter(0), oldcx(0), oldcy(0), oldcz(0),
			stepCounter(3), slipping(false), path(newPath)
	{
		assert (newPath >= 0);
		assert (newPath < 4);

		dx = pathInfo[newPath].dx;
		dy = pathInfo[newPath].dy;
		dz = 0; // we don't run up/down

		amplitude = pathInfo[newPath].amplitude;
		period = 1.0;

		bun->dir = pathInfo[newPath].dir;
		bun->setState(0);

		bun->hungry = min(100, bun->hungry + 1);

		initStep();
	}

	void initStep()
	{
		oldcx = bun->cx;
		oldcy = bun->cy;
		oldcz = bun->cz;

		counter = 16; maxCounter = counter;

		Trixel *cell = bun->grid->getCell(bun->cx + dx, bun->cy + dy, bun->cz);
		assert (cell);
		cell->occupied = bun;
	}

	void update()
	{
		// continue current movement
		int rev = ((maxCounter - counter) * TILEXY) / maxCounter;
		double phase = (double)counter / (double)maxCounter * period * M_PI;

		bun->px = rev * dx;
		bun->py = rev * dy;
		bun->pz = rev * dz + abs(sin(phase)) * amplitude;

		counter--;
		if (counter == 0)
		{
			bun->px = 0;
			bun->py = 0;
			bun->pz = 0;

			bun->cx += dx;
			bun->cy += dy;
			bun->cz += dz;
			endStep();
		}

	}

	void endStep()
	{
		Trixel *cell = bun->grid->getCell (oldcx, oldcy, oldcz);
		assert (cell);
		cell->occupied = NULL;

		stepCounter--;

		Trixel *newCell = bun->grid->getCell (bun->cx, bun->cy, bun->cz);
		assert (newCell);
		if (!slipping && stepCounter <= 0 && newCell->snow > 0)
		{
			slipping = true;
			amplitude = 0;
			bun->setState(6);
		}

		bool shouldMove = (stepCounter > 0 || slipping);
		bool canMove = bun->canMove(path);

		if (shouldMove && canMove)
		{
			initStep();
		}
		else
		{
			finished = true;
		}

	}
};

class BinkyAction : public BunAction
{
public:
	double amplitude;
	double period;

	int maxCounter;
	int counter;

	BinkyAction(Bun *bun) : BunAction(bun),
			amplitude(40), period(1.0), maxCounter(60), counter(60)
	{
		bun->setState(4);

		bun->hungry = min(100, bun->hungry + 2);
		bun->happy = max(0, bun->happy - 10);

		bun->addMoneyAndScore(BINKY_PROFIT * (bun->cz + 1));
		MainLoop::getMainLoop()->playSample(Bun::samBinky);
	}

	void update()
	{
		// continue current movement
		double phase = (double)counter / (double)maxCounter * period * M_PI;
		bun->pz = abs(sin(phase)) * amplitude;

		counter--;
		if (counter == 0)
		{
			bun->pz = 0;
			finished = true;
		}
	}

};

class BunWaitAction : public BunAction
{
	int timer;
public:
	BunWaitAction (Bun *bun, int state, int timerBase) : BunAction(bun)
	{
		bun->setState(state);
		timer = timerBase + (rand() % timerBase);
	}

	virtual void update() override
	{
		timer--;
		if (timer <= 0)
		{
			finished = true;
		}
	}
};

class BunEatAction : public BunAction
{
	int timer;
public:
	BunEatAction (Bun *bun) : BunAction(bun), timer(200)
	{
		int newdir = -1;
		for (int i = 0; i < 4; ++i)
		{
			if (bun->canEat(i))
			{
				newdir = i;
			}
		}

		if (newdir < 0)
		{
			finished = true;
			timer = 0;
			return;
		}

		bun->dir = newdir;
		bun->setState(3);

		Trixel *adjacent = bun->grid->getCell(
				bun->cx + pathInfo[newdir].dx,
				bun->cy + pathInfo[newdir].dy,
				bun->cz);
		assert (adjacent != NULL);

		adjacent->foodLeft = max(0, adjacent->foodLeft - 20);
		bun->hungry = max(0, bun->hungry - 40);
		bun->happy = min(100, bun->happy + 10);
	}

	virtual void update() override
	{
		timer--;
		if (timer <= 0)
		{
			finished = true;
		}
	}
};

void Bun::determineNextAction()
{
	// here is the trick: money is added depending on climbing height...
	int choice;

	if (hungry > 70)
	{
		happy = max (0, happy - 1);
	}

	choice = rand() % 100;
	if (canEat() && choice < hungry)
	{
		action = make_shared<BunEatAction>(this); // eat
		return;
	}

	choice = rand() % 100;
	if (choice < (happy - 60))
	{
		action = make_shared<BinkyAction>(this); // binky
		return;
	}

	choice = rand() % 100;
	if (choice < 10)
	{
		// running
		// we're going to move.
		// choose a direction
		int maxit = 0;
		int path = 0;

		bool foundPath = false;
		int choice2 = rand() % 100;

		while (maxit < 8)
		{
			maxit++;
			path = rand() % 4;

			if (!canMove((PathType)path)) continue;

			// are we hungry?
			if (choice2 < (hungry - 10))
			{
				int nx = cx + pathInfo[path].dx;
				int ny = cy + pathInfo[path].dy;
				int nz = cz + pathInfo[path].dz;
				// then check if we're going towards food;
				double smell = grid->getCell(nx, ny, nz)->smell;
				double nsmell = grid->getCell(cx, cy, cz)->smell;
				if (nsmell > smell)
					// reject this direction
					continue;
			}

			foundPath = true;
			break;
		}

		if (foundPath)
		{
			action = make_shared<BunRunAction>(this, (PathType)path);
		}
		else
		{
			action = make_shared<BunWaitAction>(this, 1, 70); // sit
		}
	}
	else if (choice < 60)
	{
		// we're going to move.
		// choose a direction
		int maxit = 0;
		int path = 0;

		bool foundPath = false;
		int choice2 = rand() % 100;

		while (maxit < (NUM_PATH_TYPES * 2))
		{
			maxit++;
			path = rand() % NUM_PATH_TYPES;

			if (!canMove((PathType)path)) continue;

			// are we hungry?
			if (choice2 < (hungry - 10))
			{
				int nx = cx + pathInfo[path].dx;
				int ny = cy + pathInfo[path].dy;
				int nz = cz + pathInfo[path].dz;
				// then check if we're going towards food;
				double smell = grid->getCell(nx, ny, nz)->smell;
				double nsmell = grid->getCell(cx, cy, cz)->smell;
				if (nsmell > smell)
					// reject this direction
					continue;
			}

			foundPath = true;
			break;
		}

		if (foundPath)
		{
			action = make_shared<BunMoveAction>(this, (PathType)path);
		}
		else
		{
			action = make_shared<BunWaitAction>(this, 1, 70); // sit
		}
	}
	else if (choice < 80)
	{
		action = make_shared<BunWaitAction>(this, 1, 70); // sit
	}
	else if (choice < 97)
	{
		action = make_shared<BunWaitAction>(this, 2, 70); // stand
	}
	else
	{
		action = make_shared<BunWaitAction>(this, 7, 200); // sleep
	}

}

void IsoCanvas::handleMouseClick (ALLEGRO_EVENT &event)
{
	if (event.mouse.button != 1) { setMode(NONE); return; }

	switch (mode)
	{
		case BUILDING:
		{
			adjustCursorPosition(event);

			int cost = cursor_block.getInfo()->cost;
			if (cost <= game->getMoney())
			{
				if (canPutBlock(cursor_block.cx, cursor_block.cy, cursor_block.cz, cursor_block.getBlockType()))
				{
					addBlock(cursor_block.getBlockType(), cursor_block.cx, cursor_block.cy, cursor_block.cz);
					game->addMoney(-cost);
					setMode (NONE);
				}
			}
			else
			{
				//TODO: feedback not enough money
			}
		}
			break;
		case REFILLING:
		{
			shared_ptr<Block> block = getBlockAt(event.mouse.x, event.mouse.y);
			if (block && block->getInfo()->refillable)
			{
				getCell(block -> cx, block-> cy, block->cz)->foodLeft = 100;
				game->addMoney(-REFILL_COST);
				setMode (NONE);
			}
		}
			break;
		case DELETING:
		{
			shared_ptr<Block> block = getBlockAt(event.mouse.x, event.mouse.y);
			if (block)
			{
				destroyBlock(block);
				setMode (NONE);
			}
		}
			break;
		case NONE: // do nothing
			break;
	}
}

void IsoCanvas::handleEvent(ALLEGRO_EVENT &event)
{
	switch (event.type)
	{
	case ALLEGRO_EVENT_MOUSE_AXES:
		if (mode == BUILDING) adjustCursorPosition(event);
		break;
	case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
		handleMouseClick(event);
		break;
	}
}

void Bun::addMoneyAndScore(int profit)
{
	grid->game->addMoney (profit);
	auto score = make_shared<Score>(grid, profit);
	score->setPosition(cx, cy, cz);
	grid->sprites.push_back(score);
}

void WeatherControl::update()
{
	counter--;
	if (counter <= 0)
	{
		// decide
		snowForce = (rand() % 500) - 300;
		if (snowForce < 0) snowForce = 0;
		counter = rand() % 1000 + 500;
		particles->setSnowForce(snowForce);

		// after every change, it takes a while for snow to be affected
		snowTileCounter = 200;
	}

	snowTileCounter--;
	if (snowTileCounter <= 0)
	{
		snowTileCounter = 8;

		// TODO: make grid slippery
		int x = rand() % DIMXY;
		int y = rand() % DIMXY;

		// pick a random tile and add or remove snow.
		Trixel *cell = canvas->getCell(x, y, 0);
		if (cell->occupied) cell->snow = 0;
		else
		{
			if (snowForce == 0) cell->snow = max (cell->snow - 1, 0);
			else cell->snow = min (cell->snow + 1, MAX_SNOW_TILES);
		}
	}

}
