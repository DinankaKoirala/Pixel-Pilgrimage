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
        m_hitBuffer.loadFromFile("Data/hit.wav");
        m_whooshBuffer.loadFromFile("Data/whoosh.wav");
        mLaughBuffer.loadFromFile("Data/evil-laugh.wav");

        m_whooshSound.setVolume(50.f);
        mLaughSound.setVolume(500.f);

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
    float mLaughTimer = 0.f;

    void scheduleNextLaugh()
    {
        mLaughTimer = 8.f;
    }
};
