#include "Bat.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstdlib>
#include <sstream>

#define FONTS_DIR "fonts/"
#define LCD_FONT FONTS_DIR "alarm clock.ttf"

#define FONT_SIZE 75
#define FONT_POS_X 20
#define FONT_POS_Y 20

#define LOW_RES // Comment to use larger size
#define BAT_START_OFFSET 20


int main(void)
{
	const struct {
		int x;
		int y;
	} SCREEN_SIZE = {1920, 1080};

#define SCREEN_CENTER_X SCREEN_SIZE.x / 2.0f
#define SCREEN_CENTER_Y SCREEN_SIZE.y / 2.0f

#ifdef LOW_RES
	// Low res
	VideoMode vm(960, 540);
	RenderWindow window(vm, "TimTheTimber", Style::Titlebar);
	View view(sf::FloatRect(0, 0, SCREEN_SIZE.x, SCREEN_SIZE.y));
	window.setView(view);
#else
	// Create a video mode object to setup window
	VideoMode vm(SCREEN_SIZE.x, SCREEN_SIZE.y);

	// Create and open a window for this game
	RenderWindow window(vm, "Pong", Style::Fullscreen);
#endif

	/**************************************************
	 * Load assets and Game Objects
	 **************************************************/

	int score = 0, lives = 3;

	// Create a bat at the bottom center of the screen
	Bat bat(SCREEN_CENTER_X, SCREEN_SIZE.y - BAT_START_OFFSET);

	// We will add a ball in the next chapter

	// Create a Text object called HUD
	Text hud;

	// A cool retro-style font
	Font font;
	font.loadFromFile(LCD_FONT);

	// Set hte font to our retro-style
	hud.setFont(font);

	// Make it nice and big
	hud.setCharacterSize(FONT_SIZE);

	// Choose a color
	hud.setFillColor(Color::White);

	hud.setPosition(FONT_POS_X, FONT_POS_Y);

	// Here is our clock for timing everything
	Clock clock;

	while (window.isOpen()) {

		// Update the delta time
		Time delta = clock.restart();

		/**************************************************
		 * Handle the player input
		 **************************************************/

		Event event;
		while (window.pollEvent(event))

			if (event.type == Event::Closed)
				window.close(); // Quit the game when the window is closed
			else if (event.type == Event::KeyPressed)
				switch (event.key.code) { // Handle the pressing of the arrow keys
					case Keyboard::Left: bat.moveLeft(); break;
					case Keyboard::Right: bat.moveRight(); break;
					default: break;
				}
			else if (event.type == Event::KeyReleased)
				switch (event.key.code) {
					// Handle the releasing of the arrow keys
					case Keyboard::Left: bat.stopLeft(); break;
					case Keyboard::Right: bat.stopRight(); break;
					// Handle the player quitting
					case Keyboard::Escape: window.close(); break;
					default: break;
				}

		/**************************************************
		 * Update the bat, the ball and the HUD
		 **************************************************/

		bat.update(delta);
		// Update the HUD text
		std::stringstream ss;
		ss << "SCORE: " << score << " LIVES:" << lives;
		hud.setString(ss.str());

		/**************************************************
		 * Draw the bat, the ball and the HUD
		 **************************************************/
		window.clear();
		window.draw(hud);
		window.draw(bat.getShape());
		window.display();
	}
}
