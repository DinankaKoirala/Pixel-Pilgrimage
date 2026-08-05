#include "Game.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 1280u, 720u }), "Bridge Level");
    window.setFramerateLimit(60);
    L2::Game game(window);
    game.run();
    return 0;
}
