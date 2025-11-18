#pragma once

#include <SFML/Graphics.hpp>

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

    sf::RenderWindow m_window;
    sf::Image m_iconImage;
    sf::Clock m_clock;
    sf::Vector2i m_dragOffset = { 0, 0 };
    sf::Sprite m_frameSprite;
    bool m_isDragging = false;
    float m_vol = 0.0F;
    float m_freq = 0.0F;

    Frog m_frog;
};