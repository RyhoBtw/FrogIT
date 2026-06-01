#pragma once

#include <SFML/Audio.hpp>

#include <cstddef>
#include <random>
#include <string>
#include <vector>

#include "SoundScape.hpp"

// One available sound discovered on disk. The filename is the stable key shared with SoundSetting.
struct CatalogEntry
{
    std::string displayName;// prettified, e.g. "Wallaces Flying Frog"
    std::string filename;// e.g. "looping/wallaces_flying_frog.mp3"
    SoundCategory category = SoundCategory::Looping;
};

// All sounds found under assets/sounds/{looping,ambient}, sorted by filename for stable order.
struct SoundCatalog
{
    std::vector<CatalogEntry> entries;
    void build();
};

// A live SFML sound, reused across soundscape switches. Non-copyable (sf::Sound is movable only).
struct PlayingSound
{
    explicit PlayingSound(const sf::SoundBuffer& buffer) : sound(buffer) {}

    sf::Sound sound;
    float ambientTimer = 0.0F;// seconds until next one-shot trigger (ambient sounds)
};

// Owns the catalog, the live sounds, all soundscapes, and the currently active selection.
class SoundScapeManager
{
  public:
    SoundScapeManager();
    ~SoundScapeManager();

    SoundScapeManager(const SoundScapeManager&) = delete;
    SoundScapeManager& operator=(const SoundScapeManager&) = delete;

    void update(float dtSeconds);

    void switchTo(int index);
    int addSoundScape(const std::string& name);// returns the new index
    void removeSoundScape(int index);
    void rename(int index, const std::string& name);

    // Push a single sound's current setting to its live sf::Sound (called on UI edits).
    void applyLiveSetting(const std::string& soundFilename);

    const SoundCatalog& catalog() const { return m_catalog; }
    std::vector<SoundScape>& soundScapes() { return m_soundScapes; }
    const std::vector<SoundScape>& soundScapes() const { return m_soundScapes; }
    int activeIndex() const { return m_activeIndex; }
    bool hasActive() const
    {
        return m_activeIndex >= 0 && m_activeIndex < static_cast<int>(m_soundScapes.size());
    }
    SoundScape* activeSoundScape();

  private:
    void buildPlayingSounds();// reserve + emplace once: no moves after construction
    void stopAll();
    void applyActiveSettings();
    void resetAmbientTimer(std::size_t index);
    SoundScape makeDefaultSoundScape(const std::string& name) const;
    void reconcile(SoundScape& scape) const;// align a scape's settings with the catalog
    void save() const;
    void load();
    int catalogIndexOf(const std::string& soundFilename) const;

    SoundCatalog m_catalog;
    std::vector<PlayingSound> m_sounds;// parallel to m_catalog.entries
    std::vector<SoundScape> m_soundScapes;
    int m_activeIndex = -1;

    std::mt19937 m_rng{ std::random_device{}() };
};
