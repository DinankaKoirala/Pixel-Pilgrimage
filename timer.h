#pragma once

#include <algorithm>

class Timer
{
public:
    explicit Timer(float duration = 20.f) : m_duration(duration) {}

    void start()
    {
        m_elapsed = 0.f;
        m_running = true;
    }

    void update(float dt)
    {
        if (m_running)
        {
            m_elapsed += dt;
            if (m_elapsed >= m_duration)
                m_running = false;
        }
    }

    bool isExpired() const { return m_elapsed >= m_duration; }

    bool isRunning() const { return m_running; }

    float getRemaining() const { return std::max(0.f, m_duration - m_elapsed); }

    float getElapsed() const { return m_elapsed; }

    void reset()
    {
        m_elapsed = 0.f;
        m_running = false;
    }

private:
    float m_duration;
    float m_elapsed = 0.f;
    bool m_running = false;
};
