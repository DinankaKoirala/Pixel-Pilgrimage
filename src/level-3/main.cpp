#include "Game.hpp"
#include "Constants.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode({Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT}), "Rescue the Princess");
    window.setFramerateLimit(60);
    Game game(window);
    game.run();
    return 0;
}
