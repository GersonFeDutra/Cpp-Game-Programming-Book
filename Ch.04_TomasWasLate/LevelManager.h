#pragma once

#include <SFML/Graphics.hpp>
using namespace sf;
using namespace std;

class LevelManager {
private:
	const int NUM_LVLS = 4;

	Vector2i m_LevelSize;
	Vector2f m_StartPosition;
	float m_TimeModifier = 1;
	float m_BaseTimeLimit = 0;
	int m_CurrentLevel = 0;

public:
	const int TILE_SIZE = 50;
	const int VERTS_IN_QUAD = 4;

	float getTimeLimit();

	Vector2f getStartPosition();

	int *nextLevel(VertexArray &rVaLevel);

	int getTile(const int *, unsigned x, unsigned y);

	Vector2i getLevelSize();

	int getCurrentLevel();
};
