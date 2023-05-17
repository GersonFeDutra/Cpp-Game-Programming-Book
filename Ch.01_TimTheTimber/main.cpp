// Include SFML library APIs
#include <SFML/Graphics.hpp>

// Make code easier to type with "using namespace"
using namespace sf;

// This is where our game starts from
int main()
{
    // Create a video mode object to setup window
    VideoMode va(1920, 1080);

    // Create and open a window for this game
    RenderWindow window(va, "TimTheTimber", Style::Fullscreen);

    return 0;
}
