#include "runLevel3.h"
#include "level-3/Game.hpp"

bool runLevel3(sf::RenderWindow& window)
{
    Game game(window);
    return game.run();
}
