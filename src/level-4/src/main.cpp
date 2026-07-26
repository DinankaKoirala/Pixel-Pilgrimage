#include "Game.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode({(unsigned)L4::SW, (unsigned)L4::SH}), "Winter Journey");
    window.setFramerateLimit(60);
    L4::Game4 game(window);
    game.run();
    return 0;
}
