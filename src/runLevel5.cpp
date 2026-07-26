#include "runLevel5.h"
#include "level-5/src/Game.hpp"

bool runLevel5(sf::RenderWindow& window)
{
    L5::Game5 game(window);
    return game.run();
}
