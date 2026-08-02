#pragma once
#include <SFML/Audio.hpp>
#include <vector>
#include <cmath>
#include <cstdint>
#include "GameSettings.h"

// Original, procedurally-synthesized chiptune loop for the menu screens.
// Dreamy "Adventure Time"-style pixel vibe: soft pulse lead over a
// C -> Am -> F -> G progression with a triangle bass and warm pad.
namespace MenuMusic
{
    namespace detail
    {
        inline sf::SoundBuffer& buffer() { static sf::SoundBuffer b; return b; }
        inline sf::Sound& sound()        { static sf::Sound s{detail::buffer()}; return s; }
        inline bool& loaded()            { static bool l = false;  return l; }
    }

    inline void updateVolume()
    {
        detail::sound().setVolume(32.f * GameSettings::get().musicScale());
    }

    inline void stop()
    {
        detail::sound().stop();
    }

    inline void play()
    {
        constexpr int sampleRate = 44100;

        if (!detail::loaded())
        {
            detail::loaded() = true;

            constexpr float bpm = 104.f;
            constexpr float stepDur = 60.f / bpm / 2.f;
            constexpr int totalSteps = 32;
            constexpr float loopDur = stepDur * totalSteps;
            const int totalSamples = static_cast<int>(loopDur * sampleRate);

            struct Note { int midi; int steps; };

            const Note lead[] = {
                {72,1},{76,1},{79,1},{76,1},{81,1},{79,1},{76,1},{74,1},
                {69,1},{72,1},{76,1},{72,1},{79,1},{76,1},{72,1},{71,1},
                {69,1},{72,1},{77,1},{72,1},{81,1},{77,1},{72,1},{74,1},
                {67,1},{71,1},{74,1},{71,1},{79,1},{74,1},{71,1},{67,1},
            };
            const Note bass[] = {
                {48,2},{48,2},{48,2},{48,2},
                {45,2},{45,2},{45,2},{45,2},
                {41,2},{41,2},{41,2},{41,2},
                {43,2},{43,2},{43,2},{43,2},
            };
            const int pad[4][3] = {
                {48,55,60},
                {45,52,60},
                {41,48,57},
                {43,50,55},
            };

            auto midiHz = [](int m) { return 440.0 * std::pow(2.0, (m - 69) / 12.0); };

            std::vector<std::int16_t> out(totalSamples, 0);

            auto mix = [&](int startSample, int duration, double freq, double amp,
                           bool pulse, double attack, double release)
            {
                const double phaseStep = freq / sampleRate;
                for (int i = 0; i < duration && startSample + i < totalSamples; ++i)
                {
                    const double t = static_cast<double>(i) / sampleRate;
                    const double phase = std::fmod((startSample + i) * phaseStep, 1.0);

                    double env = 1.0;
                    if (t < attack) env = t / attack;
                    const double remaining = duration / static_cast<double>(sampleRate) - t;
                    if (remaining < release) env = std::min(env, remaining / release);

                    double wave;
                    if (pulse)
                        wave = (phase < 0.25) ? 1.0 : -1.0;
                    else
                        wave = 2.0 / 3.14159265358979 * std::asin(std::sin(phase * 2 * 3.14159265358979));

                    out[startSample + i] = static_cast<std::int16_t>(
                        out[startSample + i] + wave * env * amp * 30000.0);
                }
            };

            int stepIndex = 0;
            for (const Note& n : lead)
            {
                const int start = static_cast<int>(stepIndex * stepDur * sampleRate);
                const int dur = static_cast<int>(n.steps * stepDur * sampleRate);
                mix(start, dur, midiHz(n.midi), 0.22, true, 0.008, 0.10);
                stepIndex += n.steps;
            }

            stepIndex = 0;
            for (const Note& n : bass)
            {
                const int start = static_cast<int>(stepIndex * stepDur * sampleRate);
                const int dur = static_cast<int>(n.steps * stepDur * sampleRate);
                mix(start, dur, midiHz(n.midi - 12), 0.26, false, 0.010, 0.06);
                stepIndex += n.steps;
            }

            for (int bar = 0; bar < 4; ++bar)
            {
                const int start = static_cast<int>(bar * 8 * stepDur * sampleRate);
                const int dur = static_cast<int>(8 * stepDur * sampleRate);
                for (int m : pad[bar])
                    mix(start, dur, midiHz(m), 0.08, false, 0.6, 0.15);
            }

            detail::buffer().loadFromSamples(out.data(), out.size(), 1, sampleRate,
                                             {sf::SoundChannel::Mono});
            detail::sound().setBuffer(detail::buffer());
            detail::sound().setLooping(true);
            updateVolume();
        }

        if (detail::sound().getStatus() != sf::Sound::Status::Playing)
            detail::sound().play();
    }
}
