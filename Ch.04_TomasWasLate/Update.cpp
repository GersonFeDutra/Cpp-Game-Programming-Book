#include "Engine.h"
#include <SFML/Graphics.hpp>
#include <sstream>

using namespace sf;

#define SPAWN_DELAY 10
#define EMITTER_EXPANSION 500.0

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
			m_SM.playReachGoal();
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

	// Check if a fire sound needs to be played

	// Iterate through the vector of Vector2f objects
	for (auto it : m_FireEmitters) {
		// Where is this emitter?
		// Store the location in pos
		float posX = it.x;
		float posY = it.y;

		// is the emitter near the player?
		// Make a rectangle around the emitter
		FloatRect localRect(posX - EMITTER_EXPANSION / 2, posY - EMITTER_EXPANSION / 2, EMITTER_EXPANSION, EMITTER_EXPANSION);

		// Is the player inside localRect?
		if (m_Thomas.getPosition().intersects(localRect))
			// Play the sound and pass in the location as well
			m_SM.playFire(Vector2f(posX, posY), m_Thomas.getCenter());
	}

	// Set the appropriate view around the appropriate character
	if (m_SplitScreen) {
		m_LeftView.setCenter(m_Thomas.getCenter());
		m_RightView.setCenter(m_Bob.getCenter());
	} else {
		// Centre full screen around appropriate character
		m_MainView.setCenter(m_Character1 ? m_Thomas.getCenter() : m_Bob.getCenter());
	}

	// Time to update the HUD?
	// Increment the number of frames since the last HUD calculation
	m_FramesSinceLastHudUpdate++;

	// Update the Hud every m_TargetFramesPerHudUpdate frames
	if (m_FramesSinceLastHudUpdate > m_TargetFramesPerHudUpdate) {
		// Update game HUD text
		stringstream ssTime;
		stringstream ssLevel;

		// Update the time text
		ssTime << (int)m_TimeRemaining;
		m_Hud.setTime(ssTime.str());

		// Update the level text
		ssLevel << "Level:" << m_LM.getCurrentLevel();
		m_Hud.setLevel(ssLevel.str());
		m_FramesSinceLastHudUpdate = 0;
	}
} // End of update function
