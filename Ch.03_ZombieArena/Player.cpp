#include "Player.h"
#include "TextureHolder.h"

#include <cmath>

#include "assets.h"

#define _Player_INVENCILITY_TIME 200 // ms
#define _Player_HIT_DAMAGE 10 // Life taken from an extern hit
#define _DEFAULT_SPEED_BOOST .2f // percent
#define _DEFAULT_HEALTH_BOOST .2f // percent

Player::Player() {
	m_Speed = START_SPEED;
	m_MaxHealht = m_Health = START_HEALTH;

	// Associate a texture with the sprite
	// WATCH this space
	m_Sprite = Sprite(TextureHolder::GetTexture(PLAYER_PNG));

	// Set the oring of the sprite to the center for smooth rotation
	m_Sprite.setOrigin(PLAYER_CENTER_X, PLAYER_CENTER_Y);
}

void Player::spawn(IntRect arena, Vector2f resolution, int tileSize) {
	// Place the player in the middle of the arena
	m_Position.x = arena.width / 2.0f;
	m_Position.y = arena.height / 2.0f;

	// Copy the details of the arena to the player's m_Arena
	m_Arena.left = arena.left;
	m_Arena.width = arena.width;
	m_Arena.top = arena.top;
	m_Arena.height = arena.height;

	// Remember how big the tiles are in this arena
	m_TileSize = tileSize;

	// Store the resolution for future use
	m_Resolution.x = resolution.x;
	m_Resolution.y = resolution.y;
}

void Player::resetPlayerStats() {
	m_Speed = START_SPEED;
	m_Health = START_HEALTH;
	m_MaxHealht = START_HEALTH;
}

Time Player::getLastHitTime() {
	return m_LastHit;
}

bool Player::hit(Time timeHit) {
	if (timeHit.asMilliseconds() - m_LastHit.asMilliseconds() > _Player_INVENCILITY_TIME) {
		m_LastHit = timeHit;
		m_Health -= _Player_HIT_DAMAGE;
		return true;
	} else
		return false;
}

FloatRect Player::getPosition() {
	return m_Sprite.getGlobalBounds();
}

Vector2f Player::getCenter() {
	return m_Position;
}

float Player::getRotation() {
	return m_Sprite.getRotation();
}

Sprite Player::getSprite() {
	return m_Sprite;
}

int Player::getHealth() {
	return m_Health;
}

void Player::move(Vector2f direction) {
	directionAxis = direction;
}

void Player::upgradeSpeed() {
	// default% speed upgrade
	m_Speed += (START_SPEED * _DEFAULT_SPEED_BOOST);
}

void Player::upgradeHealth() {
	// default% max health upgrade
	m_MaxHealht += (START_HEALTH * _DEFAULT_HEALTH_BOOST);
}

void Player::increaseHealthLvl(int amount) {
	m_Health += amount;

	// But not beyond the maximum
	if (m_Health > m_MaxHealht)
		m_Health = m_MaxHealht;
}

void Player::update(float elapsedTime, Vector2i mousePosition) {
	m_Position += directionAxis * m_Speed * elapsedTime;

	m_Sprite.setPosition(m_Position);

	// Keep (clamp) the player in the arena
	clamp(m_Position.x, m_Arena.left + m_TileSize, m_Arena.width - m_TileSize);
	clamp(m_Position.y, m_Arena.top + m_TileSize, m_Arena.height - m_TileSize);

	// clang-format off
	// Calculate the angle the player is facing, and apply it to rotation
	float angle = atan2(mousePosition.y - m_Resolution.y / 2,
			mousePosition.x - m_Resolution.x / 2) * 180 / PI;
	// clang-format on
	m_Sprite.setRotation(angle);
}
