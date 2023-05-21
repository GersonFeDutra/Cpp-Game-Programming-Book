#include <SFML/Window/WindowStyle.hpp>
#include <sstream>
// Include SFML library APIs
#include <SFML/Graphics.hpp>

// Make code easier to type with "using namespace"
using namespace sf;

// Resources paths
#define ASSETS_DIR "assets/"
#define GRAPHICS_DIR ASSETS_DIR "graphics/"
#define BACKGROUND_PNG GRAPHICS_DIR "background.png"
#define TREE_PNG GRAPHICS_DIR "tree.png"
#define BEE_PNG GRAPHICS_DIR "bee.png"
#define CLOUD_PNG GRAPHICS_DIR "cloud.png"
#define FONTS_DIR ASSETS_DIR "fonts/"
#define KOMIKAP_FONT_TTF FONTS_DIR "KOMIKAP_.ttf"

#define TREE_COORDS_X 810
#define TREE_COORDS_Y 0
#define BEE_COORDS_X 10
#define BEE_COORDS_Y 800
#define CLOUD_COORDS_X -300
#define CLOUD_OFFSET_Y 250

#define CLOUDS_N 3
#define CLOUD_MIN_SPEED 10
#define CLOUD_MAX_SPEED 200
#define CLOUD_HEIGHT_OFFSET 150

#define BEE_BASE_SPEED 200
#define BEE_BASE_HEIGHT 500
#define BEE_START_POS_X 2000

#define MESSAGE_FONT_SIZE 75
#define SCORE_FONT_SIZE 100
#define SCORE_POS_X 20
#define SCORE_POS_Y 20

#define TIME_BAR_START_WIDTH 400
#define TIME_BAR_HEIGHT 80
#define GAME_DEFAULT_TIME 6.0f

// This is where our game starts from
int main()
{
	const struct {
		int x;
		int y;
	} SCREEN_SIZE = {1920, 1080};

	// Create a video mode object to setup window
	VideoMode va(SCREEN_SIZE.x, SCREEN_SIZE.y);

	// Create and open a window for this game
	RenderWindow window(va, "TimTheTimber", Style::Fullscreen);

	/*
	 ************************************************************
	 * Load persistent resources
	 ************************************************************
	 */

	// Create a texture to hold a graphic on the GPU
	Texture textureBackground;

	// Load a graphic into the texture
	textureBackground.loadFromFile(BACKGROUND_PNG);

	// Create a sprite
	Sprite spriteBackground;

	// Attach the texture to the sprite
	spriteBackground.setTexture(textureBackground);

	// Set the spriteBackground to cover the screen
	spriteBackground.setPosition(0, 0);

	// Make a tree sprite
	Texture textureTree;
	textureTree.loadFromFile(TREE_PNG);
	Sprite spriteTree;
	spriteTree.setTexture(textureTree);
	spriteTree.setPosition(TREE_COORDS_X, TREE_COORDS_Y);

	// Prepare the bee
	Texture textureBee;
	textureBee.loadFromFile(BEE_PNG);

	struct {
		Sprite sprite;
		bool active = false; // Is the bee currently moving?
		float speed = 0.0f;  // How fast can the bee fly
	} bee;

	bee.sprite.setTexture(textureBee);
	bee.sprite.setPosition(BEE_COORDS_X, BEE_COORDS_Y);
	bee.active = false;
	bee.speed = 0.0f;

	// Make 3 cloud sprites from 1 texture
	Texture textureCloud;

	textureCloud.loadFromFile(CLOUD_PNG);

	struct Cloud {
		Sprite sprite;
		bool active;    // Is the cloud currently on the screen?
		float speed;    // How fast is the cloud?
	} clouds[CLOUDS_N]; // 3 new sprites with the same texture

	for (int i = 0, offset = 0; i <= CLOUDS_N; i++, offset += CLOUD_OFFSET_Y) {
		Sprite *cloud = &clouds[i].sprite;
		cloud->setTexture(textureCloud);

		// Position the clouds on the left of the screen at different heights
		cloud->setPosition(CLOUD_COORDS_X, offset);

		clouds[i].active = false;
		clouds[i].speed = 0.0f;
	}

	// Draw some text
	struct {
		int points;
		Text text;
	} score;
	score.points = 0;

	Text messageText;

	// We need to choose a font
	Font font;
	font.loadFromFile(KOMIKAP_FONT_TTF);

	// Set the font to our message
	messageText.setFont(font);
	score.text.setFont(font);

	// Assign the actual message
	messageText.setString("Press key to start!");
	score.text.setString("Score = 0");

	// Make it really big
	messageText.setCharacterSize(MESSAGE_FONT_SIZE);
	score.text.setCharacterSize(SCORE_FONT_SIZE);

	// Choose a color
	messageText.setFillColor(Color::White);
	score.text.setFillColor(Color::White);

	// Position the text
	FloatRect textRect = messageText.getLocalBounds();

	messageText.setOrigin(
		textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);

	messageText.setPosition(SCREEN_SIZE.x / 2.0f, SCREEN_SIZE.y / 2.0f);

	score.text.setPosition(SCORE_POS_X, SCORE_POS_Y);

	/*
	 ************************************************************
	 * Main Loop
	 ************************************************************
	 */

	// Variable to control time itself
	Clock clock;

	// Time Bar
	struct {
		RectangleShape shape;
		Time gameTimeTotal;
		float timeRemaining;
		float widthPerSecond;
	} timeBar;

	timeBar.shape.setSize(Vector2f(TIME_BAR_START_WIDTH, TIME_BAR_HEIGHT));
	timeBar.shape.setFillColor(Color::Red);
	timeBar.shape.setPosition((SCREEN_SIZE.x / 2.0f) - TIME_BAR_START_WIDTH / 2.0f, SCREEN_SIZE.y);

	timeBar.timeRemaining = GAME_DEFAULT_TIME;
	timeBar.widthPerSecond = TIME_BAR_START_WIDTH / timeBar.timeRemaining;

	// Track whether the game is running
	bool paused = true;

	while (window.isOpen()) {
		/*
		 ************************************************************
		 * Handle the players input
		 ************************************************************
		 */

		if (Keyboard::isKeyPressed(Keyboard::Escape))
			window.close();

		// Start the game
		if (Keyboard::isKeyPressed(Keyboard::Return)) {
			paused = false;

			// Reset the time and the score
			score.points = 0;
			timeBar.timeRemaining = 6;
		}

		/*
		 ************************************************************
		 * Update the scene
		 ************************************************************
		 */

		// Measure time
		Time delta = clock.restart();

		if (!paused) {

			// Subtract from the amout of time remaining
			timeBar.timeRemaining -= delta.asSeconds();
			// Size up the time bar
			timeBar.shape.setSize(
				Vector2f(timeBar.widthPerSecond * timeBar.timeRemaining, TIME_BAR_HEIGHT));

			if (timeBar.timeRemaining <= 0.0f) {
				// Pause the game
				paused = true;

				// Change the message shown to the player
				messageText.setString("Out of time!!");

		// Setup the bee
		if (!bee.active) {
			// How fast is the bee?
			srand((int)time(0));
			bee.speed = (rand() % BEE_BASE_SPEED) + BEE_BASE_SPEED;
				// Reposition the text based on its size
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(
					textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
				messageText.setPosition(SCREEN_SIZE.x / 2.0f, SCREEN_SIZE.y / 2.0f);
			}

			// Setup the bee
			if (!bee.active) {
				// How fast is the bee?
				srand((int)time(0));
				bee.speed = (rand() % BEE_BASE_SPEED) + BEE_BASE_SPEED;

				// How high is the bee?
				srand((int)time(0) * 10);
				float height = (rand() % BEE_BASE_HEIGHT) + BEE_BASE_HEIGHT;
				bee.sprite.setPosition(BEE_START_POS_X, height);

				bee.active = true;
			}
			else {
				// Move the bee
				bee.sprite.setPosition(bee.sprite.getPosition().x - (bee.speed * delta.asSeconds()),
					bee.sprite.getPosition().y);

				// Has the bee reached the left-hand edge of the screen?
				if (bee.sprite.getPosition().x < -100)
					bee.active = false; // Set it up ready to be a whole new bee next frame
			}

			// Manage the clouds
			for (int i = 0, n = 1; i < CLOUDS_N; i++, n++) {
				Cloud *cloud = &clouds[i];

				if (!cloud->active) {
					// How fast is the cloud
					srand((int)time(0) * 10 * n);
					cloud->speed = (rand() % (CLOUD_MAX_SPEED - CLOUD_MIN_SPEED)) + CLOUD_MIN_SPEED;

					// How high is the cloud
					srand((int)time(0) * 10 * n);
					float height = (rand() % CLOUD_HEIGHT_OFFSET * n) - 150 * i;
					cloud->sprite.setPosition(CLOUD_COORDS_X * n, height);
					cloud->active = true;
				}
				else {
					cloud->sprite.setPosition(
						cloud->sprite.getPosition().x + (cloud->speed * delta.asSeconds()),
						cloud->sprite.getPosition().y);

					// Has the cloud reached the right hand edge of the screen?
					if (cloud->sprite.getPosition().x > SCREEN_SIZE.x)
						cloud->active = false; // Set it up ready to be a whole new cloud next frame
				}
			}

			// Update the score text
			std::stringstream ss;
			ss << "Score = " << score.points;
			score.text.setString(ss.str());
		} // End: if !paused

		/*
		 ************************************************************
		 * Draw the scene
		 ************************************************************
		 */

		// Clear everything from the last frame
		window.clear();

		// Draw our game scene here
		window.draw(spriteBackground);

		// Draw the clouds
		for (int i = 0; i < CLOUDS_N; ++i)
			window.draw(clouds[i].sprite);

		// Draw the tree
		window.draw(spriteTree);

		// Draw the insect
		window.draw(bee.sprite);

		// Draw the score
		window.draw(score.text);

		// Draw the time bar
		window.draw(timeBar.shape);

		if (paused)
			window.draw(messageText); // Draw our message

		// Show everything we just draw
		window.display();
	}

	return 0;
}
