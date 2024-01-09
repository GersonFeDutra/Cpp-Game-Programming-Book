#include "Pickup.h"
#include "TextureHolder.h"

#define _IMGS "graphics/"
#define _AMMO_PNG _IMGS "ammo_pickup.png"
#define _HEALTH_PNG _IMGS "health_pickup.png"
#define _CENTER 25
#define _OFFSET 50
#define _UPGRADE_FACTOR 0.5
#define _RESPAWN_BOOST 10.0

Pickup::Pickup(Pickup::Type type) {
	// Store the type of this pickup
	m_Type = type;

	// Associate the texture with the sprite
	switch (type) {
		case Type::AMMO: {
			m_Sprite = Sprite(TextureHolder::GetTexture(_AMMO_PNG));
			// How much is pickup worth
			m_Value = AMMO_START_VALUE;
		} break;
		case Type::HEALTH: {
			m_Sprite = Sprite(TextureHolder::GetTexture(_HEALTH_PNG));

			// How much is pickup worth
			m_Value = HEALTH_START_VALUE;
		}
		default:
			break;
	}

	m_Sprite.setOrigin(_CENTER, _CENTER);

	m_SecondsToLive = START_SECONDS_TO_LIVE;
	m_SecondsToWait = START_WAIT_TIME;
}

void Pickup::setArena(IntRect arena) {
	// Copy the details of the arena to the pickup's m_Arena
	m_Arena.left = arena.left + _OFFSET;
	m_Arena.width = arena.width - _OFFSET;
	m_Arena.top = arena.top + _OFFSET;
	m_Arena.height = arena.height - _OFFSET;

	spawn();
}

void Pickup::spawn() {
	// Spawn at a random location
	srand((int)time(0) / (static_cast<int>(m_Type) + 1));

	int x = rand() % m_Arena.width;
	srand((int)time(0) * (static_cast<int>(m_Type) + 1));

	int y = rand() % m_Arena.height;
	m_SecondsSinceSpawn = 0;
	m_Spawned = true;

	m_Sprite.setPosition(x, y);
}

FloatRect Pickup::getPosition() {
	return m_Sprite.getGlobalBounds();
}

Sprite Pickup::getSprite() {
	return m_Sprite;
}

bool Pickup::isSpawned() {
	return m_Spawned;
}

int Pickup::gotIt() {
	m_Spawned = false;
	m_SecondsSinceDeSpawn = 0;

	return m_Value;
}

void Pickup::update(float elapsedTime) {
	if (m_Spawned)
		m_SecondsSinceSpawn += elapsedTime;
	else
		m_SecondsSinceDeSpawn += elapsedTime;

	// Do we need to hide a pickup?
	if (m_SecondsSinceSpawn > m_SecondsToLive && m_Spawned) {
		// Remove the pickup and put it somewhere else
		m_Spawned = false;
		m_SecondsSinceDeSpawn = 0;
	}

	if (m_SecondsSinceDeSpawn > m_SecondsToWait && !m_Spawned)
		spawn(); // Spawn the pickup and reset the timer
}

void Pickup::upgrade() {
	switch (m_Type) {
		case Type::AMMO:
			m_Value += (HEALTH_START_VALUE * _UPGRADE_FACTOR);
			break;
		case Type::HEALTH:
			m_Value += (HEALTH_START_VALUE * _UPGRADE_FACTOR);
			break;
		default:
			break;
	}

	// Make them more frequent and last longer
	m_SecondsToLive += (START_SECONDS_TO_LIVE / _RESPAWN_BOOST);
	m_SecondsToWait -= (START_WAIT_TIME / _RESPAWN_BOOST);
}
