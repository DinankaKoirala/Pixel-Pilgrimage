// DecorationManager.h
// DecorationManager.h
#pragma once
#include <vector>
#include <memory>
#include <string>
#include <SFML/Graphics.hpp>
#include "Decoration.h"
#include "AnimatedDecoration.h"

// Owns every background decoration in the scene (hills, trees, clouds, sun,
// birds, butterflies, foreground grass).
// Game just calls init() once, then update()/draw() every frame.
class DecorationManager
{
public:
    // Loads textures AND builds the decorations in one call.
    bool init(const std::string& assetsPath, float levelEnd);

    // dt drives the animated decorations (sway, bob, drift, loop);
    // cameraX drives parallax positioning, same as before.
    void update(float cameraX, float dt);

    // Background layers - draw BEFORE platforms/player.
    void draw(sf::RenderWindow& window);

    // Foreground layer (grass) - draw AFTER the player, so it renders in front.
    void drawForeground(sf::RenderWindow& window);

private:
    bool loadTextures(const std::string& assetsPath);
    void build(float levelEnd);

    sf::Texture hillTex[3];
    sf::Texture treeTex[3];
    sf::Texture cloudTex[3];
    sf::Texture sunTex;
    sf::Texture birdTex;
    sf::Texture butterflyTex;
    sf::Texture grassTex;

    std::vector<std::unique_ptr<Decoration>> hills;
    std::vector<std::unique_ptr<Decoration>> trees;   // SwayingTree instances
    std::vector<std::unique_ptr<Decoration>> clouds;  // DriftingCloud instances
    std::vector<std::unique_ptr<Decoration>> sky;     // sun (and later: moon, glow...) - static
    std::vector<std::unique_ptr<Decoration>> birds;   // FlappingBird instances
    std::vector<std::unique_ptr<Decoration>> butterflies;
    std::vector<std::unique_ptr<Decoration>> grass;   // foreground layer, drawn after the player
};