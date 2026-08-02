#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <algorithm>
#include <cstdint>

// Central, persistent game settings shared across every screen and level.
// The SettingsPanel UI writes into this; the rest of the game reads from it.
struct GameSettings {
    float musicVolume = 0.7f;   // 0..1
    float sfxVolume = 0.8f;     // 0..1
    float brightness = 0.6f;    // 0..1
    float contrast = 0.6f;      // 0..1
    bool fullscreen = false;
    bool showFPS = true;
    bool particles = true;
    int difficulty = 1;         // 0 = Easy, 1 = Medium, 2 = Hard

    static GameSettings& get() {
        static GameSettings instance;
        return instance;
    }

    // --- Difficulty tuning ------------------------------------------------
    // Multiplier applied to enemy speeds / game speeds (0.8x on Easy, 1.3x on Hard).
    float difficultyMultiplier() const {
        return difficulty == 0 ? 0.8f : (difficulty == 2 ? 1.3f : 1.0f);
    }
    // Number of player lives for the given difficulty (base is the Medium value).
    int livesForDifficulty(int base) const {
        if (difficulty == 0) return base + 1;
        if (difficulty == 2) return std::max(1, base - 1);
        return base;
    }
    // Scales a volume so that the default settings keep the original loudness.
    float sfxScale() const { return sfxVolume / 0.8f; }
    float musicScale() const { return musicVolume / 0.7f; }

    // --- Window helpers -----------------------------------------------------
    sf::State windowState() const {
        return fullscreen ? sf::State::Fullscreen : sf::State::Windowed;
    }
    sf::VideoMode windowVideoMode() const {
        if (fullscreen) return sf::VideoMode(sf::VideoMode::getDesktopMode());
        return sf::VideoMode({1280u, 720u});
    }
    // Like windowVideoMode(), but uses the level's native windowed size
    // so content fills the window exactly (no bars) unless fullscreen.
    sf::VideoMode windowVideoModeFor(unsigned w, unsigned h) const {
        if (fullscreen) return sf::VideoMode(sf::VideoMode::getDesktopMode());
        return sf::VideoMode({w, h});
    }
};

// Scales a logical screen of the given size to fit the window (preserving
// aspect ratio), so game content scales up correctly in fullscreen.
inline void applyLetterboxView(sf::RenderWindow& window, float logicalW, float logicalH)
{
    sf::Vector2u ws = window.getSize();
    if (ws.x == 0 || ws.y == 0) return;

    const float scale = std::min(ws.x / logicalW, ws.y / logicalH);
    const float vw = logicalW * scale;
    const float vh = logicalH * scale;

    sf::View view(sf::FloatRect({0.f, 0.f}, {logicalW, logicalH}));
    view.setViewport(sf::FloatRect({ (ws.x - vw) / 2.f / ws.x, (ws.y - vh) / 2.f / ws.y },
                                   { vw / ws.x, vh / ws.y }));
    window.setView(view);
}

// Scales a 1280x720 logical screen to fit the window (preserving aspect ratio),
// so the menu UI scales up correctly in fullscreen.
inline void applyMenuView(sf::RenderWindow& window)
{
    applyLetterboxView(window, 1280.f, 720.f);
}

// Renders global post-processing (brightness/contrast overlay) plus the
// optional FPS counter. Call tick() once per frame, then draw() right
// before window.display().
class SettingsFX {
public:
    static void tick(float dt) {
        static float accum = 0.f;
        static int frames = 0;
        static int fps = 0;
        accum += dt;
        frames++;
        if (accum >= 0.5f) {
            fps = static_cast<int>(frames / accum);
            accum = 0.f;
            frames = 0;
        }
        currentFPS() = fps;
    }

    static void draw(sf::RenderWindow& window) {
        const GameSettings& s = GameSettings::get();
        sf::Vector2u winSize = window.getSize();
        sf::Vector2f size(static_cast<float>(winSize.x), static_cast<float>(winSize.y));

        sf::View prev = window.getView();
        window.setView(window.getDefaultView());

        // Brightness: darken below the default, wash white above it.
        float alphaB = 0.f;
        sf::Color bCol;
        if (s.brightness < 0.6f) {
            alphaB = std::min((0.6f - s.brightness) * 400.f, 200.f);
            bCol = sf::Color::Black;
        } else if (s.brightness > 0.6f) {
            alphaB = std::min((s.brightness - 0.6f) * 350.f, 200.f);
            bCol = sf::Color::White;
        }
        if (alphaB > 1.f) {
            sf::RectangleShape r(size);
            r.setFillColor(sf::Color(bCol.r, bCol.g, bCol.b, static_cast<std::uint8_t>(alphaB)));
            window.draw(r);
        }

        // Contrast: wash toward mid-grey below the default.
        float alphaC = 0.f;
        if (s.contrast < 0.6f) {
            alphaC = std::min((0.6f - s.contrast) * 350.f, 180.f);
            sf::RectangleShape r(size);
            r.setFillColor(sf::Color(128, 128, 128, static_cast<std::uint8_t>(alphaC)));
            window.draw(r);
        }

        if (s.showFPS) {
            static sf::Font font;
            static bool fontReady = false;
            if (!fontReady) {
                if (!font.openFromFile("../src/level-1/assets/fonts/Helvetica.ttf")) {
                    font.openFromFile("C:/Windows/Fonts/arial.ttf");
                }
                fontReady = true;
            }
            sf::Text fpsText(font, std::to_string(currentFPS()), 18);
            fpsText.setFillColor(sf::Color(255, 255, 255, 200));
            fpsText.setOutlineColor(sf::Color::Black);
            fpsText.setOutlineThickness(1.f);
            sf::FloatRect b = fpsText.getLocalBounds();
            fpsText.setPosition({ size.x - b.size.x - 12.f, 12.f });
            window.draw(fpsText);
        }

        window.setView(prev);
    }

private:
    static int& currentFPS() {
        static int fps = 0;
        return fps;
    }
};
