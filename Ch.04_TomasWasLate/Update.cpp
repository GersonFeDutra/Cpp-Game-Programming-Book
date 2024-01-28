#include "Engine.h"
#include <SFML/Graphics.hpp>
#include <sstream>

using namespace sf;

#define SPAWN_DELAY 10

void Engine::update(float deltaAsSec) {
	if (m_NewLevelRequired)
		loadLevel(); // Load a level

	if (m_Playing) {
		// Update Thomas
		m_Thomas.update(deltaAsSec);

		// Update Bob
		m_Bob.update(deltaAsSec);

		// Detect collisions and see if characters have reached the goal tile
		// The second part of the if condition is only executed
		// when Thomas is touching the home tile
		if (detectCollisions(m_Thomas) & detectCollisions(m_Bob)) {
			m_NewLevelRequired = true;

			// Play the reach goal sound
		}

		// Let bob and Thomas jump on each others heads
		if (m_Bob.getFeet().intersects(m_Thomas.getHead()))
			m_Bob.stopFalling(m_Thomas.getHead().top);
		else if (m_Thomas.getFeet().intersects(m_Bob.getHead()))
			m_Thomas.stopFalling(m_Bob.getHead().top);

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
