#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

namespace Utility {

    // Tries to load a texture from disk. If the file doesn't exist yet
    // (useful while you're still drawing the remaining art), this creates
    // a small colored square instead, so the game keeps running rather
    // than crashing.
    inline sf::Texture loadTextureOrPlaceholder(const std::string& path, sf::Color fallbackColor) {
        sf::Texture texture;
        if (texture.loadFromFile(path)) {
            return texture;
        }

        std::cerr << "[Warning] Could not load \"" << path
                  << "\" - using a placeholder color instead.\n";

        sf::Image image;
        image.create(64, 64, fallbackColor);
        texture.loadFromImage(image);
        return texture;
    }

}
