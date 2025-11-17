#pragma once

#include <SFML/Graphics.hpp>

class FrogApp
{
  public:
    FrogApp() = default;
    bool init();
    bool isRunning();
    void render();
    void processWindowEvents();

  private:
    void minimizeWindow(sf::RenderWindow &window);

    sf::RenderWindow m_window;
    sf::Clock m_clock;
    bool m_isDragging = false;
    sf::Vector2i m_dragOffset = { 0, 0 };
    float m_vol = 0.0F;
    float m_freq = 0.0F;
    bool m_isRunning = true;
    const sf::Sprite m_frameSprite;
};