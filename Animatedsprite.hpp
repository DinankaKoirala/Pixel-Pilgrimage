#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <memory>

// ===================== ANIMATION CLIP =====================
// Describes one animation (e.g. "run", "jump") as a strip of frames
// on a single row of a spritesheet.
struct AnimationClip {
    int row = 0;                // which row of the spritesheet this clip lives on
    int frameCount = 1;         // how many frames in this clip
    float frameDuration = 0.1f; // seconds each frame is shown before advancing
    bool loop = true;           // whether it repeats (idle/run) or holds last frame (hit/death)
};

// ===================== ANIMATED SPRITE =====================
// A small reusable helper that cycles through frames of a spritesheet
// on a timer. Designed to fail gracefully: if the texture file can't be
// found, hasTexture() returns false and the owning class (Hero/Monk) is
// expected to fall back to drawing its placeholder shape instead.
//
// Usage:
//   AnimatedSprite anim(32, 48);                 // frame size in pixels
//   anim.loadTexture("assets/hero_sheet.png");
//   anim.addAnimation("idle", { 0, 4, 0.15f, true });
//   anim.addAnimation("run",  { 1, 6, 0.08f, true });
//   anim.play("run");
//   anim.update(dt);
//   anim.draw(window);
class AnimatedSprite {
public:
    AnimatedSprite(int frameWidth, int frameHeight)
        : m_frameWidth(frameWidth), m_frameHeight(frameHeight)
    {
    }

    // Attempts to load the spritesheet. Returns false if the file is missing -
    // callers should keep using their placeholder shape in that case rather
    // than treating this as a fatal error.
    bool loadTexture(const std::string& path) {
        m_textureLoaded = m_texture.loadFromFile(path);
        if (m_textureLoaded) {
            m_sprite = std::make_unique<sf::Sprite>(m_texture);
            m_sprite->setOrigin(sf::Vector2f(m_frameWidth / 2.f, m_frameHeight / 2.f));
        }
        return m_textureLoaded;
    }

    bool hasTexture() const { return m_textureLoaded; }

    void addAnimation(const std::string& name, AnimationClip clip) {
        m_clips[name] = clip;
    }

    // Switch to a new animation. Does nothing if it's already the current
    // one, so calling play("run") every frame while running doesn't restart
    // the animation from frame 0 constantly.
    void play(const std::string& name) {
        if (m_currentName == name) return;

        auto it = m_clips.find(name);
        if (it == m_clips.end()) return; // unknown animation name - ignore safely

        m_currentName = name;
        m_currentFrame = 0;
        m_timer = 0.f;
        updateTextureRect();
    }

    void update(float dt) {
        if (!m_textureLoaded) return;

        auto it = m_clips.find(m_currentName);
        if (it == m_clips.end()) return;
        AnimationClip& clip = it->second;

        m_timer += dt;
        if (m_timer >= clip.frameDuration) {
            m_timer -= clip.frameDuration;
            m_currentFrame++;

            if (m_currentFrame >= clip.frameCount) {
                m_currentFrame = clip.loop ? 0 : clip.frameCount - 1;
            }
            updateTextureRect();
        }
    }

    void setPosition(sf::Vector2f pos) {
        if (m_textureLoaded)
            m_sprite->setPosition(pos);
    }

    // Flips the sprite horizontally to face left/right - much cheaper than
    // storing separate left-facing and right-facing frames in the spritesheet.
    void setFacingRight(bool facingRight) {
        if (!m_textureLoaded) return;
        float scaleX = facingRight ? 1.f : -1.f;
        m_sprite->setScale(sf::Vector2f(scaleX, 1.f));
    }

    void draw(sf::RenderWindow& window) const {
        if (m_textureLoaded)
            window.draw(*m_sprite);
    }

    void setColor(sf::Color color) {
        if (m_textureLoaded)
            m_sprite->setColor(color);
    }

private:
    void updateTextureRect() {
        auto it = m_clips.find(m_currentName);
        if (it == m_clips.end()) return;

        int x = m_currentFrame * m_frameWidth;
        int y = it->second.row * m_frameHeight;
        m_sprite->setTextureRect(sf::IntRect(
            sf::Vector2i(x, y),
            sf::Vector2i(m_frameWidth, m_frameHeight)
        ));
    }

    sf::Texture m_texture;
    std::unique_ptr<sf::Sprite> m_sprite;
    bool m_textureLoaded = false;

    int m_frameWidth;
    int m_frameHeight;

    std::unordered_map<std::string, AnimationClip> m_clips;
    std::string m_currentName;
    int m_currentFrame = 0;
    float m_timer = 0.f;
};