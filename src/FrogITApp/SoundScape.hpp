#pragma once

#include <string>
#include <vector>

#include "Constants.hpp"

enum class SoundCategory
{
    Looping,
    Ambient
};

// Per-sound state inside a soundscape. Pure data, keyed by filename (the catalog key).
struct SoundSetting
{
    std::string filename;// catalog key, e.g. "looping/rain.mp3"
    bool enabled = false;
    float volume = SOUND_VOLUME_DEFAULT;// 0..100
    float frequency = AMBIENT_FREQ_DEFAULT;// ambient only: triggers per minute
};

// A named combination of sound settings. Copyable and serializable: holds no sf::Sound.
struct SoundScape
{
    std::string name;
    std::vector<SoundSetting> settings;// one entry per catalog sound

    SoundSetting* findSetting(const std::string& soundFilename)
    {
        for (auto& setting : settings) {
            if (setting.filename == soundFilename) {
                return &setting;
            }
        }
        return nullptr;
    }

    const SoundSetting* findSetting(const std::string& soundFilename) const
    {
        for (const auto& setting : settings) {
            if (setting.filename == soundFilename) {
                return &setting;
            }
        }
        return nullptr;
    }
};
