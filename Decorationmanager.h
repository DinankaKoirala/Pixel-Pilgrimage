// DecorationManager.h
#pragma once
#include <vector>
#include <memory>
#include <string>
#include <SFML/Graphics.hpp>
#include "Decoration.h"
#include "AnimatedDecoration.h"

// Owns every decoration in the scene (hills, trees, clouds, sun, birds).
// Game just calls init() once, then update()/draw() every frame.
class DecorationManager
{
public:
    // Loads textures AND builds the decorations in one call.
    bool init(const std::string& assetsPath, float levelEnd);

    // dt drives the animated decorations (sway, bob, drift, loop);
    // cameraX drives parallax positioning, same as before.
    void update(float cameraX, float dt);

    // Background layers (sky/birds/clouds/hills/trees) - draw BEFORE
    // platforms/player.
    void draw(sf::RenderWindow& window);

private:
    bool loadTextures(const std::string& assetsPath);
    void build(float levelEnd);

    sf::Texture hillTex[3];
    sf::Texture treeTex[3];
    sf::Texture trunkTex[3]; // matched 1:1 with treeTex - same index = same variant
    sf::Texture grassTex;    // single tile, repeated to form a continuous strip of ground grass
    sf::Texture cloudTex[3];
    sf::Texture sunTex;
    sf::Texture birdTex;

    std::vector<std::unique_ptr<Decoration>> hills;
    std::vector<std::unique_ptr<Decoration>> trunks;  // one per tree, matched to that tree's variant/width
    std::vector<std::unique_ptr<Decoration>> grass;   // one continuous tiled strip along the whole level, bottom-anchored flush with the platform top so trunks/platforms look planted in it, not floating
    std::vector<std::unique_ptr<Decoration>> trees;   // SwayingTree instances
    std::vector<std::unique_ptr<Decoration>> clouds;  // DriftingCloud instances
    std::vector<std::unique_ptr<Decoration>> sky;     // sun (and later: moon, glow...) - static
    std::vector<std::unique_ptr<Decoration>> birds;   // FlappingBird instances
};