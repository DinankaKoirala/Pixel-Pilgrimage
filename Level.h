// Level.h
#pragma once
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Platform.h"
#include "Player.h"
#include "NinjaStar.h"
#include "StarSpawner.h"
#include "Coin.h"

// Builds a fresh row of platforms (with cracked platforms mixed in),
// sprinkles a coin above every Nth regular platform, and registers the
// cracked platforms' ninja-star spawn points.
void createLevel(
    std::vector<std::unique_ptr<Platform>>& platforms,
    std::vector<std::unique_ptr<Coin>>& coins,
    StarSpawner& starSpawner,
    const sf::Texture& blockTex,
    const sf::Texture& crackedTex,
    const sf::Texture& coinTex);

// Resets the player to the start and rebuilds the level from scratch.
void restartGame(
    Player& player,
    std::vector<std::unique_ptr<Platform>>& platforms,
    std::vector<std::unique_ptr<NinjaStar>>& ninjaStars,//pass by reference with reference uses original vector 
    std::vector<std::unique_ptr<Coin>>& coins,
    StarSpawner& starSpawner,
    bool& gameOver,
    const sf::Texture& blockTex,
    const sf::Texture& crackedTex,
    const sf::Texture& coinTex);