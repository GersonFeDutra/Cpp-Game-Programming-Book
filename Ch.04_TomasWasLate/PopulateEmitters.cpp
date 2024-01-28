#include "Engine.h"

using namespace sf;
using namespace std;

#define PADDING 6

void Engine::populateEmitters(vector<Vector2f> &vSoundEmitters, int *arrayLevel) {
	// Make sure the vector is empty
	bool e = vSoundEmitters.empty();

	// Keep track of the previous emitter
	// so we don't make too many
	FloatRect previousEmitter;

	// Search for fire in the level
	for (int x = 0; x < (int)m_LM.getLevelSize().x; x++)
		for (int y = 0; y < (int)m_LM.getLevelSize().y; y++)
			if (m_LM.getTile(arrayLevel, x, y) == 2) // fire is present
				// Skip over any fire tiles too
				// near a previous emitter
				if (!FloatRect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE).intersects(previousEmitter)) {
					// Add the coordinates of this water block
					vSoundEmitters.push_back(Vector2f(x * TILE_SIZE, y * TILE_SIZE));

					// Make a rectangle P x P blocks
					// so we don't make any more emitters
					// too close to this one
					previousEmitter.left = x * TILE_SIZE;
					previousEmitter.top = y * TILE_SIZE;
					previousEmitter.width = TILE_SIZE * PADDING;
					previousEmitter.height = TILE_SIZE * PADDING;
				}

	return;
}
