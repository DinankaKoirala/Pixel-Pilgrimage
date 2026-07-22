#pragma once
#include <SFML/Graphics.hpp>

namespace Constants {
    inline const unsigned int WINDOW_WIDTH = 1306;
    inline const unsigned int WINDOW_HEIGHT = 673;

    // The stone path in the background image: from the bottom-left area
    // near the cherry blossom tree, up to the temple entrance top-right.
    inline const sf::Vector2f SLOPE_START(230.f, 610.f);
    inline const sf::Vector2f SLOPE_END(1120.f, 250.f);
}
