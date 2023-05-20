// Include SFML library APIs
#include <SFML/Graphics.hpp>

// Make code easier to type with "using namespace"
using namespace sf;

// Resources paths
#define GRAPHICS_DIR "assets/graphics/"
#define BACKGROUND_PNG GRAPHICS_DIR "background.png"
#define TREE_PNG GRAPHICS_DIR "tree.png"
#define BEE_PNG GRAPHICS_DIR "bee.png"
#define CLOUD_PNG GRAPHICS_DIR "cloud.png"

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

// This is where our game starts from
int main()
{
    const struct {
        int x;
        int y;
    } SCREEN_SIZE = { 1920, 1080 };

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
    Sprite spriteBee;
    spriteBee.setTexture(textureBee);
    spriteBee.setPosition(BEE_COORDS_X, BEE_COORDS_Y);

    // Is the bee currently moving?
    bool beeActive = false;

    // How fast can the bee fly
    float beeSpeed = 0.0f;

    // Make 3 cloud sprites from 1 texture
    Texture textureCloud;

    textureCloud.loadFromFile(CLOUD_PNG);

    struct Cloud {
        Sprite sprite;
        bool active; // Is the cloud currently on the screen?
        float speed; // How fast is the cloud?
    } clouds[CLOUDS_N]; // 3 new sprites with the same texture

    for (int i = 0, offset = 0; i <= CLOUDS_N; i++, offset += CLOUD_OFFSET_Y) {
        Sprite *cloud = &clouds[i].sprite;
        cloud->setTexture(textureCloud);

        // Position the clouds on the left of the screen at different heights
        cloud->setPosition(CLOUD_COORDS_X, offset);

        clouds[i].active = false;
        clouds[i].speed = 0.0f;
    }

    /*
     ************************************************************
     * Main Loop
     ************************************************************
     */

    // Variable to control time itself
    Clock clock;

    while (window.isOpen()) {
        /*
         ************************************************************
         * Handle the players input
         ************************************************************
         */

        if (Keyboard::isKeyPressed(Keyboard::Escape))
            window.close();
        
        
        /*
         ************************************************************
         * Update the scene
         ************************************************************
         */

        // Measure time
        Time delta = clock.restart();

        // Setup the bee
        if (!beeActive) {
            // How fast is the bee?
            srand((int)time(0));
            beeSpeed = (rand() % BEE_BASE_SPEED) + BEE_BASE_SPEED;

            // How high is the bee?
            srand((int)time(0) * 10);
            float height = (rand() % BEE_BASE_HEIGHT) + BEE_BASE_HEIGHT;
            spriteBee.setPosition(BEE_START_POS_X, height);

            beeActive = true;
        }
        else {
            // Move the bee
            spriteBee.setPosition(
                spriteBee.getPosition().x - (beeSpeed * delta.asSeconds()),
                spriteBee.getPosition().y
            );

            // Has the bee reached the left-hand edge of the screen?
            if (spriteBee.getPosition().x < -100)
                beeActive = false; // Set it up ready to be a whole new bee next frame
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
                    cloud->sprite.getPosition().y
                );

                // Has the cloud reached the right hand edge of the screen?
                if (cloud->sprite.getPosition().x > SCREEN_SIZE.x)
                    cloud->active = false; // Set it up ready to be a whole new cloud next frame
            }
        }

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
        window.draw(spriteBee);
        
        // Show everything we just draw
        window.display();

    }

    return 0;
}
