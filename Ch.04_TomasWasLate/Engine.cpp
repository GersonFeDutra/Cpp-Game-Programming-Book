#include "Engine.h"

#define _IMGS "graphics/"
#define BG_PNG _IMGS "background.png"

#define L_START_HV 0.001f
#define L_END_H 0.498f
#define L_END_V 0.998f

#define R_START_H 0.5f
#define R_START_V L_START_HV
#define R_END_H 0.499f
#define R_END_V 0.998f

Engine::Engine() {
	// Get the screen resolution
	// and create an SFML window and View
	Vector2f resolution;
	resolution.x = VideoMode::getDesktopMode().width;
	resolution.y = VideoMode::getDesktopMode().height;

	m_Window.create(VideoMode(resolution.x, resolution.y), "Thomas was late", Style::Fullscreen);

	// Initialize the full screen view
	m_MainView.setSize(resolution);
	m_HudView.reset(
			FloatRect(0, 0, resolution.x, resolution.y));

	// Initialize the split-screen views
	m_LeftView.setViewport(
			FloatRect(L_START_HV, L_START_HV, L_END_H, L_END_V));

	m_RightView.setViewport(
			FloatRect(R_START_H, R_START_V, R_END_H, R_END_V));

	m_BGLeftView.setViewport(
			FloatRect(L_START_HV, L_START_HV, L_END_H, L_END_V));

	m_BGRightView.setViewport(
			FloatRect(R_START_H, R_START_V, R_END_H, R_END_V));

	m_BackgroundTexture = TextureHolder::GetTexture(BG_PNG);

	// Associate the sprite with the texture
	m_BackgroundSprite.setTexture(m_BackgroundTexture);
}

void Engine::run() {
	// Timing
	Clock clock;

	while (m_Window.isOpen()) {
		Time delta = clock.restart();
		// Update the total game time
		m_GameTimeTotal += delta;

		// Make a decimal fraction from the delta time
		float deltaAsSec = delta.asSeconds();

		// Call each part of the game loop in turn
		input();
		update(deltaAsSec);
		draw();
	}
}
