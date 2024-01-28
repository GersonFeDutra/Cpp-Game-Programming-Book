#include "LevelManager.h"
#include "TextureHolder.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>

using namespace sf;
using namespace std;

#define LVL_TXT(N) "levels/level" #N ".txt"
#define max(A, B) ((A) > (B) ? (A) : (B))

bool print_once = true;

int *LevelManager::nextLevel(VertexArray &rVaLevel) {
	m_LevelSize.x = 0;
	m_LevelSize.y = 0;

	// Get the next level
	m_CurrentLevel++;

	if (m_CurrentLevel > NUM_LVLS) {
		m_CurrentLevel = 1;
		m_TimeModifier -= .1f;
	}

	// Load the appropriate level from a text file
	string levelToLoad;
	switch (m_CurrentLevel) {
		case 1: {
			levelToLoad = LVL_TXT(1);
			m_StartPosition.x = 100;
			m_StartPosition.y = 100;
			m_BaseTimeLimit = 30.0f;
		} break;
		case 2: {
			levelToLoad = LVL_TXT(2);
			m_StartPosition.x = 100;
			m_StartPosition.y = 3600;
			m_BaseTimeLimit = 100.0f;
		} break;
		case 3: {
			levelToLoad = LVL_TXT(3);
			m_StartPosition.x = 1250;
			m_StartPosition.y = 0;
			m_BaseTimeLimit = 30.0f;
		} break;
		case 4: {
			levelToLoad = LVL_TXT(4);
			m_StartPosition.x = 50;
			m_StartPosition.y = 200;
			m_BaseTimeLimit = 50.0f;
		} break;
	} // End switch

	ifstream inputFile(levelToLoad);
	string s;

	// Count the number of rows in the file
	while (getline(inputFile, s)) {
		++m_LevelSize.y;
		// Store the length of the rows
		m_LevelSize.x = max(s.length(), m_LevelSize.x);
	}

	// Go back to the start of the file
	inputFile.clear();
	inputFile.seekg(0, ios::beg);

	int *arrayLvl = new int[m_LevelSize.y * m_LevelSize.x];

	// Loop through the file and store all
	// the values in the 2D array
	string row;
	for (int y = 0; inputFile >> row; y++)
		for (int x = 0; x < row.length(); x++) {
			const char val = row[x];
			*(arrayLvl + (x * m_LevelSize.y + y)) = atoi(&val);
		}

	// Close the file
	inputFile.close();

	// What type of primitive are we using?
	rVaLevel.setPrimitiveType(sf::Quads);

	// Set the size of the vertex array
	rVaLevel.resize(m_LevelSize.x * m_LevelSize.y * VERTS_IN_QUAD);

	// Start at the beginning of the VertexArray
	int currentVertex = 0;

	for (int x = 0; x < m_LevelSize.x; x++)
		for (int y = 0; y < m_LevelSize.y; y++) {
			// Position each vertex in the current quad
			rVaLevel[currentVertex + 0].position = Vector2f(x * TILE_SIZE, y * TILE_SIZE);
			rVaLevel[currentVertex + 1].position = Vector2f(x * TILE_SIZE + TILE_SIZE, y * TILE_SIZE);
			rVaLevel[currentVertex + 2].position = Vector2f(x * TILE_SIZE + TILE_SIZE, y * TILE_SIZE + TILE_SIZE);
			rVaLevel[currentVertex + 3].position = Vector2f(x * TILE_SIZE, y * TILE_SIZE + TILE_SIZE);

			// Which tile from the sprite sheet should we use
			int verticalOffset = getTile(arrayLvl, x, y) * TILE_SIZE;

			rVaLevel[currentVertex + 0].texCoords = Vector2f(0, 0 + verticalOffset);
			rVaLevel[currentVertex + 1].texCoords = Vector2f(TILE_SIZE, 0 + verticalOffset);
			rVaLevel[currentVertex + 2].texCoords = Vector2f(TILE_SIZE, TILE_SIZE + verticalOffset);
			rVaLevel[currentVertex + 3].texCoords = Vector2f(0, TILE_SIZE + verticalOffset);

			// Position ready for the next four vertices
			currentVertex = currentVertex + VERTS_IN_QUAD;
		}

	return arrayLvl;
} // End of nextLevel function

Vector2i LevelManager::getLevelSize() {
	return m_LevelSize;
}

int LevelManager::getCurrentLevel() {
	return m_CurrentLevel;
}

float LevelManager::getTimeLimit() {
	return m_BaseTimeLimit * m_TimeModifier;
}

Vector2f LevelManager::getStartPosition() {
	return m_StartPosition;
}

int LevelManager::getTile(const int *data, unsigned x, unsigned y) {
	return *(data + (x * m_LevelSize.y + y));
}
