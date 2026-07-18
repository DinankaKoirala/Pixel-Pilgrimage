// DecorationManager.cpp
#include "DecorationManager.h"
#include <cstdlib>
#include <cmath>

bool DecorationManager::init(const std::string& assetsPath, float levelEnd)
{
    if (!loadTextures(assetsPath)) return false;
    build(levelEnd);
    return true;
}

bool DecorationManager::loadTextures(const std::string& assetsPath)
{
    const std::string hillFiles[3] = { "hill1.png",  "hill2.png",  "hill3.png" };
    const std::string treeFiles[3] = { "tree1.png",  "tree2.png",  "tree3.png" };
    const std::string cloudFiles[3] = { "cloud1.png", "cloud2.png", "cloud3.png" };

    for (int i = 0; i < 3; ++i)
    {
        if (!hillTex[i].loadFromFile(assetsPath + hillFiles[i]))   return false;
        if (!treeTex[i].loadFromFile(assetsPath + treeFiles[i]))   return false;
        if (!cloudTex[i].loadFromFile(assetsPath + cloudFiles[i])) return false;
    }

    if (!sunTex.loadFromFile(assetsPath + "sun.png"))   return false;
    if (!birdTex.loadFromFile(assetsPath + "bird.png")) return false;

    return true;
}

void DecorationManager::build(float levelEnd)
{
    hills.clear();
    trees.clear();
    clouds.clear();
    sky.clear();
    birds.clear();

    const float groundY = 600.f;
    const float hillGroundY = 470.f;
    const float treeGroundY = groundY - 130.f;

    for (float hx = -500.f; hx < levelEnd; hx += 500.f)
    {
        int variant = rand() % 3;
        hills.push_back(std::make_unique<Decoration>(
            hillTex[variant], sf::Vector2f(hx, hillGroundY), 0.25f));
    }

    for (float tx = -450.f; tx < levelEnd; tx += 260.f)
    {
        int variant = rand() % 3;
        trees.push_back(std::make_unique<Decoration>(
            treeTex[variant], sf::Vector2f(tx, treeGroundY), 0.55f));
    }

    const float sunX = -540.f; // must match the sun's position below

    const float cloudHeights[3] = { 80.f, 130.f, 180.f };
    int cloudSlot = 0;
    for (float cx = -500.f; cx < levelEnd; cx += 400.f)
    {
        // The sun sits at a fixed screen position (its parallax factor is 0),
        // so a cloud spawned right next to it in world space would render
        // right on top of it. Skip that slot so they don't overlap.
        if (std::fabs(cx - sunX) < 300.f)
        {
            cloudSlot++;
            continue;
        }

        int variant = rand() % 3;
        float cy = cloudHeights[cloudSlot % 3];
        clouds.push_back(std::make_unique<Decoration>(
            cloudTex[variant], sf::Vector2f(cx, cy), 0.1f));
        cloudSlot++;
    }

    sky.push_back(std::make_unique<Decoration>(
        sunTex, sf::Vector2f(sunX, 70.f), 0.f, true, 0.6f));

    // Birds: scattered across the sky like clouds, at varied heights,
    // slightly faster parallax than clouds so they feel a bit closer.
    const float birdHeights[4] = { 100.f, 150.f, 200.f, 250.f };
    int birdSlot = 0;
    for (float bx = -350.f; bx < levelEnd; bx += 350.f)
    {
        float by = birdHeights[birdSlot % 4];
        birds.push_back(std::make_unique<Decoration>(
            birdTex, sf::Vector2f(bx, by), 0.2f, true, 1.f));
        birdSlot++;
    }
}

void DecorationManager::update(float cameraX)
{
    for (auto& s : sky)    s->updateParallax(cameraX);
    for (auto& b : birds)  b->updateParallax(cameraX);
    for (auto& c : clouds) c->updateParallax(cameraX);
    for (auto& h : hills)  h->updateParallax(cameraX);
    for (auto& t : trees)  t->updateParallax(cameraX);
}

void DecorationManager::draw(sf::RenderWindow& window)
{
    for (auto& s : sky)    s->draw(window);
    for (auto& b : birds)  b->draw(window);
    for (auto& c : clouds) c->draw(window);
    for (auto& h : hills)  h->draw(window);
    for (auto& t : trees)  t->draw(window);
}