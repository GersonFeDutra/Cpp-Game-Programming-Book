#include "Bullet.h"
#include "Pickup.h"
#include "Player.h"
#include "TextureHolder.h"
#include "ZombieArena.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>

#define ARENA_START_WIDTH 500
#define ARENA_START_HEIGHT 500

#define BULLETS_LIM 100 // Maximum number of bullets on memory
#define DEFAULT_BULLETS_SPARE 24
#define DEFAULT_BULLETS_INCLIP 6
#define DEFAULT_CLIP_SIZE 6
#define DEFAULT_FIRE_RATE 1.0

#define SHOOT_DELAY 1000 // ms

#define SAVE "gamedata/scores.txt"
#define _AUDIO "sound/"
#define HIT_WAV _AUDIO "hit.wav"
#define SPLAT_WAV _AUDIO "splat.wav"
#define SHOOT_WAV _AUDIO "shoot.wav"
#define RELOAD_WAV _AUDIO "reload.wav"
#define FAILED_WAV _AUDIO "reload_failed.wav"
#define POWERUP_WAV _AUDIO "powerup.wav"
#define PICKUP_WAV _AUDIO "pickup.wav"

#define _IMGS "graphics/"
#define BG_PNG _IMGS "background_sheet.png"
#define BACKGROUND_PNG _IMGS "background.png"

#define CROSSHAIR_PNG _IMGS "crosshair.png"
#define CROSSHAIR_OFFSET 25

#define AMMO_PNG _IMGS "ammo_icon.png"
#define AMMO_ICON_X 20
#define AMMO_ICON_Y 980

#define HUD_FONT "fonts/zombiecontrol.ttf"
#define DEFAULT_FONT_SIZE 55
#define HUD_FONT_SIZE 155
#define PAUSE_OFFSET 400

#define GAME_OVER_SIZE 125
#define GAME_OVER_X 250
#define GAME_OVER_Y 850

#define LVL_UP_SIZE 80
#define LVL_UP_X 150
#define LVL_UP_Y 250

#define AMMO_FONT_X 200
#define AMMO_FONT_Y 980

#define SCORE_OFFSET 20
#define HI_SCORE_OFFSET 1400

#define ZOMBIES_COUNTER_X 1500
#define ZOMBIES_COUNTER_Y 980

#define WAVE_COUNTER_X 1250
#define WAVE_COUNTER_Y 980

#define HEALTH_BAR_X 450
#define HEALTH_BAR_Y 980

#define DEBUG_FONT_SIZE 25
#define DEBUG_X 20
#define DEBUG_Y 220

#define DEFAULT_FPS_INTERVAL 1000

#define HEALTH_BAR_HEIGHT 50

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

	// HUD

	// For the home/ game over screen
	Sprite spriteGameOver;
	Texture textureGameOver = TextureHolder::GetTexture(BACKGROUND_PNG);
	spriteGameOver.setTexture(textureGameOver);
	spriteGameOver.setPosition(0, 0);

	// Create a view for the HUD
	View hudView(sf::FloatRect(0, 0, resolution.x, resolution.y));

	// Create a sprite for the ammo icon
	Sprite spriteAmmoIcon;
	Texture textureAmmoIcon = TextureHolder::GetTexture(AMMO_PNG);
	spriteAmmoIcon.setTexture(textureAmmoIcon);
	spriteAmmoIcon.setPosition(AMMO_ICON_X, AMMO_ICON_Y);

	// Load the font
	Font font;
	font.loadFromFile(HUD_FONT);

	// Paused
	Text pausedText;
	pausedText.setFont(font);
	pausedText.setCharacterSize(HUD_FONT_SIZE);
	pausedText.setFillColor(Color::White);
	pausedText.setPosition(PAUSE_OFFSET, PAUSE_OFFSET);
	pausedText.setString("Press Enter\n to continue");

	// Game Over
	Text gameOverText;
	gameOverText.setFont(font);
	gameOverText.setCharacterSize(GAME_OVER_SIZE);
	gameOverText.setFillColor(Color::White);
	gameOverText.setPosition(GAME_OVER_X, GAME_OVER_Y);
	gameOverText.setString("Press Enter to play");

	// Leveling up
	Text lvlUpText;
	lvlUpText.setFont(font);
	lvlUpText.setCharacterSize(LVL_UP_SIZE);
	lvlUpText.setFillColor(Color::White);
	lvlUpText.setPosition(LVL_UP_X, LVL_UP_Y);

	// clang-format off
	std::stringstream lvlUpStream;
	lvlUpStream <<
		"1- Increased rate of fire\n"
		"2 - Increased clip size(next reaload)\n"
		"3 - Increased max health\n"
		"4 - Increased run speed\n"
		"5 - More and better health pickups\n"
		"6 - More and better ammo pickups\n";
	lvlUpText.setString(lvlUpStream.str());
	// clang-format on

	// Ammo
	Text ammoText;
	ammoText.setFont(font);
	ammoText.setCharacterSize(DEFAULT_FONT_SIZE);
	ammoText.setFillColor(Color::White);
	ammoText.setPosition(AMMO_FONT_X, AMMO_FONT_Y);

	// Score
	Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(DEFAULT_FONT_SIZE);
	scoreText.setFillColor(Color::White);
	scoreText.setPosition(SCORE_OFFSET, 0);

	// Load the high score from a text file
	std::ifstream inputFile(SAVE);
	if (inputFile.is_open()) {
		// >> Reads the data
		inputFile >> hiScore;
		inputFile.close();
	}

	// High score
	Text hiScoreText;
	hiScoreText.setFont(font);
	hiScoreText.setCharacterSize(DEFAULT_FONT_SIZE);
	hiScoreText.setFillColor(Color::White);
	hiScoreText.setPosition(HI_SCORE_OFFSET, 0);
	std::stringstream s_score;
	s_score << "Hi Score: " << hiScore;
	hiScoreText.setString(s_score.str());

	// Zombies remaining
	Text zombiesRemainingText;
	zombiesRemainingText.setFont(font);
	zombiesRemainingText.setCharacterSize(DEFAULT_FONT_SIZE);
	zombiesRemainingText.setFillColor(Color::White);
	zombiesRemainingText.setPosition(ZOMBIES_COUNTER_X, ZOMBIES_COUNTER_Y);
	zombiesRemainingText.setString("Zombies: 100");

	// Wave number
	int wave = 0;
	Text waveNumberText;
	waveNumberText.setFont(font);
	waveNumberText.setCharacterSize(DEFAULT_FONT_SIZE);
	waveNumberText.setFillColor(Color::White);
	waveNumberText.setPosition(WAVE_COUNTER_X, WAVE_COUNTER_Y);
	waveNumberText.setString("Wave: 0");

	// Health bar
	RectangleShape healthBar;
	healthBar.setFillColor(Color::Red);
	healthBar.setPosition(HEALTH_BAR_X, HEALTH_BAR_Y);

	// Debug HUD
	Text debugText;
	debugText.setFont(font);
	debugText.setCharacterSize(DEBUG_FONT_SIZE);
	debugText.setFillColor(Color::White);
	debugText.setPosition(DEBUG_X, DEBUG_Y);
	std::ostringstream ss;

	// When did we last update the HUD?
	int framesSinceLastHudUpdate = 0;

	// How often (in frames) should we update the HUD
	int fpsMeasurementFrameInterval = DEFAULT_FPS_INTERVAL;

	// Audio

	// Prepare the hit sound
	SoundBuffer hitBuffer;
	hitBuffer.loadFromFile(HIT_WAV);
	Sound hit;
	hit.setBuffer(hitBuffer);

	// Prepare the splat sound
	SoundBuffer splatBuffer;
	splatBuffer.loadFromFile(SPLAT_WAV);
	Sound splat;
	splat.setBuffer(splatBuffer);

	// Prepare the shoot sound
	SoundBuffer shootBuffer;
	shootBuffer.loadFromFile(SHOOT_WAV);
	Sound shoot;
	shoot.setBuffer(shootBuffer);

	// Prepare the reload sound
	SoundBuffer reloadBuffer;
	reloadBuffer.loadFromFile(RELOAD_WAV);
	Sound reload;
	reload.setBuffer(reloadBuffer);

	// Prepare the reload failed sound
	SoundBuffer failedBuffer;
	failedBuffer.loadFromFile(FAILED_WAV);
	Sound failed;
	failed.setBuffer(failedBuffer);

	// Prepare the powerup sound
	SoundBuffer powerupBuffer;
	powerupBuffer.loadFromFile(POWERUP_WAV);
	Sound powerup;
	powerup.setBuffer(powerupBuffer);

	// Prepare the pickup sound
	SoundBuffer pickupBuffer;
	pickupBuffer.loadFromFile(PICKUP_WAV);
	Sound pickup;
	pickup.setBuffer(pickupBuffer);

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
							case State::GAME_OVER: {
								state = State::LEVELING_UP;
								wave = 0;
								score = 0;

								// Prepare the gun and ammo for next game
								currentBullet = 0;
								bulletsSpare = DEFAULT_BULLETS_SPARE;
								bulletsInClip = DEFAULT_BULLETS_INCLIP;
								clipSize = DEFAULT_CLIP_SIZE;
								fireRate = DEFAULT_FIRE_RATE;

								// Reset the player's stats
								player.resetPlayerStats();
							} break;
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
									reload.play();
								} else if (bulletsSpare > 0) {
									// Only few bullets left
									bulletsInClip = bulletsSpare;
									bulletsSpare = 0;
									reload.play();
								} else {
									failed.play();
								}
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

							shoot.play();
							bulletsInClip--;
						} // End fire a bullet
						// clang-format on
					} break;
					case State::LEVELING_UP: {
						switch (event.key.code) {
							case Keyboard::Num1: {
								// Increase fire rate
								fireRate++;
								state = State::PLAYING;
							} break;
							case Keyboard::Num2: {
								// Increase clip size
								clipSize += clipSize;
								state = State::PLAYING;
							} break;
							case Keyboard::Num3: {
								// Increase health
								player.upgradeHealth();
								state = State::PLAYING;
							} break;
							case Keyboard::Num4: {
								// Increase speed
								player.upgradeSpeed();
								state = State::PLAYING;
							} break;
							case Keyboard::Num5: {
								// Upgrade pickup
								healthPickup.upgrade();
								state = State::PLAYING;
							} break;
							case Keyboard::Num6: {
								// Upgrade pickup
								ammoPickup.upgrade();
								state = State::PLAYING;
							} break;
							default:
								break;
						}
						if (state == State::PLAYING) {
							// Increase the wave number
							wave++;

							// Preapare the level
							arena.width = ARENA_START_WIDTH * wave;
							arena.height = ARENA_START_HEIGHT * wave;
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
							numZombies = 5 * wave;

							// Delete the previously allocated memory (if it exists)
							delete[] zombies;
							zombies = createHorde(numZombies, arena);
							numZombiesAlive = numZombies;

							// Play the powerup sound
							powerup.play();

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

									// Make a splat sound
									splat.play();
								}
							}
				} // End zombie being shot

				// Have any zombis touched the player
				for (int i = 0; i < numZombiesAlive; i++) {
					if (zombies[i].isAlive() && player.getPosition().intersects(zombies[i].getPosition())) {
						if (player.hit(gameTimeTotal))
							hit.play();
						if (player.getHealth() <= 0) {
							state = State::GAME_OVER;

							std::ofstream outputFile(SAVE);
							// << writes the data
							outputFile << hiScore;
							outputFile.close();
						}
					}
				} // End player touched

				// Has the player touched health pickup
				if (healthPickup.isSpawned() && player.getPosition().intersects(healthPickup.getPosition())) {
					player.increaseHealthLvl(healthPickup.gotIt());
					pickup.play();
				}

				// Has the player touched ammo pickup
				if (ammoPickup.isSpawned() && player.getPosition().intersects(ammoPickup.getPosition())) {
					bulletsSpare += ammoPickup.gotIt();
					reload.play();
				}

				// Size up the health bar
				healthBar.setSize(Vector2f(player.getHealth() * 3, HEALTH_BAR_HEIGHT));

				// Increment the number of frames since the previous update
				framesSinceLastHudUpdate++;

				// re-caulculate every fpsMeasurementFrameInterval frames
				if (framesSinceLastHudUpdate > fpsMeasurementFrameInterval) {
					// Update game HUD text
					std::stringstream ssAmmo, ssScore, ssHiScore, ssWave, ssZombiesAlive;

					// Update the ammo text
					ssAmmo << bulletsInClip << "/" << bulletsSpare;
					ammoText.setString(ssAmmo.str());

					// Update the score text
					ssScore << "Score: " << score;
					scoreText.setString(ssScore.str());

					// Updtate the high score text
					ssHiScore << "High Score: " << hiScore;
					hiScoreText.setString(ssHiScore.str());

					// Update the wave
					ssWave << "Wave: " << wave;
					waveNumberText.setString(ssWave.str());

					// Update the high score text
					ssZombiesAlive << "Zombies: " << numZombiesAlive;
					zombiesRemainingText.setString(ssZombiesAlive.str());

					framesSinceLastHudUpdate = 0;
				} // End HUD update

				// Increment the number
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

				// Switch to the HUD view
				window.setView(hudView);

				// Draw all the HUD elements
				window.draw(spriteAmmoIcon);
				window.draw(ammoText);
				window.draw(scoreText);
				window.draw(hiScoreText);
				window.draw(healthBar);
				window.draw(waveNumberText);
				window.draw(zombiesRemainingText);
			} break;
			case State::LEVELING_UP: {
				window.draw(spriteGameOver);
				window.draw(lvlUpText);
			} break;
			case State::PAUSED: {
				window.draw(pausedText);
			} break;
			case State::GAME_OVER: {
				window.draw(spriteGameOver);
				window.draw(gameOverText);
				window.draw(scoreText);
				window.draw(hiScoreText);
			} break;
			default:
				break;
		}
		window.display();
	} // End game loop

	// Delete the previously allocated memory (if it exists)
	delete[] zombies;
}
