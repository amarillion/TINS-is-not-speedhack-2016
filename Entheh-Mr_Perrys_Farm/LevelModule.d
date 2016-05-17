module LevelModule;
import Main;
import allegro5.allegro;
import allegro5.allegro_primitives;
import Maths;
import Colours;
import std.string;
import std.math;
import std.random;
import Scopes;

enum numGrasses = 256;

enum TileType : ubyte {
	None,
	Wall,
	FirstAnimal = 64,
	FirstFood = 128,
}
bool isAnimal(TileType tileType) { return tileType >= TileType.FirstAnimal && tileType <= AnimalType.max.toTileType; }
bool isFood(TileType tileType) { return tileType >= TileType.FirstFood && tileType <= FoodType.max.toTileType; }
AnimalType toAnimalType(TileType tileType) { return cast(AnimalType)(tileType - TileType.FirstAnimal); }
FoodType toFoodType(TileType tileType) { return cast(FoodType)(tileType - TileType.FirstFood); }
TileType toTileType(AnimalType animalType) { return cast(TileType)(TileType.FirstAnimal + animalType); }
TileType toTileType(FoodType foodType) { return cast(TileType)(TileType.FirstFood + foodType); }

bool animalCanGo(TileType tileType, AnimalType animalType) { return tileType == TileType.None || (tileType.isFood && tileType.toFoodType != cast(FoodType)animalType); }

struct Tile {
	TileType type;
}

enum AnimalType : ubyte {
	Cow,
	Sheep,
	Pig,
	Duck,
}

enum FoodType : ubyte {
	Beef,
	Lamb,
	Bacon,
	Duck,
}

enum ComponentType : ubyte {
	Bomb,
	TurnChanger,
	Multiply,
	Rain,
	Snow,
	Sun,
}

struct Decal {
	ALLEGRO_BITMAP* sprite;
	int x, y, flags;
	float scale, angle;
}

struct Level
{
	enum w=27, h=27;

	//Show the game in a square area matching the screen height.
	enum gameWidth = screenHeight, gameHeight = screenHeight;
	enum tw = gameWidth / w;
	enum th = gameHeight / h;

	Tile[w][h] map;

	//Number of items of each tile type that are placeable in the gameplay setup phase
	ubyte[256] placeables;

	//Number of components of each type that are available for your machine
	ubyte[256] components;

	static Decal[] decals;

	ref Tile mapWrapped(int x, int y) {
		return map[y.modUnsigned($)][x.modUnsigned($)];
	}

	void draw()
	{
		auto clip = Clip(0, 0, gameWidth, gameHeight);
		al_draw_filled_rectangle(0, 0, gameWidth, gameHeight, green);
		foreach (ref decal; decals) {
			float sw = al_get_bitmap_width(decal.sprite);
			float sh = al_get_bitmap_height(decal.sprite);
			al_draw_scaled_rotated_bitmap(decal.sprite, sw/2, sh/2, decal.x+sw/2, decal.y+sh/2, decal.scale, decal.scale, decal.angle, decal.flags);
		}
		foreach (y; 0..h) {
			foreach (x; 0..w) {
				drawTile(x*tw, y*th, map[y][x]);
			}
		}
	}

	static void drawTile(float x, float y, Tile tile, float direction=0) {
		if (tile.type.isAnimal) {
			auto s = animalSprites[tile.type.toAnimalType];
			float sw = al_get_bitmap_width(s);
			float sh = al_get_bitmap_height(s);
			al_draw_scaled_rotated_bitmap(s, sw/2, sh/2, x+tw/2, y+th/2, tw/sw, th/sh, direction*PI/2, 0);
		} else if (tile.type.isFood) {
			al_draw_bitmap(foodSprites[tile.type.toFoodType], x, y, 0);
		} else if (tile.type != TileType.None) {
			al_draw_bitmap(wallSprite, x, y, 0);
		}
		/*
		ALLEGRO_COLOR colour;
		final switch (tile.type) {
			case TileType.None: colour = black; break;
			case TileType.Wall: colour = green; break;
			case AnimalType.Cow  .toTileType: colour = al_map_rgb(255,0,0); break;
			case AnimalType.Sheep.toTileType: colour = al_map_rgb(180,180,180); break;
			case AnimalType.Pig  .toTileType: colour = al_map_rgb(255,140,180); break;
			case AnimalType.Duck .toTileType: colour = al_map_rgb(0,60,60); break;
			case FoodType.Beef   .toTileType: colour = al_map_rgb(0,60,60); break;
			case FoodType.Lamb   .toTileType: colour = al_map_rgb(0,60,60); break;
			case FoodType.Bacon  .toTileType: colour = al_map_rgb(0,60,60); break;
			case FoodType.Duck   .toTileType: colour = al_map_rgb(0,60,60); break;
		}
		al_draw_filled_rectangle(x, y, x+tw, y+th, colour);
		*/
	}

	static void drawComponent(float x, float y, ComponentType type, int direction) {
		al_draw_rotated_bitmap(componentSprites[type], tw/2, th/2, x+tw/2, y+th/2, direction*PI/2, 0);
		/*
		ALLEGRO_COLOR colour;
		final switch (type) {
			case ComponentType.Bomb: colour = al_map_rgb(60,60,60); break;
			case ComponentType.TurnChanger: colour = al_map_rgb(255,60,60); break;
			case ComponentType.Multiply: colour = al_map_rgb(60,255,60); break;
			case ComponentType.Rain: colour = al_map_rgb(60,60,255); break;
			case ComponentType.Snow: colour = al_map_rgb(255,255,255); break;
			case ComponentType.Sun: colour = al_map_rgb(255,255,60); break;
		}
		al_draw_filled_rectangle(x, y, x+tw, y+th, colour);
		*/
	}

	static void decodeMouse(ALLEGRO_EVENT* event, int* x, int* y) {
		*x = event.mouse.x.divRoundDown(tw);
		*y = event.mouse.y.divRoundDown(th);
	}

	enum currentVersion = 0;

	void save(string path)
	{
		auto f = al_fopen(path.toStringz(), "wb");
		if (!f) throw new Exception(format("Couldn't open file %s", path));
		scope (exit) al_fclose(f);

		al_fwrite(f, "MrPF".ptr, 4);
		al_fwrite32le(f, currentVersion);

		foreach (y; 0..h) {
			foreach (x; 0..w) {
				al_fputc(f, map[y][x].type);
			}
		}

		void saveSparse(in ubyte[256] array) {
			foreach (i; 0..256) {
				if (array[i]) {
					al_fputc(f, array[i]);
					al_fputc(f, i);
				}
			}
			al_fputc(f, 0);
		}
		saveSparse(placeables);
		saveSparse(components);

		if (al_ferror(f)) throw new Exception(format("Couldn't write file %s: %s", path, al_ferrmsg(f)));
		if (al_feof(f)) throw new Exception(format("Truncated file: %s", path));
	}

	void load(string path)
	{
		placeGrass();

		auto f = al_fopen(path.toStringz(), "rb");
		if (!f) throw new Exception(format("Couldn't open file %s", path));
		scope (exit) al_fclose(f);

		char[4] magic;
		al_fread(f, magic.ptr, 4);
		if (magic != "MrPF") throw new Exception(format("Not a Mr Perry's Farm file: %s", path));

		int version_ = al_fread32le(f);
		if (version_ < 0 || version_ > currentVersion) throw new Exception(format("File %s: unsupported version %s", path, version_));

		foreach (y; 0..h) {
			foreach (x; 0..w) {
				map[y][x] = Tile(cast(TileType)al_fgetc(f));
			}
		}

		void loadSparse(out ubyte[256] array) {
			array[] = 0;
			while (true) {
				int count = al_fgetc(f);
				if (count <= 0) break;
				int index = al_fgetc(f);
				if (index < 0) break;
				array[index] = cast(ubyte)count;
			}
		}
		loadSparse(placeables);
		loadSparse(components);

		if (al_ferror(f)) throw new Exception(format("Couldn't read file %s: %s", path, al_ferrmsg(f)));
		if (al_feof(f)) throw new Exception(format("Truncated file: %s", path));
	}

	void placeGrass() {
		decals.length = numGrasses;
		foreach (ref grass; decals) {
			grass.sprite = grassSprites[uniform(0, grassSprites.length)];
			int sw = al_get_bitmap_width(grass.sprite);
			int sh = al_get_bitmap_height(grass.sprite);
			grass.x = uniform(1-sw, gameWidth);
			grass.y = uniform(1-sh, gameHeight);
			grass.flags = uniform(0, 4);	//randomly flip horiz/vert
			grass.angle = 0;
			grass.scale = 1;
		}
	}
}
