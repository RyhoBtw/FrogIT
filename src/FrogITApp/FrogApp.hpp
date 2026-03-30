#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "Frog.hpp"

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
    void minimizeWindow(sf::RenderWindow& window);
    void turnWindowBackgroundInvisible(sf::RenderWindow& window);
    void setWindowTopMost(sf::RenderWindow& window);
    void renderSpeechBubble(Frog& frog);
    void updateFrogCount(int newCount);

    sf::RenderWindow m_window;
    sf::Image m_iconImage;
    sf::Clock m_clock;
    sf::Vector2i m_dragOffset = { 0, 0 };
    sf::Sprite m_frameSprite;
    bool m_isDragging = false;
    float m_vol = 0.0F;
    float m_freq = 0.0F;

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