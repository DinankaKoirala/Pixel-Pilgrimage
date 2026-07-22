#include "Header/audioManager.h"
#include <iostream>

void AudioManager::loadSFX(const std::string& name, const std::string& path){
    sf::SoundBuffer buffer;
    if(!buffer.loadFromFile(path)){
        std::cerr<<"Failed to load "<<name<<std::endl;
        return;
    }
    sfx_buffers[name] = std::move(buffer);
    sfx_sounds[name] = std::make_unique<sf::Sound>(sfx_buffers[name]);
}

void AudioManager::playSFX(const std::string& name){
    auto playing = sfx_sounds.find(name);
    if(playing == sfx_sounds.end()){
        std::cerr<<name<<" doesnt exist"<<std::endl;
        return;
    }
    playing->second->setVolume(100);
    playing->second->play();
}

void AudioManager::loadMusic(const std::string& path){
    if(!music.openFromFile(path)){
        std::cerr<<"Failed to load music"<<std::endl;
        return;
    }
    music.setLooping(true);
    music.setVolume(50); 
}

void AudioManager::playMusic(){
    music.play();
}

void AudioManager::stopMusic(){
    music.stop();
}

void AudioManager::setMusicLoop(bool loop){
    music.setLooping(loop); 
}

void AudioManager::playSFXIfNotPlaying(const std::string& name){
    auto playing = sfx_sounds.find(name);
    if(playing == sfx_sounds.end()){
        std::cerr<<name<<" doesnt exist"<<std::endl;
        return;
    }
    if(playing->second->getStatus() == sf::Sound::Status::Playing){
        return;
    }
    else {
        playing->second->setVolume(100);
        playing->second->play();
    }
}