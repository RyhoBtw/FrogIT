#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "Frog.hpp"
#include "FrogSettingsUI.hpp"
#include "SoundScapeManager.hpp"
#include "WindowHandling.hpp"

class FrogApp
{
  public:
    FrogApp();
    ~FrogApp();
    void render();
    void processWindowEvents();
    const sf::RenderWindow& getWindow();

  private:
    void initFrogWindow(Frog& frog, unsigned int bPP);
    void updateFrogCount(int newCount);

    sf::RenderWindow m_window;
    sf::Clock m_clock;
    sf::Vector2i m_dragOffset = { 0, 0 };
    sf::Sprite m_frameSprite;
    bool m_isDragging = false;

    SoundScapeManager m_soundScapes;
    FrogSettingsUI m_settingsUI;

    sf::Vector2u m_desktopSize;
    unsigned int m_bitsPerPixel = 32;

    std::vector<std::unique_ptr<Frog>> m_frogs;
    int m_frogCount = 1;

    // Speech bubble
    sf::RenderWindow m_speechWindow;
    sf::Font m_font;
    bool m_speechWindowOpen = false;
    Frog* m_activeSpeaker = nullptr;
};