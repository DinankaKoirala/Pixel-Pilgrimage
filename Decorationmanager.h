// DecorationManager.h
// DecorationManager.h
#pragma once
#include <vector>
#include <memory>
#include <string>
#include <SFML/Graphics.hpp>
#include "Decoration.h"

// Owns every background decoration in the scene (hills, trees, clouds, sun, birds).
// Game just calls init() once, then update()/draw() every frame.
class DecorationManager
{
public:
    // Loads textures AND builds the decorations in one call.
    bool init(const std::string& assetsPath, float levelEnd);

    void update(float cameraX);

    void draw(sf::RenderWindow& window);

private:
    bool loadTextures(const std::string& assetsPath);
    void build(float levelEnd);

    sf::Texture hillTex[3];
    sf::Texture treeTex[3];
    sf::Texture cloudTex[3];
    sf::Texture sunTex;
    sf::Texture birdTex;

    std::vector<std::unique_ptr<Decoration>> hills;
    std::vector<std::unique_ptr<Decoration>> trees;
    std::vector<std::unique_ptr<Decoration>> clouds;
    std::vector<std::unique_ptr<Decoration>> sky;  // sun (and later: moon, glow...)
    std::vector<std::unique_ptr<Decoration>> birds; // floating birds, parallax like clouds
};