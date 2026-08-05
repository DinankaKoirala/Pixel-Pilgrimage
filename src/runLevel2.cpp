#include "runLevel2.h"
#include "level-2/src/Game.h"

bool runLevel2(sf::RenderWindow& window)
{
    L2::Game game(window);
    return game.run();
}
