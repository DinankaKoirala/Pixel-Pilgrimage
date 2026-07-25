#pragma once
#include <SFML/Audio.hpp>

class SoundManager
{
public:
    SoundManager()
        : m_hitSound(m_hitBuffer)
        , m_whooshSound(m_whooshBuffer)
        , mLaughSound(mLaughBuffer)
    {
        m_hitBuffer.loadFromFile("level-5/assets/sounds/hit.wav");
        m_whooshBuffer.loadFromFile("level-5/assets/sounds/whoosh.wav");
        mLaughBuffer.loadFromFile("level-5/assets/sounds/evil-laugh.wav");

        m_whooshSound.setVolume(10.f);
        mLaughSound.setVolume(500.f);

        m_bgm.openFromFile("level-5/assets/sounds/bgm.mp3");
        m_bgm.setLooping(true);
        m_bgm.setVolume(70.f);
        m_bgm.play();

        scheduleNextLaugh();
    }

    void playHit()
    {
        m_hitSound.play();
    }

    void playWhoosh()
    {
        m_whooshSound.play();
    }

    void update(float dt)
    {
        mLaughTimer -= dt;
        if (mLaughTimer <= 0.f)
        {
            mLaughSound.stop();
            mLaughSound.play();
            scheduleNextLaugh();
        }
    }

private:
    sf::SoundBuffer m_hitBuffer;
    sf::SoundBuffer m_whooshBuffer;
    sf::SoundBuffer mLaughBuffer;
    sf::Sound m_hitSound;
    sf::Sound m_whooshSound;
    sf::Sound mLaughSound;
    sf::Music m_bgm;
    float mLaughTimer = 0.f;

    void scheduleNextLaugh()
    {
        mLaughTimer = 8.f;
    }
};
