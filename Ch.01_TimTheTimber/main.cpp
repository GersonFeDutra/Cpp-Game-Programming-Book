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
#define BRANCH_PNG GRAPHICS_DIR "branch.png"
#define PLAYER_PNG GRAPHICS_DIR "player.png"
#define LOG_PNG GRAPHICS_DIR "log.png"
#define RIP_PNG GRAPHICS_DIR "rip.png"
#define AXE_PNG GRAPHICS_DIR "axe.png"
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

#define BRANCHES_N 6
#define BRANCH_START_POS_X -2000
#define BRANCH_START_POS_Y -2000
#define BRANCH_ORIGIN_X 220
#define BRANCH_ORIGIN_Y 20
#define BRANCH_HEIGHT_OFFSET 150
#define BRANCH_LEFT_POS 610
#define BRANCH_RIGHT_POS 1130
#define BRANCH_HIDE_POS 3000

#define PLAYER_POS_X 700
#define PLAYER_POS_Y 830
#define PLAYER_POS_RIGHT 1200
#define PLAYER_POS_LEFT 580
#define PLAYER_START_POS_X PLAYER_POS_LEFT
#define PLAYER_START_POS_Y 720
#define PLAYER_HIDDEN_POS_X 2000
#define PLAYER_HIDDEN_POS_Y 600

#define AXE_POS_X 700
#define AXE_POS_Y 830
// Line the axe up with the tree
#define AXE_POS_LEFT 700.0f
#define AXE_POS_RIGHT 1075.0f
#define AXE_HIDDEN_POS 2000

#define LOG_POS_X 810
#define LOG_POS_Y 720
#define LOG_SPEED_X 1000
#define LOG_SPEED_Y -1500
#define LOG_SPEED -5000
#define LOG_RIGHT_LIM -100
#define LOG_LEFT_LIM 2000

#define RIP_POS_X 525
#define RIP_POS_Y 760
#define RIP_START_POS_X 600
#define RIP_START_POS_Y 860
#define RIP_HIDDEN_POS_X 675
#define RIP_HIDDEN_POS_Y 2000

#define LOW_RES

// Update the text axis to its center
#define text_axis_center(TEXT)                                                                     \
	FloatRect textRect = TEXT.getLocalBounds();                                                    \
	TEXT.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
#define text_screen_center(TEXT, SCREEN_SIZE)                                                      \
	text_axis_center(TEXT) TEXT.setPosition(SCREEN_SIZE.x / 2.0f, SCREEN_SIZE.y / 2.0f);

#define time_bonus(SCORE) ((2.0f / (SCORE)) + .15f)

// Function declarations
void branches_update(int seed);

// Where is the player / branch?
enum class side { LEFT, RIGHT, NONE };

struct branch {
	Sprite sprite;
	side position;
} branches[BRANCHES_N];

// This is where our game starts from
int main()
{
	const struct {
		int x;
		int y;
	} SCREEN_SIZE = {1920, 1080};

#ifdef LOW_RES
	// Low res
	VideoMode vm(960, 540);
	RenderWindow window(vm, "TimTheTimber", Style::Titlebar);
	View view(sf::FloatRect(0, 0, SCREEN_SIZE.x, SCREEN_SIZE.y));
	window.setView(view);
#else
	// Create a video mode object to setup window
	VideoMode va(SCREEN_SIZE.x, SCREEN_SIZE.y);

	// Create and open a window for this game
	RenderWindow window(va, "TimTheTimber", Style::Fullscreen);
#endif

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
	text_screen_center(messageText, SCREEN_SIZE);

	score.text.setPosition(SCORE_POS_X, SCORE_POS_Y);

	// Prepare the branches
	Texture textureBranch;
	textureBranch.loadFromFile(BRANCH_PNG);

	// Set the texture for each branch sprite
	for (struct branch *p = branches + BRANCHES_N; p-- != branches;) {
		Sprite *branch = &p->sprite;

		branch->setTexture(textureBranch);
		branch->setPosition(BRANCH_START_POS_X, BRANCH_START_POS_Y);

		// Set the sprite's position to dead centre
		// We can then spin it round without changing its position
		branch->setOrigin(BRANCH_ORIGIN_X, BRANCH_ORIGIN_Y);
	}

	// Prepare the player
	Texture texturePlayer;
	texturePlayer.loadFromFile(PLAYER_PNG);

	struct {
		Sprite sprite;
		side currentSide;
	} player;

	player.sprite.setTexture(texturePlayer);
	player.sprite.setPosition(PLAYER_POS_X, PLAYER_POS_Y);

	// Player starts on the left
	player.currentSide = side::LEFT;

	// Prepare the gravestone
	Texture textureRip;
	textureRip.loadFromFile(RIP_PNG);
	Sprite spriteRip;
	spriteRip.setTexture(textureRip);
	spriteRip.setPosition(RIP_START_POS_X, RIP_START_POS_Y);

	// Prepare the axe
	Texture textureAxe;
	textureAxe.loadFromFile(AXE_PNG);
	Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(AXE_POS_X, AXE_POS_Y);

	// Prepare the flying log
	Texture textureLog;
	textureLog.loadFromFile(LOG_PNG);

	struct {
		Sprite sprite;
		bool active;
		float speedX;
		float speedY;
	} treeLog;

	treeLog.sprite.setTexture(textureLog);
	treeLog.sprite.setPosition(LOG_POS_X, LOG_POS_Y);

	// Some other useful log related variables
	treeLog.active = false;
	treeLog.speedX = LOG_SPEED_X;
	treeLog.speedY = LOG_SPEED_Y;

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
		float remainingTime;
		float widthPerSecond;
	} timeBar;

	timeBar.shape.setSize(Vector2f(TIME_BAR_START_WIDTH, TIME_BAR_HEIGHT));
	timeBar.shape.setFillColor(Color::Red);
	timeBar.shape.setPosition((SCREEN_SIZE.x / 2.0f) - TIME_BAR_START_WIDTH / 2.0f, SCREEN_SIZE.y);
	timeBar.shape.setPosition(
		(SCREEN_SIZE.x / 2.0f) - TIME_BAR_START_WIDTH / 2.0f, SCREEN_SIZE.y - TIME_BAR_HEIGHT);

	timeBar.remainingTime = GAME_DEFAULT_TIME;
	timeBar.widthPerSecond = TIME_BAR_START_WIDTH / timeBar.remainingTime;

	// Track whether the game is running
	bool paused = true;

	// Control the player input
	bool acceptInput = false;

	while (window.isOpen()) {
		/*
		 ************************************************************
		 * Handle the players input
		 ************************************************************
		 */

		Event event;

		while (window.pollEvent(event)) {
			if (event.type == Event::KeyReleased && !paused) {
				// Listen for the key presses again
				acceptInput = true;

				// hide the axe
				spriteAxe.setPosition(AXE_HIDDEN_POS, spriteAxe.getPosition().y);
			}
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape))
			window.close();

		// Start the game
		if (Keyboard::isKeyPressed(Keyboard::Return)) {
			paused = false;

			// Reset the time and the score
			score.points = 0;
			timeBar.remainingTime = 6;

			// Make the branches disappear - starting in the second position
			for (struct branch *p = branches + (BRANCHES_N - 1); p != branches; p--)
				p->position = side::NONE;

			// Make sure the gravestone is hidden
			spriteRip.setPosition(RIP_HIDDEN_POS_X, RIP_HIDDEN_POS_Y);

			// Move the player into position
			player.sprite.setPosition(PLAYER_START_POS_X, PLAYER_START_POS_Y);

			acceptInput = true;
		}

		// Wrap the player controls to make sure we are accepting input
		if (acceptInput) {
			// First handle pressing the right cursor key
			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				// Make sure the player is on the right
				player.currentSide = side::RIGHT;
				score.points++;

				// Add to the amount of time remaining
				timeBar.remainingTime += time_bonus(score.points);

				spriteAxe.setPosition(AXE_POS_RIGHT, spriteAxe.getPosition().y);

				player.sprite.setPosition(PLAYER_POS_RIGHT, PLAYER_START_POS_Y);

				// Update the branches
				branches_update(score.points);

				// Set the log flying to the left
				treeLog.sprite.setPosition(LOG_POS_X, LOG_POS_Y);
				treeLog.speedX = -LOG_SPEED;
				treeLog.active = true;

				acceptInput = false;
			}

			// Handle the left cursor key
			if (Keyboard::isKeyPressed(Keyboard::Left)) {
				// Make sure the player is on the left
				player.currentSide = side::LEFT;
				score.points++;

				// Add to the amount of time remaining
				timeBar.remainingTime += time_bonus(score.points);

				spriteAxe.setPosition(AXE_POS_LEFT, spriteAxe.getPosition().y);

				player.sprite.setPosition(PLAYER_POS_LEFT, PLAYER_START_POS_Y);

				// Update the branches
				branches_update(score.points);

				// Set the log flying to the left
				treeLog.sprite.setPosition(LOG_POS_X, LOG_POS_Y);
				treeLog.speedX = LOG_SPEED;
				treeLog.active = true;

				acceptInput = false;
			}
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
			timeBar.remainingTime -= delta.asSeconds();
			// Size up the time bar
			timeBar.shape.setSize(
				Vector2f(timeBar.widthPerSecond * timeBar.remainingTime, TIME_BAR_HEIGHT));

			if (timeBar.remainingTime <= 0.0f) {
				// Pause the game
				paused = true;

				// Change the message shown to the player
				messageText.setString("Out of time!!");

				// Reposition the text based on its size
				text_screen_center(messageText, SCREEN_SIZE);
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

			// Update the branch sprites
			for (int i = 0; i < BRANCHES_N; i++) {
				struct branch *branch = &branches[i];
				float height = i * BRANCH_HEIGHT_OFFSET;

				if (branch->position == side::LEFT) {
					// Move the sprite to the left side
					branch->sprite.setPosition(BRANCH_LEFT_POS, height);

					// Flip the sprite round the other way
					branch->sprite.setRotation(180);
				}
				else if (branch->position == side::RIGHT) {
					// Move the sprite to the right side
					branch->sprite.setPosition(BRANCH_RIGHT_POS, height);

					// Set the sprite rotation to normal
					branch->sprite.setRotation(0);
				}
				else {
					// Hide the branch
					branch->sprite.setPosition(BRANCH_HIDE_POS, height);
				}
			}

			// Handle a flying log
			if (treeLog.active) {
				treeLog.sprite.setPosition(
					treeLog.sprite.getPosition().x + (LOG_SPEED_X * delta.asSeconds()),
					treeLog.sprite.getPosition().y + (LOG_SPEED_Y * delta.asSeconds()));

				// Has the log reached the right hand edge?
				if (treeLog.sprite.getPosition().x < LOG_RIGHT_LIM ||
					treeLog.sprite.getPosition().x > LOG_LEFT_LIM) {
					// Set it up ready to be a whole new log next frame
					treeLog.active = false;
					treeLog.sprite.setPosition(LOG_POS_X, LOG_POS_Y);
				}
			}

			// Has the player been squished by a branch?
			if (branches[BRANCHES_N - 1].position == player.currentSide) {
				// death
				paused = true;
				acceptInput = false;

				// Draw the gravestone
				spriteRip.setPosition(RIP_POS_X, RIP_POS_Y);

				// Hide the player
				player.sprite.setPosition(PLAYER_HIDDEN_POS_X, PLAYER_HIDDEN_POS_Y);

				// Change the text of the message
				messageText.setString("SQUISHED!");

				// Center it on screen
				text_screen_center(messageText, SCREEN_SIZE);
			}

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

		// Draw the branches
		for (struct branch *p = branches; p < branches + BRANCHES_N; p++)
			window.draw(p->sprite);

		// Draw the tree
		window.draw(spriteTree);

		// Draw the player
		window.draw(player.sprite);

		// Draw the axe
		window.draw(spriteAxe);

		// Draw the flying log
		window.draw(treeLog.sprite);

		// Draw the gravestone
		window.draw(spriteRip);

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


// Function definition
void branches_update(int seed)
{
	// Move all the branches down one place
	for (struct branch *p = branches + (BRANCHES_N - 1); p > branches; p--)
		p->position = (p - 1)->position;

	// Spawn a new branch at position 0
	// LEFT, RIGHT or NONE
	srand((int)time(0) + seed);
	int r = (rand() % 5);

	switch (r) {
		case 0: branches[0].position = side::LEFT; break;
		case 1: branches[0].position = side::RIGHT; break;
		default: branches[0].position = side::NONE; break;
	}
}
