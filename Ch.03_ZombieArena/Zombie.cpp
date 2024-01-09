#include "Zombie.h"
#include "TextureHolder.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

#define DIR_GRAPHICS "graphics/"
#define IMG_BLOATER DIR_GRAPHICS "bloater.png"
#define IMG_CHASER DIR_GRAPHICS "chaser.png"
#define IMG_CRAWLER DIR_GRAPHICS "chaser.png"
#define IMG_BLOOD DIR_GRAPHICS "blood.png"
#define _ORIGIN 25
#define PI 3.141

// TODO -> Usar padrão Factory e polimorfismo com serialização em Json
void Zombie::spawn(float startX, float startY, Zombie::Type type, int seed) {
	switch (type) {
		case Zombie::Type::BLOATER: {
			m_Sprite = Sprite(TextureHolder::GetTexture(IMG_BLOATER));
			m_Speed = BLOATER_SPEED;
			m_Health = BLOATER_HEALTH;
		} break;
		case Zombie::Type::CHASER: {
			m_Sprite = Sprite(TextureHolder::GetTexture(IMG_CHASER));
			m_Speed = CHASER_SPEED;
			m_Health = CHASER_HEALTH;
		} break;
		case Zombie::Type::CRAWLER: {
			m_Sprite = Sprite(TextureHolder::GetTexture(IMG_CRAWLER));
			m_Speed = CRAWLER_SPEED;
			m_Health = CRAWLER_HEALTH;
		}; break;
	}
	// Modify the speed to make the zombie unique
	// Every zombie is unique. Create a speed modifier
	srand((int)time(0) * seed);

	// Somewhere between 80 and 100
	float modifier = (rand() % MAX_VARIANCE) + OFFSET;

	// Express this as a fraction of 1
	modifier /= 100; // Now equals between .7 and 1
	m_Speed *= modifier;

	m_Position.x = startX;
	m_Position.y = startY;

	// Initialize its location
	m_Sprite.setOrigin(_ORIGIN, _ORIGIN);

	// Set its position
	m_Sprite.setPosition(m_Position);
}

bool Zombie::hit() {
	m_Health--;

	if (m_Health < 0) {
		// dead
		m_Alive = false;
		m_Sprite.setTexture(TextureHolder::GetTexture(IMG_BLOOD));
		return true;
	}

	// injured but not dead
	return false;
}

bool Zombie::isAlive() {
	return m_Alive;
}

FloatRect Zombie::getPosition() {
	return m_Sprite.getGlobalBounds();
}

Sprite Zombie::getSprite() {
	return m_Sprite;
}

// TODO -> Fazer um lerp
void Zombie::update(float elapsedTime, Vector2f playerLocation) {
	float playerX = playerLocation.x;
	float playerY = playerLocation.y;

	// Update the zombie position variables
	if (playerX > m_Position.x)
		m_Position.x = m_Position.x + m_Speed * elapsedTime;
	else if (playerX < m_Position.x)
		m_Position.x = m_Position.x - m_Speed * elapsedTime;
	if (playerY > m_Position.y)
		m_Position.y = m_Position.y + m_Speed * elapsedTime;
	else if (playerY < m_Position.y)
		m_Position.y = m_Position.y - m_Speed * elapsedTime;

	// Move the sprite
	m_Sprite.setPosition(m_Position);

	// Face the sprite in the correct direction
	float angle = (std::atan2(playerY - m_Position.y, playerX - m_Position.x) * 180 / PI);

	m_Sprite.setRotation(angle);
}
