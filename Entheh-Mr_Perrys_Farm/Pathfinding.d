module Pathfinding;
import LevelModule;
import Maths;
import std.math;

/*
We do a floodfill from the various food items.
As the fill hits an animal, that animal moves towards the source of the fill.

Since different animal types have preferences about what food types they want,
we have to do this separately for each animal type, seeding from the various compatible foods for that animal type.
However, we don't have to do it separately per animal or per placed food (which could be too costly for pathological levels).
*/

private enum w = Level.w, h = Level.h;

struct CostCell {
	ubyte cost = 255;

	//The weights are used to try and get animals to approximate a straight line in open spaces.
	byte xWeight;
	byte yWeight;

	void chooseDirection(int* dx, int* dy, int* bias) {
		*dx = *dy = 0;
		if (!xWeight || !yWeight) *bias = 0;
		if (xWeight | yWeight) {
			if (abs(xWeight) - abs(yWeight) >= *bias) {
				*dx = -sgn(xWeight);
				*bias += abs(yWeight);
			} else {
				*dy = -sgn(yWeight);
				*bias -= abs(xWeight);
			}
		}
	}
}

struct QueueEntry {
	ubyte x, y;
}

void pathfind(bool delegate(FoodType) foodFilter)(ref Level level, ref CostCell[w][h] costs, ref QueueEntry[w*h] queue)
{
	costs = costs.init;
	int queueStart = 0, queueEnd = 0;

	//Seed the queue from the various food items.
	foreach (ubyte y; 0..h) {
		foreach (ubyte x; 0..w) {
			if (level.map[y][x].type.isFood && foodFilter(level.map[y][x].type.toFoodType)) {
				costs[y][x].cost = 0;
				queue[queueEnd++] = QueueEntry(x, y);
			}
		}
	}

	//Fill outwards from the seeds.
	while (queueStart < queue.length) {
		with (queue[queueStart++]) {
			auto cost = costs[y][x];
			void trySpread(int dx, int dy) {
				ubyte x2 = cast(ubyte)(x+dx).modUnsigned(w);
				ubyte y2 = cast(ubyte)(y+dy).modUnsigned(h);
				if (level.map[y2][x2].type == TileType.None) {
					int newCost = cost.cost + 1;
					if (newCost <= costs[y2][x2].cost) {
						if (newCost < costs[y2][x2].cost) queue[queueEnd++] = QueueEntry(x2, y2);
						costs[y2][x2].cost = cast(ubyte)newCost;
						void increase(ref byte weight, int newWeight, int d) {
							newWeight = (abs(newWeight) + 1) * d;
							if (abs(newWeight) > abs(weight)) weight = cast(byte)newWeight;
						}
						increase(costs[y2][x2].xWeight, cost.xWeight, dx);
						increase(costs[y2][x2].yWeight, cost.yWeight, dy);
					}
				}
			}
			trySpread(-1, 0);
			trySpread(+1, 0);
			trySpread(0, -1);
			trySpread(0, +1);
		}
	}
}
