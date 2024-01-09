#include "Bullet.h"
#include "Pickup.h"
#include "Player.h"
#include "TextureHolder.h"
#include "ZombieArena.h"
#include <SFML/Graphics.hpp>

#define ARENA_START_WIDTH 500
#define ARENA_START_HEIGHT 500

#define BULLETS_LIM 100 // Maximum number of bullets on memory
#define DEFAULT_BULLETS_SPARE 24
#define DEFAULT_BULLETS_INCLIP 6
#define DEFAULT_CLIP_SIZE 6
#define DEFAULT_FIRE_RATE 1.0

#define SHOOT_DELAY 1000 // ms

#define _IMGS "graphics/"
#define BG_PNG _IMGS "background_sheet.png"
#define CROSSHAIR_PNG _IMGS "crosshair.png"
#define CROSSHAIR_OFFSET 25

#define get_input_axis(DIRECTION)                                  \
	{                                                              \
		DIRECTION.y -= (float)Keyboard::isKeyPressed(Keyboard::W); \
		DIRECTION.y += (float)Keyboard::isKeyPressed(Keyboard::S); \
		DIRECTION.x -= (float)Keyboard::isKeyPressed(Keyboard::A); \
		DIRECTION.x += (float)Keyboard::isKeyPressed(Keyboard::D); \
	}

using namespace sf;

int main(void) {
	TextureHolder holder;

	// The game will always be in one of four states
	enum class State { PAUSED,
		LEVELING_UP,
		GAME_OVER,
		PLAYING };

	// Start with the GAME_OVER state
	State state = State::GAME_OVER;

	// Get the screen resolution and create an SFML window
	Vector2f resolution;
	resolution.x = VideoMode::getDesktopMode().width;
	resolution.y = VideoMode::getDesktopMode().height;

	RenderWindow window(VideoMode(resolution.x, resolution.y), "Zombie Arena", Style::Fullscreen);

	// Create an SFML View for the main action
	View mainView(sf::FloatRect(0, 0, resolution.x, resolution.y));

	// Here is our clock for timing everything
	Clock clock;

	// How long has the PLAYING state been active
	Time gameTimeTotal;

	// Where is the mouse in relation to world coordinates
	Vector2f mouseWorldPosition;

	// Where is the mouse in relation to screen coordinates
	Vector2i mouseScreenPosition;

	// Create an instance of the Player class
	Player player;

	// The boundaries of the arena
	IntRect arena;

	// Create the background
	VertexArray background;
	// Load the texture for our background vertex array
	Texture textureBackground = TextureHolder::GetTexture(BG_PNG);

	// Prepare for a horde of zombies
	int numZombies;
	int numZombiesAlive;
	Zombie *zombies = nullptr;

	// 100 bullets should do
	Bullet bullets[BULLETS_LIM];
	int currentBullet = 0;
	int bulletsSpare = DEFAULT_BULLETS_SPARE;
	int bulletsInClip = DEFAULT_BULLETS_INCLIP;
	int clipSize = DEFAULT_CLIP_SIZE;
	float fireRate = DEFAULT_FIRE_RATE;
	// When was the fire button last pressed?
	Time lastPressed;

	// Hide the mouse pointer and replace it with cross-hair
	window.setMouseCursorVisible(true);
	Sprite spriteCrosshair;
	Texture textureCrosshair = TextureHolder::GetTexture(CROSSHAIR_PNG);
	spriteCrosshair.setTexture(textureCrosshair);
	spriteCrosshair.setOrigin(CROSSHAIR_OFFSET, CROSSHAIR_OFFSET);

	// Create a couple of pickups
	Pickup healthPickup(Pickup::Type::HEALTH);
	Pickup ammoPickup(Pickup::Type::AMMO);

	// About the game
	int score = 0;
	int hiScore = 0;

	// The main game loop
	while (window.isOpen()) {
		/*
		 *  _    _                    _  _         _                       _
		 * | |  | |                  | || |       (_)                     | |
		 * | |__| |  __ _  _ __    __| || |  ___   _  _ __   _ __   _   _ | |_
		 * |  __  | / _` || '_ \  / _` || | / _ \ | || '_ \ | '_ \ | | | || __|
		 * | |  | || (_| || | | || (_| || ||  __/ | || | | || |_) || |_| || |_
		 * |_|  |_| \__,_||_| |_| \__,_||_| \___| |_||_| |_|| .__/  \__,_| \__|
		 *													| |
		 *													|_|
		 */
		// Handle events by polling
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::KeyPressed) {
				switch (event.key.code) {
					case Keyboard::Return:
						switch (state) {
							// Pause a game while playing
							case State::PLAYING:
								state = State::PAUSED;
								break;
							case State::PAUSED: {
								// Restart while paused
								state = State::PLAYING;
								// Reset the clock so there isn't a frame jump
								clock.restart();
							} break;
							// Restart the game while in game over
							case State::GAME_OVER:
								state = State::LEVELING_UP;
								break;
							default:
								break;
						}
						break;
					// Handle the player quitting
					case sf::Keyboard::Escape:
						window.close();
						break;
					default:
						break;
				}
				switch (state) {
					case State::PLAYING: {
						switch (event.key.code) {
							case Keyboard::R: { // Reloading
								if (bulletsSpare >= clipSize) {
									// Plenty of bullets. Reload
									bulletsInClip = clipSize;
									bulletsSpare -= clipSize;
								} else if (bulletsSpare > 0) {
									// Only few bullets left
									bulletsInClip = bulletsSpare;
									bulletsSpare = 0;
								} else
									; // More here soon?!
							} break;
							default:
								break;
						}

						Vector2f direction(.0f, .0f); // Handle WASD while playing
						get_input_axis(direction);
						player.move(direction);

						// Fire a bullet
						// clang-format off
						if (Mouse::isButtonPressed(sf::Mouse::Left) &&
								(gameTimeTotal.asMilliseconds()
								 - lastPressed.asMilliseconds()
								 > 1000 / fireRate && bulletsInClip > 0)) {
							// Pass the centre of the player and the centre of
							// the cross-hair to the shoot funtion
							bullets[currentBullet].shoot(
									player.getCenter().x, player.getCenter().y,
									mouseWorldPosition.x, mouseWorldPosition.y);
							++currentBullet %= BULLETS_LIM;
							lastPressed = gameTimeTotal;
							bulletsInClip--;
						} // End fire a bullet
						// clang-format on
					} break;
					case State::LEVELING_UP: {
						switch (event.key.code) {
							case Keyboard::Num1:
							case Keyboard::Num2:
							case Keyboard::Num3:
							case Keyboard::Num4:
							case Keyboard::Num5:
							case Keyboard::Num6: {
								state = State::PLAYING;
							} break;
							default:
								break;
						}
						if (state == State::PLAYING) {
							// Prepare the level
							// We will modify the next two lines later
							arena.width = ARENA_START_WIDTH;
							arena.height = ARENA_START_HEIGHT;
							arena.top = arena.left = 0;

							// Pass the vertex array by reference
							// to the createBackground function
							int tileSize = createBackground(background, arena);

							// Spawn the player in the middle of the arena
							player.spawn(arena, resolution, tileSize);

							// Configure the pickups
							healthPickup.setArena(arena);
							ammoPickup.setArena(arena);

							// Create a horde of zombies
							numZombies = 10;

							// Delete the previously allocated memory (if it exists)
							delete[] zombies;
							zombies = createHorde(numZombies, arena);
							numZombiesAlive = numZombies;

							// Reset the clock so there isn't a frame jump
							clock.restart();
						}
					} // End LEVELING UP
					default:
						break;
				} // End state machine
			} else if (event.type == Event::KeyReleased) {
				switch (state) {
					case State::PLAYING: {
						Vector2f direction(.0f, .0f); // Handle WASD while playing
						get_input_axis(direction);
						player.move(direction);
					} break;
					default:
						break;
				}
			}

		} // End event polling

		/*
		 *  _    _           _       _          __
		 * | |  | |         | |     | |        / _|
		 * | |  | |_ __   __| | __ _| |_ ___  | |_ _ __ __ _ _ __ ___   ___
		 * | |  | | '_ \ / _` |/ _` | __/ _ \ |  _| '__/ _` | '_ ` _ \ / _ \
		 * | |__| | |_) | (_| | (_| | ||  __/ | | | | | (_| | | | | | |  __/
		 *  \____/| .__/ \__,_|\__,_|\__\___| |_| |_|  \__,_|_| |_| |_|\___|
		 *        | |
		 *        |_|
		 */
		switch (state) {
			case State::PLAYING: {
				// Update the delta time
				Time delta = clock.restart();

				// Update the total game time
				gameTimeTotal += delta;

				// Make a decimal fraction of 1 from the delta time
				float deltaAsSec = delta.asSeconds();

				// Where is the mouse pointer
				mouseScreenPosition = Mouse::getPosition();

				// Convert mouse position to world coordinates of mainView
				mouseWorldPosition = window.mapPixelToCoords(Mouse::getPosition(), mainView);

				// Set the cross-hair to the mouse world location
				spriteCrosshair.setPosition(mouseWorldPosition);

				// Update the player
				player.update(deltaAsSec, mouseScreenPosition);

				// Make a note of the players new position
				Vector2f playerPos(player.getCenter());

				// Make the view centre around the player
				mainView.setCenter(player.getCenter());

				// Loop through each Zombie and update them
				for (int i = 0; i < numZombies; i++)
					if (zombies[i].isAlive())
						zombies[i].update(delta.asSeconds(), playerPos);

				// Update any bullets that are in-flight
				for (int i = 0; i < BULLETS_LIM; i++) // TODO -> Optimize this
					if (bullets[i].isInFlight())
						bullets[i].update(deltaAsSec);

				// Update the pickups
				healthPickup.update(deltaAsSec);
				ammoPickup.update(deltaAsSec);

				// Collision detection
				// Have any zombies been shot?
				for (int i = 0; i < BULLETS_LIM; i++) {
					for (int j = 0; j < numZombies; j++)
						if (bullets[i].isInFlight() && zombies[j].isAlive())
							if (bullets[i].getPosition().intersects(zombies[j].getPosition())) {
								// Stop the bullet
								bullets[i].stop();

								// Register the hit and see if it was a kill
								if (zombies[j].hit()) {
									// Not just the hit but a kill too
									score += 10;

									if (score >= hiScore)
										hiScore = score;

									numZombiesAlive--;

									// When all the zombies are dead (again)
									if (!numZombiesAlive)
										state = State::LEVELING_UP;
								}
							}
				} // End zombie being shot

				// Have any zombis touched the player
				for (int i = 0; i < numZombiesAlive; i++) {
					if (zombies[i].isAlive() && player.getPosition().intersects(zombies[i].getPosition())) {
						if (player.hit(gameTimeTotal))
							; // More here later
						if (player.getHealth() <= 0)
							state = State::GAME_OVER;
					}
				} // End player touched

				// Has the player touched health pickup
				if (healthPickup.isSpawned() && player.getPosition().intersects(healthPickup.getPosition()))
					player.increaseHealthLvl(healthPickup.gotIt());

				// Has the player touched ammo pickup
				if (ammoPickup.isSpawned() && player.getPosition().intersects(ammoPickup.getPosition()))
					bulletsSpare += ammoPickup.gotIt();
			} break;
			default:
				break;
		} // End updating the scene

		/*
		 *  _____
		 * |  __ \
		 * | |  | |_ __ __ ___      __  ___  ___ ___ _ __   ___
		 * | |  | | '__/ _` \ \ /\ / / / __|/ __/ _ | '_ \ / _ \
		 * | |__| | | | (_| |\ V  V /  \__ | (_|  __| | | |  __/
		 * |_____/|_|  \__,_| \_/\_/   |___/\___\___|_| |_|\___|
		 *
		 */
		switch (state) {
			case State::PLAYING: {
				window.clear();

				// Set the mainView to be displayed in the window
				// and draw everything related to it
				window.setView(mainView);

				// Draw the background
				window.draw(background, &textureBackground);

				// Draw the zombies
				for (int i = 0; i < numZombies; i++)
					window.draw(zombies[i].getSprite());

				// Draw the bullets
				for (int i = 0; i < BULLETS_LIM; i++) // TODO -> Optimize this
					if (bullets[i].isInFlight())
						window.draw(bullets[i].getShape());

				// Draw the player
				window.draw(player.getSprite());

				// Draw the pick-upds, if currently spawned
				if (ammoPickup.isSpawned())
					window.draw(ammoPickup.getSprite());

				if (healthPickup.isSpawned())
					window.draw(healthPickup.getSprite());

				// Draw the cross-hair
				window.draw(spriteCrosshair);
			} break;
			case State::LEVELING_UP:
			case State::PAUSED:
			case State::GAME_OVER:
			default:
				break;
		}
		window.display();
	} // End game loop

	// Delete the previously allocated memory (if it exists)
	delete[] zombies;
}
