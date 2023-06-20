#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Player
{
private:
	const float START_SPEED = 200;
	const float START_HEALTH = 100;

	Vector2f m_Position; // Where is the player?
	Sprite m_Sprite;     // Of course, we will need a sprite

	// WATCH this space -> Interestin changes here soon!!
	Texture m_Texture; // And a texture too

	Vector2f m_Resolution; // What is the screen resolution
	IntRect m_Arena;       // What size is the current arena
	int m_TileSize;        // How big is each tile of the arena

	// Which direction(s) is the player currently moving in
	// (1, 0) = RIGHT, (-1, 0) = LEFT, (0, 1) = DOWN, (0, -1) = UP, (0, 0) = NONE
	Vector2f directionAxis;

	int m_Health;    // How much health has the player got?
	int m_MaxHealht; // What is the maximum health the player can have
	Time m_LastHit;  // When was the player last hit

	float m_Speed; // Speed in pixels per second

public:
	Player();

	void spawn(IntRect areana, Vector2f resolution, int tileSize);

	// Call this at the end of every game
	void resetPlayerStats();

	// Handle the player getting hit by a zombie
	bool hit(Time timeHit);

	// How long ago was the player last hit
	Time getLastHitTime();

	// Where is the player
	FloatRect getPosition();

	// Where is the center of the player
	Vector2f getCenter();

	// What angle is the player facing
	float getRotation();

	// Send a copy of the sprite to the main function
	Sprite getSprite();

	// Move or stop the player moving in a specific direction
	void move(Vector2f direction);

	// We will call this function once every frame
	void update(float elapsedTime, Vector2i mousePosition);

	// Give the player a speed boost
	void upgradeSpeed();

	// Give the player some health
	void upgradeHealth();

	// Increase the maximum amount of healht th player can have
	void increaseHealthLvl(int amount);

	// How much health has the player currently got?
	int getHealth();
};
