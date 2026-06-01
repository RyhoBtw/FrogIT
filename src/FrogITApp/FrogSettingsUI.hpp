#pragma once

#include "Constants.hpp"

class SoundScapeManager;

// Renders the soundscape settings controls inside the caller's already-open ImGui window.
class FrogSettingsUI
{
  public:
    void render(SoundScapeManager& manager);

  private:
    char m_nameBuffer[SOUNDSCAPE_NAME_MAX] = {};// edit buffer for the active soundscape's name
    int m_lastEditedIndex = -1;// which soundscape m_nameBuffer currently mirrors
};
