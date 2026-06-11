#include "FrogSettingsUI.hpp"

#include <imgui.h>

#include <cstddef>
#include <cstdio>
#include <string>

#include "Constants.hpp"
#include "SoundScape.hpp"
#include "SoundScapeManager.hpp"

static_assert(SOUNDSCAPE_NAME_MAX == 64, "Keep m_nameBuffer size in sync with SOUNDSCAPE_NAME_MAX");

namespace
{
    // Draws the per-sound rows for one category, editing the active soundscape's settings.
    void renderCategory(SoundScapeManager& manager, SoundScape& scape, SoundCategory category)
    {
        for (const auto& entry : manager.catalog().entries) {
            if (entry.category != category) {
                continue;
            }
            SoundSetting* setting = scape.findSetting(entry.filename);
            if (setting == nullptr) {
                continue;
            }
            ImGui::PushID(entry.filename.c_str());// unique id: labels below are reused per row
            if (ImGui::Checkbox(entry.displayName.c_str(), &setting->enabled)) {
                manager.applyLiveSetting(entry.filename);
            }
            if (setting->enabled) {
                if (ImGui::SliderFloat("Volume", &setting->volume, SOUND_VOLUME_MIN, SOUND_VOLUME_MAX,
                                       "%.0f")) {
                    manager.applyLiveSetting(entry.filename);
                }
                if (category == SoundCategory::Ambient) {
                    if (ImGui::SliderFloat("Frequency", &setting->frequency, AMBIENT_FREQ_MIN,
                                           AMBIENT_FREQ_MAX, "%.1f/min")) {
                        manager.applyLiveSetting(entry.filename);
                    }
                }
            }
            ImGui::PopID();
        }
    }
}

void FrogSettingsUI::renderSoundscapeSelector(SoundScapeManager& manager)
{
    auto& scapes = manager.soundScapes();
    const int active = manager.activeIndex();

    const char* preview =
        manager.hasActive() ? scapes[static_cast<std::size_t>(active)].name.c_str() : "(none)";

    if (ImGui::BeginCombo("Soundscape", preview)) {
        for (int i = 0; i < static_cast<int>(scapes.size()); ++i) {
            const bool selected = i == active;
            if (ImGui::Selectable(scapes[static_cast<std::size_t>(i)].name.c_str(), selected)) {
                if (i != active) {
                    manager.switchTo(i);
                    m_lastEditedIndex = -1;
                }
            }
            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (ImGui::Button("New")) {
        const std::string name = "Soundscape " + std::to_string(scapes.size() + 1);
        manager.addSoundScape(name);
        m_lastEditedIndex = -1;
    }
    ImGui::SameLine();
    ImGui::BeginDisabled(scapes.size() <= 1);
    if (ImGui::Button("Delete")) {
        manager.removeSoundScape(active);
        m_lastEditedIndex = -1;
    }
    ImGui::EndDisabled();
}

void FrogSettingsUI::renderNameEditor(SoundScapeManager& manager)
{
    if (!manager.hasActive())
        return;

    auto& scapes = manager.soundScapes();
    const int active = manager.activeIndex();

    if (m_lastEditedIndex != active) {
        const std::string& currentName = scapes[static_cast<std::size_t>(active)].name;
        std::snprintf(m_nameBuffer, sizeof(m_nameBuffer), "%s", currentName.c_str());
        m_lastEditedIndex = active;
    }
    if (ImGui::InputText("Name", m_nameBuffer, sizeof(m_nameBuffer)))
        manager.rename(active, std::string{ m_nameBuffer });
}

void FrogSettingsUI::render(SoundScapeManager& manager)
{
    renderSoundscapeSelector(manager);
    renderNameEditor(manager);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    SoundScape* scape = manager.activeSoundScape();
    if (scape == nullptr) {
        ImGui::TextUnformatted("No soundscape selected.");
        return;
    }

    if (ImGui::CollapsingHeader("Looping Sounds", ImGuiTreeNodeFlags_DefaultOpen)) {
        renderCategory(manager, *scape, SoundCategory::Looping);
    }
    if (ImGui::CollapsingHeader("Ambient Sounds", ImGuiTreeNodeFlags_DefaultOpen)) {
        renderCategory(manager, *scape, SoundCategory::Ambient);
    }
}
