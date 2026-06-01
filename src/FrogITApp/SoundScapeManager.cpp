#include "SoundScapeManager.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <system_error>

#include "Constants.hpp"
#include "ResourceManager.hpp"

namespace
{
    // "wallaces_flying_frog" -> "Wallaces Flying Frog"
    std::string prettifyStem(const std::string& stem)
    {
        std::string out = stem;
        bool atWordStart = true;
        for (char& ch : out) {
            if (ch == '_' || ch == '-') {
                ch = ' ';
                atWordStart = true;
                continue;
            }
            if (atWordStart) {
                ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
                atWordStart = false;
            }
        }
        return out;
    }

    bool isMp3(const std::filesystem::path& path)
    {
        std::string ext = path.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return ext == ".mp3";
    }

    void scanDir(const char* dir, const char* keyPrefix, SoundCategory category,
                 std::vector<CatalogEntry>& out)
    {
        std::error_code errorCode;
        const std::filesystem::path base{ dir };
        if (!std::filesystem::is_directory(base, errorCode)) {
            return;
        }
        for (const auto& entry : std::filesystem::directory_iterator(base, errorCode)) {
            if (errorCode) {
                break;
            }
            const std::filesystem::path& path = entry.path();
            if (!entry.is_regular_file(errorCode) || !isMp3(path)) {
                continue;
            }
            CatalogEntry catalogEntry;
            // Build the key with a literal '/' (never path.string(), which yields '\' on Windows).
            catalogEntry.filename = std::string{ keyPrefix } + path.filename().string();
            catalogEntry.displayName = prettifyStem(path.stem().string());
            catalogEntry.category = category;
            out.push_back(std::move(catalogEntry));
        }
    }

    std::string trim(const std::string& str)
    {
        const auto first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) {
            return "";
        }
        const auto last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, last - first + 1);
    }
}

void SoundCatalog::build()
{
    entries.clear();
    scanDir(SOUND_DIR_LOOPING, SOUND_KEY_PREFIX_LOOPING, SoundCategory::Looping, entries);
    scanDir(SOUND_DIR_AMBIENT, SOUND_KEY_PREFIX_AMBIENT, SoundCategory::Ambient, entries);
    // Iteration order is unspecified; sort for deterministic UI rows and save output.
    std::sort(entries.begin(), entries.end(),
              [](const CatalogEntry& a, const CatalogEntry& b) { return a.filename < b.filename; });
}

SoundScapeManager::SoundScapeManager()
{
    m_catalog.build();
    buildPlayingSounds();
    load();

    if (m_soundScapes.empty()) {
        addSoundScape("Default");
    } else {
        if (!hasActive()) {
            m_activeIndex = 0;
        }
        switchTo(m_activeIndex);
    }
}

SoundScapeManager::~SoundScapeManager() { save(); }

void SoundScapeManager::buildPlayingSounds()
{
    // Reserve to final size before any emplace so no reallocation moves an sf::Sound.
    m_sounds.reserve(m_catalog.entries.size());
    for (const auto& entry : m_catalog.entries) {
        const sf::SoundBuffer& buffer = ResourceManager::getSound(entry.filename);
        m_sounds.emplace_back(buffer);
    }
}

int SoundScapeManager::catalogIndexOf(const std::string& soundFilename) const
{
    for (std::size_t i = 0; i < m_catalog.entries.size(); ++i) {
        if (m_catalog.entries[i].filename == soundFilename) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

SoundScape* SoundScapeManager::activeSoundScape()
{
    if (!hasActive()) {
        return nullptr;
    }
    return &m_soundScapes[static_cast<std::size_t>(m_activeIndex)];
}

SoundScape SoundScapeManager::makeDefaultSoundScape(const std::string& name) const
{
    SoundScape scape;
    scape.name = name;
    scape.settings.reserve(m_catalog.entries.size());
    for (const auto& entry : m_catalog.entries) {
        SoundSetting setting;
        setting.filename = entry.filename;
        setting.enabled = false;
        setting.volume = SOUND_VOLUME_DEFAULT;
        setting.frequency = AMBIENT_FREQ_DEFAULT;
        scape.settings.push_back(std::move(setting));
    }
    return scape;
}

void SoundScapeManager::reconcile(SoundScape& scape) const
{
    // Drop settings for sounds that no longer exist on disk.
    std::erase_if(scape.settings,
                  [this](const SoundSetting& setting) { return catalogIndexOf(setting.filename) < 0; });
    // Add defaults for catalog sounds the scape doesn't know about yet.
    for (const auto& entry : m_catalog.entries) {
        if (scape.findSetting(entry.filename) == nullptr) {
            SoundSetting setting;
            setting.filename = entry.filename;
            setting.volume = SOUND_VOLUME_DEFAULT;
            setting.frequency = AMBIENT_FREQ_DEFAULT;
            scape.settings.push_back(std::move(setting));
        }
    }
}

int SoundScapeManager::addSoundScape(const std::string& name)
{
    m_soundScapes.push_back(makeDefaultSoundScape(name));
    const int index = static_cast<int>(m_soundScapes.size()) - 1;
    switchTo(index);
    return index;
}

void SoundScapeManager::removeSoundScape(int index)
{
    if (index < 0 || index >= static_cast<int>(m_soundScapes.size())) {
        return;
    }
    m_soundScapes.erase(m_soundScapes.begin() + index);

    if (m_soundScapes.empty()) {
        m_activeIndex = -1;
        stopAll();
        return;
    }
    if (m_activeIndex >= static_cast<int>(m_soundScapes.size())) {
        m_activeIndex = static_cast<int>(m_soundScapes.size()) - 1;
    }
    switchTo(m_activeIndex);
}

void SoundScapeManager::rename(int index, const std::string& name)
{
    if (index < 0 || index >= static_cast<int>(m_soundScapes.size())) {
        return;
    }
    m_soundScapes[static_cast<std::size_t>(index)].name = name;
}

void SoundScapeManager::stopAll()
{
    for (auto& playing : m_sounds) {
        playing.sound.stop();
        playing.ambientTimer = 0.0F;
    }
}

void SoundScapeManager::resetAmbientTimer(std::size_t index)
{
    SoundScape* scape = activeSoundScape();
    if (scape == nullptr) {
        return;
    }
    const SoundSetting* setting = scape->findSetting(m_catalog.entries[index].filename);
    if (setting == nullptr || setting->frequency <= AMBIENT_FREQ_EPSILON) {
        m_sounds[index].ambientTimer = 0.0F;
        return;
    }
    std::uniform_real_distribution<float> jitter(1.0F - AMBIENT_INTERVAL_JITTER,
                                                 1.0F + AMBIENT_INTERVAL_JITTER);
    const float mean = AMBIENT_INTERVAL_BASE / setting->frequency;
    m_sounds[index].ambientTimer = mean * jitter(m_rng);
}

void SoundScapeManager::switchTo(int index)
{
    stopAll();
    if (index < 0 || index >= static_cast<int>(m_soundScapes.size())) {
        m_activeIndex = -1;
        return;
    }
    m_activeIndex = index;
    applyActiveSettings();
}

void SoundScapeManager::applyActiveSettings()
{
    SoundScape* scape = activeSoundScape();
    if (scape == nullptr) {
        return;
    }
    reconcile(*scape);

    for (std::size_t i = 0; i < m_catalog.entries.size(); ++i) {
        const CatalogEntry& entry = m_catalog.entries[i];
        const SoundSetting* setting = scape->findSetting(entry.filename);
        if (setting == nullptr) {
            continue;
        }
        m_sounds[i].sound.setVolume(setting->volume);

        if (entry.category == SoundCategory::Looping) {
            m_sounds[i].sound.setLooping(true);
            if (setting->enabled) {
                m_sounds[i].sound.play();
            } else {
                m_sounds[i].sound.stop();
            }
        } else {
            m_sounds[i].sound.setLooping(false);
            if (setting->enabled) {
                resetAmbientTimer(i);
            } else {
                m_sounds[i].ambientTimer = 0.0F;
            }
        }
    }
}

void SoundScapeManager::applyLiveSetting(const std::string& soundFilename)
{
    const int idx = catalogIndexOf(soundFilename);
    SoundScape* scape = activeSoundScape();
    if (idx < 0 || scape == nullptr) {
        return;
    }
    const std::size_t i = static_cast<std::size_t>(idx);
    const SoundSetting* setting = scape->findSetting(soundFilename);
    if (setting == nullptr) {
        return;
    }
    m_sounds[i].sound.setVolume(setting->volume);

    if (m_catalog.entries[i].category == SoundCategory::Looping) {
        m_sounds[i].sound.setLooping(true);
        const bool playing = m_sounds[i].sound.getStatus() == sf::SoundSource::Status::Playing;
        if (setting->enabled && !playing) {
            m_sounds[i].sound.play();
        } else if (!setting->enabled && playing) {
            m_sounds[i].sound.stop();
        }
    } else {
        m_sounds[i].sound.setLooping(false);
        if (setting->enabled) {
            resetAmbientTimer(i);
        } else {
            m_sounds[i].ambientTimer = 0.0F;
        }
    }
}

void SoundScapeManager::update(float dtSeconds)
{
    SoundScape* scape = activeSoundScape();
    if (scape == nullptr) {
        return;
    }
    for (std::size_t i = 0; i < m_catalog.entries.size(); ++i) {
        if (m_catalog.entries[i].category != SoundCategory::Ambient) {
            continue;
        }
        const SoundSetting* setting = scape->findSetting(m_catalog.entries[i].filename);
        if (setting == nullptr || !setting->enabled || setting->frequency <= AMBIENT_FREQ_EPSILON) {
            continue;
        }
        if (m_sounds[i].ambientTimer <= 0.0F) {
            resetAmbientTimer(i);
            continue;
        }
        m_sounds[i].ambientTimer -= dtSeconds;
        if (m_sounds[i].ambientTimer <= 0.0F) {
            // Only fire if the previous play has finished; if not, wait for the next interval.
            if (m_sounds[i].sound.getStatus() != sf::SoundSource::Status::Playing) {
                std::uniform_real_distribution<float> pitchJitter(0.8F, 1.2F);
                m_sounds[i].sound.setPitch(pitchJitter(m_rng));
                m_sounds[i].sound.setVolume(setting->volume);
                m_sounds[i].sound.play();
            }
            resetAmbientTimer(i);
        }
    }
}

void SoundScapeManager::save() const
{
    std::ofstream out{ SOUNDSCAPE_CONFIG_FILE, std::ios::trunc };
    if (!out) {
        std::cout << "Warning: could not write " << SOUNDSCAPE_CONFIG_FILE << '\n';
        return;
    }
    out << "active=" << m_activeIndex << '\n';
    for (const auto& scape : m_soundScapes) {
        out << "[soundscape]\n";
        out << "name=" << scape.name << '\n';
        for (const auto& setting : scape.settings) {
            const int idx = catalogIndexOf(setting.filename);
            const bool ambient = idx >= 0
                && m_catalog.entries[static_cast<std::size_t>(idx)].category == SoundCategory::Ambient;
            const char* prefix = ambient ? "ambient." : "loop.";
            out << prefix << setting.filename << ".enabled=" << (setting.enabled ? 1 : 0) << '\n';
            out << prefix << setting.filename << ".vol=" << setting.volume << '\n';
            if (ambient) {
                out << prefix << setting.filename << ".freq=" << setting.frequency << '\n';
            }
        }
    }
}

void SoundScapeManager::load()
{
    std::ifstream in{ SOUNDSCAPE_CONFIG_FILE };
    if (!in) {
        return;
    }

    m_soundScapes.clear();
    m_activeIndex = -1;
    SoundScape* current = nullptr;

    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty()) {
            continue;
        }
        if (line == "[soundscape]") {
            m_soundScapes.emplace_back();
            current = &m_soundScapes.back();
            continue;
        }
        const auto eq = line.find('=');
        if (eq == std::string::npos) {
            continue;
        }
        const std::string key = trim(line.substr(0, eq));
        const std::string value = trim(line.substr(eq + 1));

        if (key == "active") {
            try {
                m_activeIndex = std::stoi(value);
            } catch (...) {
                m_activeIndex = -1;
            }
            continue;
        }
        if (current == nullptr) {
            continue;
        }
        if (key == "name") {
            current->name = value;
            continue;
        }

        std::string rest;
        if (key.rfind("loop.", 0) == 0) {
            rest = key.substr(5);
        } else if (key.rfind("ambient.", 0) == 0) {
            rest = key.substr(8);
        } else {
            continue;
        }
        const auto lastDot = rest.find_last_of('.');
        if (lastDot == std::string::npos) {
            continue;
        }
        const std::string filename = rest.substr(0, lastDot);
        const std::string field = rest.substr(lastDot + 1);

        if (catalogIndexOf(filename) < 0) {
            continue;// sound in file but not on disk anymore: ignore
        }
        SoundSetting* setting = current->findSetting(filename);
        if (setting == nullptr) {
            current->settings.push_back(
                SoundSetting{ filename, false, SOUND_VOLUME_DEFAULT, AMBIENT_FREQ_DEFAULT });
            setting = &current->settings.back();
        }
        try {
            if (field == "enabled") {
                setting->enabled = std::stoi(value) != 0;
            } else if (field == "vol") {
                setting->volume = std::stof(value);
            } else if (field == "freq") {
                setting->frequency = std::stof(value);
            }
        } catch (...) {
            // tolerate malformed numeric values: keep the default
        }
    }

    for (auto& scape : m_soundScapes) {
        reconcile(scape);
    }
}
