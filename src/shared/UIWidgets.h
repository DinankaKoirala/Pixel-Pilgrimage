#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <algorithm>
#include <iostream>
#include "GameSettings.h"
#include "MenuMusic.h"

class Button {
public:
    sf::RectangleShape box;
    sf::Text label;
    sf::Color normal;
    sf::Color hover;

    Button(float x, float y, float w, float h,
        const std::string& text, sf::Font& font,
        sf::Color nc = sf::Color(110, 75, 45),
        sf::Color hc = sf::Color(160, 110, 55))
        : normal(nc), hover(hc), label(font, text)
    {
        box.setPosition({ x, y });
        box.setSize({ w, h });
        box.setFillColor(nc);
        box.setOutlineColor(sf::Color(245, 222, 179));
        box.setOutlineThickness(3);

        label.setCharacterSize(26);
        label.setFillColor(sf::Color(255, 240, 180));
        sf::FloatRect tb = label.getLocalBounds();
        label.setOrigin({ tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f });
        label.setPosition({ x + w / 2.f, y + h / 2.f });
    }

    bool contains(sf::Vector2f p) {
        return box.getGlobalBounds().contains(p);
    }

    void draw(sf::RenderWindow& w, sf::Vector2f mouse) {
        box.setFillColor(contains(mouse) ? hover : normal);
        w.draw(box);
        w.draw(label);
    }
};

class Slider {
public:
    sf::RectangleShape track;
    sf::RectangleShape thumb;
    sf::Text label;
    float value;
    float x, y, w;

    Slider(float px, float py, float pw,
        const std::string& lbl, sf::Font& font,
        float val = 0.7f)
        : label(font, lbl), value(val), x(px), y(py), w(pw)
    {
        track.setPosition({ px, py + 10.f });
        track.setSize({ pw, 6.f });
        track.setFillColor(sf::Color(80, 80, 80));
        track.setOutlineColor(sf::Color::White);
        track.setOutlineThickness(1);

        thumb.setSize({ 14.f, 20.f });
        thumb.setFillColor(sf::Color(100, 200, 255));
        thumb.setOutlineColor(sf::Color::White);
        thumb.setOutlineThickness(1);
        updateThumb();

        label.setCharacterSize(13);
        label.setFillColor(sf::Color::White);
        label.setPosition({ px, py - 18.f });
    }

    void updateThumb() {
        thumb.setPosition({ x + value * (w - 14.f), y + 3.f });
    }

    bool drag(sf::Vector2f mouse, bool pressed) {
        if (pressed && track.getGlobalBounds().contains(mouse)) {
            value = std::max(0.f, std::min(1.f, (mouse.x - x) / (w - 14.f)));
            updateThumb();
            return true;
        }
        return false;
    }

    void draw(sf::RenderWindow& win) {
        win.draw(track);
        win.draw(thumb);
        win.draw(label);
    }
};

class Toggle {
public:
    sf::RectangleShape box;
    sf::Text label;
    bool on;

    Toggle(float px, float py,
        const std::string& lbl, sf::Font& font,
        bool initial = true)
        : on(initial), label(font, lbl)
    {
        box.setPosition({ px, py });
        box.setSize({ 20.f, 20.f });
        box.setOutlineColor(sf::Color::White);
        box.setOutlineThickness(2);

        label.setCharacterSize(13);
        label.setFillColor(sf::Color::White);
        label.setPosition({ px + 28.f, py });
    }

    void click(sf::Vector2f p) {
        if (box.getGlobalBounds().contains(p))
            on = !on;
    }

    void draw(sf::RenderWindow& win) {
        box.setFillColor(on ? sf::Color(80, 200, 80) : sf::Color(60, 60, 60));
        win.draw(box);
        win.draw(label);
    }
};

struct SettingsPanel {
    bool visible;
    sf::RectangleShape panel;
    sf::Text title;
    Slider musicVol;
    Slider sfxVol;
    Slider brightness;
    Slider contrast;
    Toggle fullscreen;
    Toggle showFPS;
    Toggle particles;
    sf::Text diffLabel;
    Button btnEasy;
    Button btnMed;
    Button btnHard;
    Button btnClose;

    SettingsPanel(sf::Font& font)
        : visible(false),
        title(font, "SETTINGS"),
        diffLabel(font, "Difficulty:"),
        musicVol(312.f + 50.f, 184.f + 80.f, 300.f, "Music Volume", font, GameSettings::get().musicVolume),
        sfxVol(312.f + 50.f, 184.f + 140.f, 300.f, "SFX Volume", font, GameSettings::get().sfxVolume),
        brightness(312.f + 50.f, 184.f + 200.f, 300.f, "Brightness", font, GameSettings::get().brightness),
        contrast(312.f + 50.f, 184.f + 250.f, 300.f, "Contrast", font, GameSettings::get().contrast),
        fullscreen(312.f + 50.f, 184.f + 270.f, "Fullscreen", font, GameSettings::get().fullscreen),
        showFPS(312.f + 50.f, 184.f + 300.f, "Show FPS", font, GameSettings::get().showFPS),
        particles(312.f + 50.f, 184.f + 330.f, "Particles", font, GameSettings::get().particles),
        btnEasy(312.f + 150.f, 184.f + 355.f, 60.f, 24.f, "Easy", font, sf::Color(40, 120, 40), sf::Color(60, 180, 60)),
        btnMed(312.f + 220.f, 184.f + 355.f, 60.f, 24.f, "Medium", font, sf::Color(120, 100, 20), sf::Color(200, 160, 30)),
        btnHard(312.f + 290.f, 184.f + 355.f, 60.f, 24.f, "Hard", font, sf::Color(120, 30, 30), sf::Color(200, 50, 50)),
        btnClose(312.f + 150.f, 184.f + 395.f, 100.f, 28.f, "CLOSE", font, sf::Color(80, 30, 30), sf::Color(160, 50, 50))
    {
        panel.setPosition({ 312.f, 184.f });
        panel.setSize({ 400.f, 450.f });
        panel.setFillColor(sf::Color(20, 20, 40, 230));
        panel.setOutlineColor(sf::Color(180, 140, 60));
        panel.setOutlineThickness(3);

        title.setCharacterSize(22);
        title.setFillColor(sf::Color(255, 220, 80));
        float tw = title.getLocalBounds().size.x;
        title.setPosition({ 312.f + 200.f - tw / 2.f, 184.f + 14.f });

        diffLabel.setCharacterSize(13);
        diffLabel.setFillColor(sf::Color::White);
        diffLabel.setPosition({ 312.f + 50.f, 184.f + 360.f });
    }

    void handleClick(sf::Vector2f mp) {
        fullscreen.click(mp);
        showFPS.click(mp);
        particles.click(mp);

        if (btnEasy.contains(mp)) GameSettings::get().difficulty = 0;
        if (btnMed.contains(mp))  GameSettings::get().difficulty = 1;
        if (btnHard.contains(mp)) GameSettings::get().difficulty = 2;
        if (btnClose.contains(mp)) {
            visible = false;
        }

        GameSettings& g = GameSettings::get();
        g.fullscreen = fullscreen.on;
        g.showFPS = showFPS.on;
        g.particles = particles.on;
    }

    void handleDrag(sf::Vector2f mouse, bool pressed) {
        musicVol.drag(mouse, pressed);
        sfxVol.drag(mouse, pressed);
        brightness.drag(mouse, pressed);
        contrast.drag(mouse, pressed);

        GameSettings& g = GameSettings::get();
        g.musicVolume = musicVol.value;
        g.sfxVolume = sfxVol.value;
        g.brightness = brightness.value;
        g.contrast = contrast.value;

        MenuMusic::updateVolume();
    }

    void draw(sf::RenderWindow& win, sf::Vector2f mouse) {
        int diff = GameSettings::get().difficulty;
        btnEasy.normal = (diff == 0) ? sf::Color(60, 200, 60) : sf::Color(40, 120, 40);
        btnMed.normal  = (diff == 1) ? sf::Color(240, 200, 40) : sf::Color(120, 100, 20);
        btnHard.normal = (diff == 2) ? sf::Color(240, 60, 60) : sf::Color(120, 30, 30);

        win.draw(panel);
        win.draw(title);
        musicVol.draw(win);
        sfxVol.draw(win);
        brightness.draw(win);
        contrast.draw(win);
        fullscreen.draw(win);
        showFPS.draw(win);
        particles.draw(win);
        win.draw(diffLabel);
        btnEasy.draw(win, mouse);
        btnMed.draw(win, mouse);
        btnHard.draw(win, mouse);
        btnClose.draw(win, mouse);
    }
};
