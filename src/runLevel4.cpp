#include "runLevel4.h"
#include "level-4/src/Game.hpp"

bool runLevel4(sf::RenderWindow& window)
{
    L4::Game4 game(window);
    return game.run();
}
