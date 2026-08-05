#pragma once
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>

class AudioManager {
public:
    void loadSFX(const std::string& name, const std::string& path);
    void playSFX(const std::string& name);

    void loadMusic(const std::string& path);
    void playMusic();
    void stopMusic();
    void setMusicLoop(bool loop);
    void playSFXIfNotPlaying(const std::string& name);

private:
    std::unordered_map<std::string, sf::SoundBuffer> sfx_buffers;
    std::unordered_map<std::string, std::unique_ptr<sf::Sound>> sfx_sounds;
    sf::Music music;
};
