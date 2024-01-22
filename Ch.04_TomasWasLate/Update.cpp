#include "Engine.h"
#include <SFML/Graphics.hpp>
#include <sstream>

using namespace sf;

#define SPAWN_DELAY 10

void Engine::update(float deltaAsSec) {
	if (m_NewLevelRequired) {
		// These calls to spawn will be moved to
		// a new loadLevel() function soon.
		// Spawn Thomas and Bob.
		m_Thomas.spawn(Vector2f(0, 0), GRAVITY);
		m_Bob.spawn(Vector2f(100, 0), GRAVITY);

		// Makw sure spawn is called only once
		m_TimeRemaining = SPAWN_DELAY;
		m_NewLevelRequired = false;
	}

	if (m_Playing) {
		// Update Thomas
		m_Thomas.update(deltaAsSec);

		// Update Bob
		m_Bob.update(deltaAsSec);

		// Count down the time the player has left
		m_TimeRemaining -= deltaAsSec;

		// Have Thomas and Bob run out of time?
		if (m_TimeRemaining <= 0)
			m_NewLevelRequired = true;
	} // End if playing

	// Set the appropriate view around the appropriate character
	if (m_SplitScreen) {
		m_LeftView.setCenter(m_Thomas.getCenter());
		m_RightView.setCenter(m_Bob.getCenter());
	} else {
		// Centre full screen around appropriate character
		m_MainView.setCenter(m_Character1 ? m_Thomas.getCenter() : m_Bob.getCenter());
	}
}
