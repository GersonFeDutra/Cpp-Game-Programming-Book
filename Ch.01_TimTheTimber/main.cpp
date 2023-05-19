// Include SFML library APIs
#include <SFML/Graphics.hpp>

// Make code easier to type with "using namespace"
using namespace sf;

// Resources paths
#define BACKGROUND_PNG "assets/graphics/background.png"

// This is where our game starts from
int main()
{
    // Create a video mode object to setup window
    VideoMode va(1920, 1080);

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
    
    // Set the spriteBackground to cover teh screen
    spriteBackground.setPosition(0, 0);

    /*
     ************************************************************
     * Main Loop
     ************************************************************
     */

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
        
        /*
         ************************************************************
         * Draw the scene
         ************************************************************
         */

        // Clear everything from the last frame
        window.clear();

        // Draw our game scene here
        window.draw(spriteBackground);
        
        // Show everything we just draw
        window.display();

    }

    return 0;
}
