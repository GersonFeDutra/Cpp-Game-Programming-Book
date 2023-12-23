#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Zombie {
private:
	// How fast is each zombie type?
	const float BLOATER_SPEED = 40.0f;
	const float CHASER_SPEED = 80.0f;
	const float CRAWLER_SPEED = 20.0f;

	// How tough is each zombie type
	const float BLOATER_HEALTH = 5;
	const float CHASER_HEALTH = 1;
	const float CRAWLER_HEALTH = 3;

	// Make each zombie vary its speed slightly
	const int MAX_VARIANCE = 30;
	const int OFFSET = 101 - MAX_VARIANCE;

	// Where is this zombie?
	Vector2f m_Position;

	// A sprite for this zombie
	Sprite m_Sprite;

	// How fast can this one run/crawl?
	float m_Speed;

	// How much health has it got?
	float m_Health;

	// Is it still alive?
	bool m_Alive;

	// Public prototype go here
public:
	// Handle when a bullet hits a zombie
	bool hit();
	// Find out if the zombie is alive
	bool isAlive();

	enum class Type { BLOATER = 0,
		CHASER = 1,
		CRAWLER = 2 };
	// Spawn a new zombie
	void spawn(float startX, float startY, Type type, int seed);

	// Return a rectangle that is the position in the world
	FloatRect getPosition();

	// Get a copy of the sprite to draw
	Sprite getSprite();

	// Update the zombie each frame
	void update(float elapsedTime, Vector2f playerLocation);
};
