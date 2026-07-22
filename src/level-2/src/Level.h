// Level.h
#pragma once
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Platform.h"
#include "Player.h"
#include "NinjaStar.h"
#include "StarSpawner.h"

// Builds a fresh row of platforms (with cracked platforms mixed in) and
// registers their ninja-star spawn points.
void createLevel(
    std::vector<std::unique_ptr<Platform>>& platforms,
    StarSpawner& starSpawner,
    const sf::Texture& blockTex,
    const sf::Texture& crackedTex);

// Resets the player to the start and rebuilds the level from scratch.
void restartGame(
    Player& player,
    std::vector<std::unique_ptr<Platform>>& platforms,
    std::vector<std::unique_ptr<NinjaStar>>& ninjaStars,
    StarSpawner& starSpawner,
    bool& gameOver,
    const sf::Texture& blockTex,
    const sf::Texture& crackedTex);