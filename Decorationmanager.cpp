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
    const std::string trunkFiles[3] = { "trunk1.png", "trunk2.png", "trunk3.png" };
    const std::string cloudFiles[3] = { "cloud1.png", "cloud2.png", "cloud3.png" };

    for (int i = 0; i < 3; ++i)
    {
        if (!hillTex[i].loadFromFile(assetsPath + hillFiles[i]))   return false;
        if (!treeTex[i].loadFromFile(assetsPath + treeFiles[i]))   return false;
        if (!trunkTex[i].loadFromFile(assetsPath + trunkFiles[i])) return false;
        if (!cloudTex[i].loadFromFile(assetsPath + cloudFiles[i])) return false;
    }

    if (!grassTex.loadFromFile(assetsPath + "grass.png")) return false;

    if (!sunTex.loadFromFile(assetsPath + "sun.png"))   return false;
    if (!birdTex.loadFromFile(assetsPath + "bird.png")) return false;

    return true;
}

void DecorationManager::build(float levelEnd)
{
    hills.clear();
    trunks.clear();
    grass.clear();
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
        trees.push_back(std::make_unique<SwayingTree>(
            treeTex[variant], sf::Vector2f(tx, treeGroundY), 0.55f));

        // Bottom-anchored at groundY (the platform's top surface, y=600) -
        // NOT beyond it, so it bridges the gap up to the tree above without
        // ever dipping into/overlapping the wooden platform blocks.
        // Same variant index as its tree, so the width always matches.
        // scaleX = 0.5f narrows the trunk to half its native width while
        // keeping full height - tweak between ~0.4-0.6 to match your tree art.
        trunks.push_back(std::make_unique<Decoration>(
            trunkTex[variant], sf::Vector2f(tx, groundY), 0.55f,
            /*centerOrigin=*/false, /*scale=*/1.f, /*scaleX=*/0.5f));
    }

    // Continuous strip of ground grass, tiled across the whole level like the
    // sky background is in Game::render(). parallaxFactor = 1.f (NOT the
    // 0.55f used by trees/trunks) so it scrolls in perfect lockstep with the
    // platforms/trunks instead of slowly drifting apart from them over a long
    // level. Bottom-anchored at groundY, same as the trunks/platforms, so its
    // bottom edge sits flush with the platform top and never overlaps it -
    // it only grows upward from there, burying the trunk bases.
    //
    // grassTileWidth is a guess at grass.png's width - tune it (and scaleX
    // below) once you have the real asset so tiles butt up cleanly instead of
    // leaving gaps or over-stacking. Deliberately overlapping tiles slightly
    // (scaleX > 1) hides minor width mismatches so it still reads as one
    // continuous band of grass even before that's dialed in.
    const float grassTileWidth = 128.f;
    for (float gx = -500.f; gx < levelEnd; gx += grassTileWidth)
    {
        grass.push_back(std::make_unique<Decoration>(
            grassTex, sf::Vector2f(gx, groundY), 1.f,
            /*centerOrigin=*/false, /*scale=*/1.15f, /*scaleX=*/1.1f));
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
        clouds.push_back(std::make_unique<DriftingCloud>(
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
        birds.push_back(std::make_unique<FlappingBird>(
            birdTex, sf::Vector2f(bx, by), 0.2f, 1.f));
        birdSlot++;
    }
}

void DecorationManager::update(float cameraX, float dt)
{
    // animate() first (sway/bob/drift/loop set animOffset & animRotationDeg),
    // then updateParallax() applies both the camera-driven position AND
    // whatever animate() just computed.
    for (auto& s : sky) { s->animate(dt); s->updateParallax(cameraX); }
    for (auto& b : birds) { b->animate(dt); b->updateParallax(cameraX); }
    for (auto& c : clouds) { c->animate(dt); c->updateParallax(cameraX); }
    for (auto& h : hills) { h->animate(dt); h->updateParallax(cameraX); }
    for (auto& u : trunks) { u->animate(dt); u->updateParallax(cameraX); }
    for (auto& g : grass) { g->animate(dt); g->updateParallax(cameraX); }
    for (auto& t : trees) { t->animate(dt); t->updateParallax(cameraX); }
}

void DecorationManager::draw(sf::RenderWindow& window)
{
    for (auto& s : sky)    s->draw(window);
    for (auto& b : birds)  b->draw(window);
    for (auto& c : clouds) c->draw(window);
    for (auto& h : hills)  h->draw(window);
    for (auto& u : trunks) u->draw(window);
    for (auto& g : grass)  g->draw(window);
    for (auto& t : trees)  t->draw(window);
}