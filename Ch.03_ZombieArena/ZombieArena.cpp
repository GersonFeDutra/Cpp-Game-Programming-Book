#include "ZombieArena.h"
#include "Zombie.h"

#define _OFFSET 20

Zombie *createHorde(int numZombies, IntRect arena) {
	Zombie *zombies = new Zombie[numZombies];

	int maxY = arena.height - _OFFSET;
	int minY = arena.top + _OFFSET;
	int maxX = arena.width - 20;
	int minX = arena.left + 20;

	enum {
		LEFT,
		RIGHT,
		TOP,
		BOTTOM,
	};

	for (int i = 0; i < numZombies; i++) {
		// Which side should the zombie spawn
		srand((int)time(0) * i);
		int side = (rand() % 4);

		float x, y;
		switch (side) {
			case LEFT: {
				x = minX;
				y = (rand() % maxY) + minY;
			} break;
			case RIGHT: {
				x = maxX;
				y = (rand() % maxY) + minY;
			} break;
			case TOP: {
				x = (rand() % maxX) + minX;
				y = minY;
			} break;
			case BOTTOM: {
				x = (rand() % maxX) + minX;
				y = maxY;
			} break;
			default:
				break;
		}

		// Bloater, Crawler or runner
		srand((int)time(0) * i * 2);
		int type = (rand() % 3);

		// Spawn the new zombie into the array
		zombies[i].spawn(x, y, static_cast<Zombie::Type>(type), i);
	}

	return zombies;
}
