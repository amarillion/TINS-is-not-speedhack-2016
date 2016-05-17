module Game;
import Main;
import std.math;
import allegro5.allegro;
import allegro5.allegro_primitives;
import allegro5.allegro_native_dialog;
import allegro5.allegro_font;
import allegro5.allegro_audio;
import LevelModule;
import Levels;
import Colours;
static import MainMenu;
import Maths;
import Buttons;
import std.string;
import std.array;
import Pathfinding;
import Scopes;
import Text;
import std.random;
import Particles;

//version = debugPathfinding;

void startGame() {
	string[] allLevelNames;
	GetLevelNames(allLevelNames);
	if (allLevelNames.length == 0) {
		al_show_native_message_box(display, "Error", "Error", "There are no levels!", null, ALLEGRO_MESSAGEBOX_ERROR);
	} else {
		theGame = new Game(allLevelNames);
	}
}

class Game
{
	enum w=Level.w, h=Level.h;

	//Simulation rate (we interpolate when rendering)
	enum tickDuration = 0.2f;
	//Move speeds, expressed as number of ticks to move one square
	enum machineMoveTicks = 2;
	enum animalMoveTicks = 6;
	enum rainedAnimalMoveTicks = 3;
	enum sunnedAnimalMoveTicks = 10;

	string[] allLevelNames;
	int levelNum;

	//This is a struct so it can be easily completely reinitialised.
	LevelSetup levelSetup;
	alias levelSetup this;

	//Temp storage
	CostCell[w][h] pathfindCosts;
	QueueEntry[w*h] pathfindQueue;

	//Editor entry point
	this(in Level levelToTest, string name) {
		initialLevel = levelToTest;
		allLevelNames ~= name;
		startSetup();
	}

	//Game entry point
	this(string[] levelNames) {
		allLevelNames = levelNames;
		initialLevel.load(levelsPath ~ allLevelNames[levelNum] ~ levelExt);
		startSetup();
	}

	void initNewLevel() {
		levelSetup = LevelSetup.init;
		initialLevel.load(levelsPath ~ allLevelNames[levelNum] ~ levelExt);
		startSetup();
	}

	void startSetup() {
		level = initialLevel;
		levelPlay = LevelPlay.init;
		level.decals.length = numGrasses;
	}

	void startPlay() {
		initialLevel = level;
		currentTile = Tile(TileType.None);
		draggingComponent = placingMachine = false;

		//Lift the animals out into moving sprites.
		foreach (y; 0..h) {
			foreach (x; 0..w) {
				if (level.map[y][x].type.isAnimal) {
					Animal animal;
					animal.type = level.map[y][x].type.toAnimalType;
					animal.x = x; animal.y = y;
					animals ~= animal;
					level.map[y][x] = Tile(TileType.None);
				}
			}
		}

		machines ~= machine;
		playStartTime = nextTickTime = time;
	}

	bool playing() { return !nextTickTime.isNaN; }

	void handleEvent(ALLEGRO_EVENT* event)
	{
		if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			int direction, index;
			if (!placingMachine && getHoveredMachineUiComponent(event.mouse.x, event.mouse.y, &direction, &index, draggingComponent)) {
				currentTile.type = TileType.None;
				if (draggingComponent && event.mouse.button == 1) {
					machine.components[direction].insertInPlace(index, currentComponent);
					if (--level.components[currentComponent] == 0) draggingComponent = false;
					if (machine.x >= 0 && !machineHasRoomAt(machine.x, machine.y)) machine.x = machine.y = -1;
				} else if (index < machine.components[direction].length) {
					currentComponent = machine.components[direction][index];
					draggingComponent = (event.mouse.button == 1);
					machine.components[direction].replaceInPlace(index, index+1, cast(ComponentType[])[]);
					level.components[currentComponent]++;
				}
			} else {
				draggingComponent = false;
				int x = event.mouse.x.divRoundDown(Level.tw);
				int y = event.mouse.y.divRoundDown(Level.th);
				if (x >= 0 && x < w && y >= 0 && y < h) {
					if (placingMachine) {
						if (machineHasRoomAt(x, y)) {
							machine.x = x;
							machine.y = y;
							placingMachine = false;
						}
					} else {
						Tile removedTile = Tile(TileType.None);
						if (placement[y][x]) {
							removedTile = level.map[y][x];
							level.map[y][x] = Tile(TileType.None);
							placement[y][x] = false;
							level.placeables[removedTile.type]++;
						}
						if (event.mouse.button == 1) {
							if (machineOccupies(x, y)) {
								currentTile = Tile(TileType.None);
								machine.x = machine.y = -1;
								placingMachine = true;
							} else if (currentTile.type == TileType.None) {
								currentTile = removedTile;
							} else if (level.map[y][x].type == TileType.None) {
								level.map[y][x] = currentTile;
								placement[y][x] = true;
								if (--level.placeables[currentTile.type] == 0) currentTile.type = TileType.None;
							}
						} else {
							currentTile.type = TileType.None;
						}
					}
				} else {
					currentTile.type = TileType.None;
					placingMachine = false;
				}
			}
		}
	}

	bool machineHasRoomAt(int x, int y) {
		if (level.map[y][x].type != TileType.None) return false;
		if (machine.components[0].length && level.mapWrapped(x, y-1).type != TileType.None) return false;
		if (machine.components[1].length && level.mapWrapped(x+1, y).type != TileType.None) return false;
		if (machine.components[2].length && level.mapWrapped(x, y+1).type != TileType.None) return false;
		if (machine.components[3].length && level.mapWrapped(x-1, y).type != TileType.None) return false;
		return true;
	}

	bool machineOccupies(int x, int y) {
		if (machine.x == x && machine.y == y) return true;
		if (machine.components[0].length && machine.x == x && (machine.y-1).modUnsigned(h) == y) return true;
		if (machine.components[1].length && (machine.x+1).modUnsigned(w) == x && machine.y == y) return true;
		if (machine.components[2].length && machine.x == x && (machine.y+1).modUnsigned(h) == y) return true;
		if (machine.components[3].length && (machine.x-1).modUnsigned(w) == x && machine.y == y) return true;
		return false;
	}

	void update()
	{
		if (playing) {
			updateParticles(explosions);
			updateParticles(bloodParticles, bloodSplat);
			updateParticles(weatherParticles[ComponentType.Rain], null);
			updateParticles(weatherParticles[ComponentType.Snow], null);
			updateParticles(weatherParticles[ComponentType.Sun ], null);
			updateWeatherParticles();
		}

		while (playing && time > nextTickTime) {
			//Block down the animals so that the machines can hit them.
			foreach (ref animal; animals) {
				animal.eating = level.map[animal.y][animal.x];
			}
			foreach (ref animal; animals) {
				level.map[animal.y][animal.x] = Tile(animal.type.toTileType);
				level.mapWrapped(animal.x-animal.dx, animal.y-animal.dy) = Tile(animal.type.toTileType);
			}

			//I want to be sure we iterate over machines newly made during the loop,
			//so I'm using a traditional 'for' loop and evaluating machines.length each time to make sure this happens.
			int writePos = 0;
			for (int readPos = 0; readPos < machines.length; readPos++) {
				auto machine = machines[readPos];

				machine.ticksTillMove--;
				if (machine.ticksTillMove >= 0) {
					machines[writePos++] = machine;
					continue;
				}
				machine.ticksTillMove += machineMoveTicks;

				newDirectionSearch: while (true) {
					//Work out where the centre of the machine will be.
					int probeX = machine.x, probeY = machine.y;
					final switch (machine.direction) {
						case 0: probeY--; break;
						case 1: probeX++; break;
						case 2: probeY++; break;
						case 3: probeX--; break;
					}

					//Check if any part of the machine is now in contact with something.
					bool hitSomething = false;
					bool shouldMultiply = false;
					void testHit(int dx, int dy, int direction) {
						auto hit = this.level.mapWrapped(probeX+dx, probeY+dy);
						if (hit.type != TileType.None) {
							hitSomething = true;
							if (hit.type.isAnimal) al_play_sample(animalSounds[hit.type.toAnimalType], 1f, ((probeX+dx+0.5f)/w)*2-1, 1f, ALLEGRO_PLAYMODE.ALLEGRO_PLAYMODE_ONCE, null);
							if (direction >= 0) {
								auto component = machine.components[direction][$-1];
								machine.components[direction].length--;
								blowComponent(machine, machine.x+dx, machine.y+dy, component, &shouldMultiply);
							}
						}
					}
					testHit(0, 0, -1);
					if (hitSomething) {
						//Machine itself has hit something!
						blowBomb(machine.x, machine.y);
						break newDirectionSearch;
					} else {
						if (machine.components[0].length > 0) testHit(0, -1, 0);
						if (machine.components[1].length > 0) testHit(+1, 0, 1);
						if (machine.components[2].length > 0) testHit(0, +1, 2);
						if (machine.components[3].length > 0) testHit(-1, 0, 3);
						if (hitSomething) {
							if (machine.turnAnticlockwise) machine.direction--; else machine.direction++;
							machine.direction &= 3;
							if (shouldMultiply) multiply(machine);
							continue newDirectionSearch;
						} else {
							//No hits - proceed.
							machine.x = probeX.modUnsigned(w);
							machine.y = probeY.modUnsigned(h);
							machines[writePos++] = machine;
							break newDirectionSearch;
						}
					}
				}
			}
			machines.length = writePos;

			//Remove the animals from the map. Deal with dead animals here too.
			writePos = 0;
			foreach (readPos, ref animal; animals) {
				level.mapWrapped(animal.x-animal.dx, animal.y-animal.dy) = Tile(TileType.None);
				level.map[animal.y][animal.x] = animal.eating;
				if (animal.dead) {
					numKills[animal.type]++;

					float cx = animal.x, cy = animal.y;
					float lag = ((nextTickTime - time) / tickDuration + animal.ticksLeftTillMove) / animal.startTicksTillMove;
					cx -= animal.dx * lag;
					cy -= animal.dy * lag;
					createBloodParticles(bloodParticles, (cx + 0.5f) * Level.tw, (cy + 0.5f) * Level.th, 2);

					al_play_sample(animalSounds[animal.type], 0.5f, ((cx+0.5f)/w)*2-1, 1f, ALLEGRO_PLAYMODE.ALLEGRO_PLAYMODE_ONCE, null);
					al_play_sample(splatSound, 1f, ((cx+0.5f)/w)*2-1, 1f, ALLEGRO_PLAYMODE.ALLEGRO_PLAYMODE_ONCE, null);

					//Perhaps this animal wants to explete.
					if (uniform(0, 4) == 0) {
						currentExpletives ~= Expletive(expletives[animal.type][uniform(0,$)], (cx + 0.5f) * Level.tw, (cy + 0.5) * Level.th, time);
					}
				}
				else animals[writePos++] = animal;
			}
			animals.length = writePos;

			//Block the machines into the map so that the animals don't try to go through them.
			//Note: this MUST currently happen after the initial machine move! Otherwise we could erase walls behind the machine in its starting spot!
			blockDownMachines(TileType.Wall);

			//Move all the animals.
			pathfind!(foodType => foodType != FoodType.Beef)(level, pathfindCosts, pathfindQueue);
			moveAnimals(AnimalType.Cow);
			pathfind!(foodType => foodType != FoodType.Lamb)(level, pathfindCosts, pathfindQueue);
			moveAnimals(AnimalType.Sheep);
			pathfind!(foodType => foodType != FoodType.Bacon)(level, pathfindCosts, pathfindQueue);
			moveAnimals(AnimalType.Pig);
			pathfind!(foodType => foodType != FoodType.Duck)(level, pathfindCosts, pathfindQueue);
			moveAnimals(AnimalType.Duck);

			//Remove the machines from the map.
			blockDownMachines(TileType.None);

			//Make sure the animals' render directions are known.
			foreach (ref animal; animals) {
				if (animal.dy<0) animal.direction=0;
				if (animal.dx>0) animal.direction=1;
				if (animal.dy>0) animal.direction=2;
				if (animal.dx<0) animal.direction=3;
			}

			nextTickTime += tickDuration;
		}
	}

	void blockDownMachines(TileType type) {
		foreach (ref machine; machines) {
			void blockDownMachine(int x, int y) {
				this.level.mapWrapped(x, y) = Tile(type);
				if (machine.components[0].length > 0) this.level.mapWrapped(x, y-1) = Tile(type);
				if (machine.components[1].length > 0) this.level.mapWrapped(x+1, y) = Tile(type);
				if (machine.components[2].length > 0) this.level.mapWrapped(x, y+1) = Tile(type);
				if (machine.components[3].length > 0) this.level.mapWrapped(x-1, y) = Tile(type);
			}
			blockDownMachine(machine.x, machine.y);
			//Block down the space behind the machine too.
			final switch (machine.direction) {
				case 0: blockDownMachine(machine.x, machine.y+1); break;
				case 1: blockDownMachine(machine.x-1, machine.y); break;
				case 2: blockDownMachine(machine.x, machine.y-1); break;
				case 3: blockDownMachine(machine.x+1, machine.y); break;
			}
		}
	}

	void moveAnimals(AnimalType type) {
		//We DON'T want to process newly multiplied animals this tick.
		int initialNumAnimals = animals.length;
		foreach (i; 0..initialNumAnimals) {
			Animal* animal = &animals[i];
			if (animal.type == type) {
				animal.ticksLeftTillMove--;
				if (animal.ticksLeftTillMove >= 0) continue;

				auto weatherType = weather[animal.y][animal.x].type;

				animal.startTicksTillMove = (weatherType == ComponentType.Rain ? rainedAnimalMoveTicks : weatherType == ComponentType.Sun ? sunnedAnimalMoveTicks : animalMoveTicks);
				animal.ticksLeftTillMove += animal.startTicksTillMove;

				//If the animal just ate something, it should multiply.
				if (animal.eating.type.isFood) {
					level.map[animal.y][animal.x] = Tile(TileType.None);
					bool tryPropelAnimal(ref Animal animal, int dx, int dy) {
						if (this.level.mapWrapped(animal.x + dx, animal.y + dy).type.animalCanGo(animal.type)) {
							animal.dx = dx; animal.dy = dy;
							animal.x = (animal.x + dx).modUnsigned(w);
							animal.y = (animal.y + dy).modUnsigned(h);
							return true;
						} else {
							animal.dx = animal.dy = 0;
							return false;
						}
					}
					Animal newAnimal = *animal;
					int dx = animal.dx, dy = animal.dy;
					int mul = 1;
					Animal*[] unpropelled;
					//Ideally propel the two animals sideways compared to the old direction.
					if (!tryPropelAnimal(*animal, dy, -dx)) unpropelled ~= animal;
					if (!tryPropelAnimal(newAnimal, -dy, dx)) unpropelled ~= &newAnimal;
					if (unpropelled.length >= 1) {
						//If that isn't possible, propel one forwards and the other backwards.
						//If all else fails, keep them (should only be one of them) still for the amount of time the other will take to move.
						if (!tryPropelAnimal(*unpropelled[0], dx, dy)) tryPropelAnimal(*unpropelled[0], -dx, -dy);
						if (unpropelled.length == 2) tryPropelAnimal(*unpropelled[1], -unpropelled[0].dx, -unpropelled[0].dy);
					}
					animals ~= newAnimal;
					continue;
				}

				if (weatherType == ComponentType.Snow && level.mapWrapped(animal.x+animal.dx, animal.y+animal.dy).type.animalCanGo(animal.type)) {
					//Slip onwards!
				} else {
					pathfindCosts[animal.y][animal.x].chooseDirection(&animal.dx, &animal.dy, &animal.pathfindingBias);
				}
				if (animal.dx | animal.dy) {
					animal.x = (animal.x + animal.dx).modUnsigned(w);
					animal.y = (animal.y + animal.dy).modUnsigned(h);
				} else {
					animal.ticksLeftTillMove = 0;
				}
			}
		}
	}

	void blowComponent(ref Machine machine, int x, int y, ComponentType type, bool* shouldMultiply) {
		final switch (type) {
			case ComponentType.Bomb: blowBomb(x, y); break;
			case ComponentType.TurnChanger: changeTurn(machine); break;
			case ComponentType.Multiply: *shouldMultiply = true; break;
			case ComponentType.Rain, ComponentType.Snow, ComponentType.Sun: blowWeather(type, x, y); break;
		}
	}

	//Catch anything within this shape:
	//   25 26 29 34 41 50
	//16 17 20    25 32 41
	// 9 10 13 18    25 34
	// 4  5  8 13 20    29
	// 1  2  5 10 17    26
	// 0  1  4  9 16    25
	enum r = 4;
	enum r2 = 20;

	void addWrappedDecal(ALLEGRO_BITMAP* sprite, int x, int y, int flags, float scale, float rotate) {
		foreach (wy; -1..2) {
			foreach (wx; -1..2) {
				Level.decals ~= Decal(sprite, x+wx*Level.gameWidth, y+wy*Level.gameHeight, flags, scale, rotate);
			}
		}
	}

	void blowBomb(int x, int y) {
		//Create an explosion effect.
		explosions ~= new Explosion((x+0.5f)*Level.tw, (y+0.5f)*Level.th);

		//Create a burn mark.
		addWrappedDecal(burnMark, (x-r)*Level.tw, (y-r)*Level.th, uniform(0,4), 1, uniform(0,2)*PI/2);

		//Blow up nearby animals!
		foreach (ref animal; animals) {
			int x1 = animal.x.diffWrapped(x, w), x2 = (animal.x - animal.dx).diffWrapped(x, w);
			int y1 = animal.y.diffWrapped(y, h), y2 = (animal.y - animal.dy).diffWrapped(y, h);
			int d1 = x1*x1 + y1*y1, d2 = x2*x2 + y2*y2;
			if (d1 <= r2 || d2 <= r2) animal.dead = true;
		}

		al_play_sample(poofSound, 1f, ((x+0.5f)/w)*2-1, 1f, ALLEGRO_PLAYMODE.ALLEGRO_PLAYMODE_ONCE, null);
	}

	void changeTurn(ref Machine machine) {
		machine.turnAnticlockwise = !machine.turnAnticlockwise;
	}

	void multiply(ref Machine existingMachine) {
		machines ~= existingMachine;
		auto machine = &machines[$-1];
		machine.ticksTillMove = 0;
		machine.turnAnticlockwise = !machine.turnAnticlockwise;
		machine.direction ^= 2;
	}

	void blowWeather(ComponentType type, int cx, int cy) {
		//Create an initial shower of weather particles.
		createBloodParticles(weatherParticles[type], (cx + 0.5f) * Level.tw, (cy + 0.5f) * Level.th, 1);

		//Apply this weather to the relevant tiles.
		foreach (y; -r..r+1) {
			foreach (x; -r..r+1) {
				if (x*x + y*y <= r2) {
					weather[(cy+y).modUnsigned(h)][(cx+x).modUnsigned(w)] = WeatherCell(type, cast(ubyte)cx, cast(ubyte)cy);
				}
			}
		}
	}

	void updateWeatherParticles() {
		foreach (y; 0..h) {
			foreach (x; 0..w) {
				if (weather[y][x].type != ComponentType.init) {
					if (uniform(0f,1f) < deltaTime) {
						createWeatherParticle(weatherParticles[weather[y][x].type], (x + uniform(0f, 1f)) * Level.tw, (y + uniform(0f, 1f)) * Level.th, weather[y][x].type == ComponentType.Sun ? 1f : 0.5f);
					}
				}
			}
		}
	}

	void draw()
	{
		level.draw();
		drawSprites();

		version (debugPathfinding) drawPathfinding();

		drawSetupUi();

		//Draw whatever tile or component the player has picked up.
		ALLEGRO_MOUSE_STATE state;
		getConvertedMouseState(&state);
		if (currentTile.type != TileType.None) Level.drawTile(state.x - Level.tw/2, state.y - Level.th/2, currentTile);
		if (draggingComponent) {
			int direction, index;
			getHoveredMachineUiComponent(state.x, state.y, &direction, &index, true);
			Level.drawComponent(state.x - Level.tw/2, state.y - Level.th/2, currentComponent, direction);
		}
		if (placingMachine) drawMachine(machine, state.x - Level.tw/2, state.y - Level.th/2);
	}

	//Note: only shows the most recently calculated pathfinding map (we calculate several per tick).
	version (debugPathfinding) void drawPathfinding() {
		foreach (y; 0..h) {
			foreach (x; 0..w) {
				auto cost = pathfindCosts[y][x];
				int c = cost.cost;
				auto colour = al_map_rgb(cast(ubyte)(c*4),cast(ubyte)(c*32),255);
				float cx = (x+0.5f)*Level.tw;
				float cy = (y+0.5f)*Level.th;
				enum wf = 4;
				al_draw_line(cx, cy, cx+cost.xWeight*wf, cy+cost.yWeight*wf, colour, 3);
			}
		}
	}

	void drawSprites() {
		//Make sure stuff looks good going off the edges and coming back at the opposite edges.
		auto clip = Clip(0, 0, Level.gameWidth, screenHeight);
		void doWithRepeat(void delegate() action) {
			ALLEGRO_TRANSFORM transform;
			foreach (y; -1..2) {
				foreach (x; -1..2) {
					al_identity_transform(&transform);
					al_translate_transform(&transform, x * Level.gameWidth, y * screenHeight);
					al_use_transform(&transform);
					action();
				}
			}
		}
		if (playing) {
			doWithRepeat(&drawAnimals);
			doWithRepeat(&drawMachines);

			ALLEGRO_TRANSFORM transform;
			al_identity_transform(&transform);
			al_use_transform(&transform);
			drawParticles(bloodParticles, bloodDroplet);

			doWithRepeat({ drawParticles(this.explosions); });
		} else {
			if (machine.x >= 0) doWithRepeat({ drawMachine(this.machine, this.machine.x*Level.tw, this.machine.y*Level.th); });
		}
		ALLEGRO_TRANSFORM transform;
		al_identity_transform(&transform);
		al_use_transform(&transform);
		drawParticles(weatherParticles[ComponentType.Rain], componentSprites[ComponentType.Rain]);
		drawParticles(weatherParticles[ComponentType.Snow], componentSprites[ComponentType.Snow]);
		drawParticles(weatherParticles[ComponentType.Sun ], componentSprites[ComponentType.Sun ]);
		updateAndDrawExpletives();
	}

	enum wobble = PI/24;

	void drawAnimals() {
		foreach (ref animal; animals) {
			float cx = animal.x, cy = animal.y;
			float lag = ((nextTickTime - time) / tickDuration + animal.ticksLeftTillMove) / animal.startTicksTillMove;
			cx -= animal.dx * lag;
			cy -= animal.dy * lag;
			cx *= Level.tw;
			cy *= Level.th;
			float direction = animal.direction;
			if (animal.dx | animal.dy) direction += sin(lag*4*PI)*wobble;
			Level.drawTile(cx, cy, Tile(animal.type.toTileType), direction);
		}
	}

	void drawMachines() {
		foreach (ref machine; machines) {
			float cx = machine.x, cy = machine.y;
			float lag = ((nextTickTime - time) / tickDuration + machine.ticksTillMove) / machineMoveTicks;
			final switch (machine.direction) {
				case 0: cy += lag; break;
				case 1: cx -= lag; break;
				case 2: cy -= lag; break;
				case 3: cx += lag; break;
			}
			cx *= Level.tw;
			cy *= Level.th;
			drawMachine(machine, cx, cy);
		}
	}

	void drawMachine(in Machine machine, float cx, float cy) {
		int cw = al_get_bitmap_width(machineCore);
		int ch = al_get_bitmap_height(machineCore);
		void drawComponents(int direction, int dx, int dy, in ComponentType[] components) {
			if (components.length >= 1) {
				Level.drawComponent(cx + dx*Level.tw, cy + dy*Level.th, components[$-1], direction);
				if (components.length >= 2) Level.drawComponent(cx + dx*Level.tw/2, cy + dy*Level.th/2, components[$-2], direction);
			}
		}
		drawComponents(0, 0, -1, machine.components[0]);
		drawComponents(1, +1, 0, machine.components[1]);
		drawComponents(2, 0, +1, machine.components[2]);
		drawComponents(3, -1, 0, machine.components[3]);
		al_draw_scaled_rotated_bitmap(machineCore, cw/2, ch/2, cx+Level.tw/2, cy+Level.th/2, cast(float)Level.tw/cw, cast(float)Level.th/ch, machine.direction * PI / 2, machine.turnAnticlockwise ? ALLEGRO_FLIP_HORIZONTAL : 0);
	}

	enum margin = 16;
	enum uiLeft = Level.gameWidth + margin;
	enum uiRight = screenWidth;
	enum xTileSpacing = 160;
	enum yTileSpacing = Level.th + margin;
	enum lineSpacing = MainMenu.lineSpacing;
	enum machineUiTop = screenHeight - (uiRight-uiLeft);

	void drawSetupUi() {
		al_draw_filled_rectangle(Level.gameWidth, 0, screenWidth, screenHeight, black);
		drawTileUi();
		if (playing) drawPoetryUi(); else drawMachineUi();

		enum bottom = screenHeight - lineSpacing;
		enum xCentre1 = uiLeft+(uiRight-uiLeft)/3;
		enum xCentre2 = xCentre1+(uiRight-uiLeft)/3;
		if (levelNum > 0) clickableTextCentre(uiLeft, bottom, xCentre1, "Prev Level", { levelNum--; initNewLevel(); });
		if (levelNum < allLevelNames.length - 1) clickableTextCentre(xCentre1, bottom, xCentre2, "Next Level", { levelNum++; initNewLevel(); });
		clickableTextCentre(xCentre2, bottom, uiRight, theEditor ? "End Test" : "Main Menu", &exitGame);
		//clickableTextCentre(uiRight - 280, screenHeight - lineSpacing, uiRight, theEditor ? "End Test" : "Main Menu", &exitGame);
	}

	void exitGame() {
		delete theGame;
		theGame = null;
	}

	void drawTileUi()
	{
		enum top = 0, bottom = machineUiTop;

		al_draw_filled_rectangle(uiLeft, top, uiRight, bottom, black);

		al_draw_text(theFont, white, (uiLeft+uiRight)/2, margin - 6, ALLEGRO_ALIGN_CENTRE, allLevelNames[levelNum].toStringz());

		enum xMiddle = uiRight - xTileSpacing*2 + 30;

		if (playing) {
			drawKillsUi(AnimalType.Cow  , uiLeft, top+margin+yTileSpacing*1);
			drawKillsUi(AnimalType.Sheep, uiLeft, top+margin+yTileSpacing*2);
			drawKillsUi(AnimalType.Pig  , uiLeft, top+margin+yTileSpacing*3);
			drawKillsUi(AnimalType.Duck , uiLeft, top+margin+yTileSpacing*4);

			clickableTextLeft(xMiddle, top+margin+yTileSpacing*5-lineSpacing-6, uiRight, "Stop", &startSetup);
		} else {
			drawPlaceableTileUi(Tile(AnimalType.Cow  .toTileType), uiLeft,            top+margin+yTileSpacing*1);
			drawPlaceableTileUi(Tile(AnimalType.Sheep.toTileType), uiLeft,            top+margin+yTileSpacing*2);
			drawPlaceableTileUi(Tile(AnimalType.Pig  .toTileType), uiLeft,            top+margin+yTileSpacing*3);
			drawPlaceableTileUi(Tile(AnimalType.Duck .toTileType), uiLeft,            top+margin+yTileSpacing*4);
			drawPlaceableTileUi(Tile(FoodType.Beef .toTileType), uiLeft+xTileSpacing, top+margin+yTileSpacing*1);
			drawPlaceableTileUi(Tile(FoodType.Lamb .toTileType), uiLeft+xTileSpacing, top+margin+yTileSpacing*2);
			drawPlaceableTileUi(Tile(FoodType.Bacon.toTileType), uiLeft+xTileSpacing, top+margin+yTileSpacing*3);
			drawPlaceableTileUi(Tile(FoodType.Duck .toTileType), uiLeft+xTileSpacing, top+margin+yTileSpacing*4);

			drawComponentUi(ComponentType.Bomb             , xMiddle, top+margin+yTileSpacing*1);
			drawComponentUi(ComponentType.TurnChanger      , xMiddle, top+margin+yTileSpacing*2);
			drawComponentUi(ComponentType.Multiply         , xMiddle, top+margin+yTileSpacing*3);
			drawComponentUi(ComponentType.Rain, xMiddle+xTileSpacing, top+margin+yTileSpacing*1);
			drawComponentUi(ComponentType.Snow, xMiddle+xTileSpacing, top+margin+yTileSpacing*2);
			drawComponentUi(ComponentType.Sun , xMiddle+xTileSpacing, top+margin+yTileSpacing*3);

			if (machine.x == -1 && !placingMachine) drawPlaceableMachineUi(xMiddle-150, top+margin+yTileSpacing*2);

			if (machine.x >= 0) clickableTextLeft(xMiddle, top+margin+yTileSpacing*5-lineSpacing-6, uiRight, "Go!", &startPlay);
		}
	}

	void drawKillsUi(AnimalType type, int x, int y)
	{
		enum tw = Level.tw, th = Level.th;
		Level.drawTile(x, y, Tile(type.toTileType), sin((time-playStartTime)*4*PI)*wobble);
		int textWidth = al_get_text_width(theFont, "000");
		al_draw_textf(theFont, green, x+tw+2+textWidth, y-8, ALLEGRO_ALIGN_RIGHT, "%d", numKills[type]);
	}

	void drawPlaceableTileUi(Tile tile, int x, int y)
	{
		enum tw = Level.tw, th = Level.th;
		Level.drawTile(x, y, tile);
		int textWidth = al_get_text_width(theFont, "000");
		if (level.placeables[tile.type] > 0) {
			//Click to start/stop dragging.
			registerButton(x, y, x+tw+2+textWidth, y+th, {
				this.currentTile = (this.currentTile.type == tile.type ? Tile(TileType.None) : tile);
				this.draggingComponent = false;
				this.placingMachine = false;
			});
		}
		al_draw_textf(theFont, green, x+tw+2+textWidth, y-8, ALLEGRO_ALIGN_RIGHT, "%d", level.placeables[tile.type]);
	}

	void drawComponentUi(ComponentType type, int x, int y)
	{
		enum tw = Level.tw, th = Level.th;
		Level.drawComponent(x, y, type, 0);
		int textWidth = al_get_text_width(theFont, "000");
		if (level.components[type] > 0) {
			//Click to start/stop dragging.
			registerButton(x, y, x+tw+2+textWidth, y+th, {
				this.currentTile = Tile(TileType.None);
				this.draggingComponent = (!this.draggingComponent || this.currentComponent != type);
				this.currentComponent = type;
				this.placingMachine = false;
			});
		}
		al_draw_textf(theFont, green, x+tw+2+textWidth, y-8, ALLEGRO_ALIGN_RIGHT, "%d", level.components[type]);
	}

	void drawPlaceableMachineUi(int x, int y)
	{
		enum tw = Level.tw, th = Level.th;
		drawMachine(machine, x, y);
		//Click to start/stop dragging.
		registerButton(x-tw, y-tw, x+tw*2, y+th*2, {
			this.currentTile = Tile(TileType.None);
			this.draggingComponent = false;
			this.placingMachine = !this.placingMachine;
		});
	}

	void drawMachineUi() {
		enum top = machineUiTop, bottom = screenHeight;

		int cw = al_get_bitmap_width(machineCore);
		int ch = al_get_bitmap_height(machineCore);
		enum cx = (uiLeft+uiRight)/2;
		enum cy = (top+bottom)/2;
		float space = cy - ch/2 - top;

		//Draw the machine components...
		ALLEGRO_TRANSFORM transform;
		foreach (d; 0..4) {
			al_build_transform(&transform, cx, cy, 1, 1, d * PI / 2);
			al_use_transform(&transform);
			float spacing = Level.th+4;
			if (spacing * machine.components[d].length > space) spacing = space / machine.components[d].length;
			foreach_reverse (i, component; machine.components[d]) {
				Level.drawComponent(-Level.tw/2, -ch/2 - spacing*(i+1), component, 0);
			}
		}
		al_identity_transform(&transform);
		al_use_transform(&transform);

		//...and the machine core.
		bool hover = (!playing && registerButton(cx-cw/2, cy-ch/2, cx+cw/2, cy+ch/2, {
			this.machine.direction = (this.machine.direction + 1) & 3;
			if (this.machine.direction == 0) this.machine.turnAnticlockwise = !this.machine.turnAnticlockwise;
		}));
		//Wish: can we brighten it somehow when hover is true?
		al_draw_rotated_bitmap(machineCore, cw/2, ch/2, cx, cy, machine.direction * PI / 2, machine.turnAnticlockwise ? ALLEGRO_FLIP_HORIZONTAL : 0);
	}

	void updateAndDrawExpletives() {
		enum expletiveTime = 1f;
		int writePos = 0;
		foreach (readPos, ref expletive; currentExpletives) {
			float t = time - expletive.startTime;
			if (t >= expletiveTime) continue;
			float y = expletive.y - lineSpacing + lineSpacing*2*expm1(-t * 4 / expletiveTime);
			al_draw_text(theFont, white, expletive.x, y, ALLEGRO_ALIGN_CENTRE, expletive.text);
			currentExpletives[writePos++] = expletive;
		}
		currentExpletives.length = writePos;
	}

	void drawPoetryUi() {
		enum top = machineUiTop + lineSpacing;

		enum timePerLine = 2f;
		float progress = (time - playStartTime) / timePerLine;
		int currentLine = cast(int)progress;
		if (currentLine > 8) currentLine = 8;
		foreach (i; 0..currentLine) {
			al_draw_text(theFont, green, uiLeft, top+i*lineSpacing, ALLEGRO_ALIGN_LEFT, poems[levelNum%$][i].toStringz());
		}
		if (currentLine < 8) {
			alias i = currentLine;
			al_draw_text(theFont, green, uiLeft, top+i*lineSpacing, ALLEGRO_ALIGN_LEFT, poems[levelNum%$][i][0..cast(int)($*(progress-currentLine))].toStringz());
		}
	}

	//index is set precisely if the mouse is precisely over a component; or length if it's in the general ballpark of that direction but maybe off to the side.
	//We return false if nowhere near. So on a true return, index is always good for insertion, and is good for deletion only if < length.
	bool getHoveredMachineUiComponent(int x, int y, int* direction, int* index, bool inPlaceToo) {
		if (x >= uiLeft && x < uiRight && y >= machineUiTop && y < screenHeight) {
			//Dropped on the machine UI
			int ch = al_get_bitmap_height(machineCore);
			enum cx = (uiLeft+uiRight)/2, cy = (machineUiTop+screenHeight)/2;
			float space = cy - ch/2 - machineUiTop;
			int dx = x - cx, dy = y - cy;
			*direction = (dy<dx && dy<-dx ? 0 : dy>dx && dy>-dx ? 2 : dx>0 ? 1 : 3);
			float spacing = Level.th+4;
			if (spacing * machine.components[*direction].length > space) spacing = space / machine.components[*direction].length;
			int getIndexNorth(int dx, int dy) { return dx >= -Level.tw/2 && dx < Level.tw/2 ? cast(int)floor((-ch/2 - 2 - dy) / spacing) : int.max; }
			final switch (*direction) {
				case 0: *index = getIndexNorth(dx, +dy); break;
				case 1: *index = getIndexNorth(dy, -dx); break;
				case 2: *index = getIndexNorth(dx, -dy); break;
				case 3: *index = getIndexNorth(dy, +dx); break;
			}
			if (*index < 0) return false;
			int length = machine.components[*direction].length;
			if (*index > length) *index = length;
			return true;
		} else if (inPlaceToo && machine.x >= 0 && x >= 0 && x <= Level.gameWidth && y >= 0 && y < screenHeight) {
			//Dropped on the machine in place
			int cx = machine.x*Level.tw + Level.tw/2;
			int cy = machine.y*Level.th + Level.th/2;
			int dx = x - cx, dy = y - cy;
			*direction = (dy<dx && dy<-dx ? 0 : dy>dx && dy>-dx ? 2 : dx>0 ? 1 : 3);
			*index = machine.components[*direction].length;
			return true;
		} else {
			*direction = 0; *index = -1;
			return false;
		}
	}
}

struct LevelSetup
{
	Level level;

	//The tile or component you're currently holding for placement.
	Tile currentTile;
	ComponentType currentComponent;
	bool draggingComponent;
	bool placingMachine;

	//Where you've placed your own things, so we know what you're allowed to remove again.
	bool[Level.w][Level.h] placement;

	//Your machine's initial configuration.
	Machine machine;

	//This is a struct so it can be easily completely reinitialised.
	LevelPlay levelPlay;
	alias levelPlay this;
}

struct LevelPlay
{
	Level initialLevel;

	double playStartTime;
	double nextTickTime;

	//This is an array so that machines can split off during the play phase.
	Machine[] machines;

	Animal[] animals;

	int[AnimalType.max+1] numKills;

	WeatherCell[Level.w][Level.h] weather;

	Explosion[] explosions;
	BloodParticle[] bloodParticles;
	BloodParticle[][ComponentType.max+1] weatherParticles;

	Expletive[] currentExpletives;
}

struct Expletive {
	const(char)* text;
	float x, y;
	double startTime;
}

struct Machine
{
	//Four dynamic arrays (one per direction) where components can be added to the machine.
	ComponentType[][4] components;

	//The machine's current direction (bearing / 90).
	int direction;

	//Which way will it turn when it hits something?
	bool turnAnticlockwise;

	int x=-1, y=-1;

	int ticksTillMove;
}

struct Animal
{
	AnimalType type;
	int x, y;
	int dx, dy;		//Used for moving
	int direction;	//Used for drawing
	int pathfindingBias;
	int startTicksTillMove = 1;
	int ticksLeftTillMove;
	bool dead;
	Tile eating;
}

struct WeatherCell
{
	ComponentType type;

	//The centre position where the weather area was triggered from.
	//Helps us draw things nicely.
	ubyte cx, cy;
}
