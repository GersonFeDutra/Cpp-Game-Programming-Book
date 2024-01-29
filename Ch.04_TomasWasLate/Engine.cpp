#include "Engine.h"

#define _IMGS "graphics/"
#define BG_PNG _IMGS "background.png"
#define TILES_PNG _IMGS "tiles_sheet.png"

#define _SHADERS "shaders/"
#define VERT_SHADER _SHADERS "vertShader.vert"
#define RIPPLE_SHADER _SHADERS "rippleShader.frag"

#define L_START_HV 0.001f
#define L_END_H 0.498f
#define L_END_V 0.998f

#define R_START_H 0.5f
#define R_START_V L_START_HV
#define R_END_H 0.499f
#define R_END_V 0.998f

#define PARTICLES_N 1000

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

	// Can this graphics card use shaders?
	if (!sf::Shader::isAvailable()) {
		// Time to get a new PC
		// or remove all the shader related code :(
		m_Window.close();
	} else
		// Load two shaders (1 vertex, 1 fragment)
		m_RippleShader.loadFromFile(VERT_SHADER, RIPPLE_SHADER);

	m_BackgroundTexture = TextureHolder::GetTexture(BG_PNG);

	// Associate the sprite with the texture
	m_BackgroundSprite.setTexture(m_BackgroundTexture);

	// Load the texture for the background vertex array
	m_LevelTiles = TextureHolder::GetTexture(TILES_PNG);

	// Initialize the particle system
	m_PS.init(PARTICLES_N);
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
