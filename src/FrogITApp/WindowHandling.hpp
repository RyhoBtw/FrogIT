#pragma once

#include <SFML/Graphics.hpp>

class WindowHandling
{
  public:
    static void minimizeWindow(sf::RenderWindow& window);
    static void turnWindowBackgroundInvisible(sf::RenderWindow& window);
    static void setWindowTopMost(sf::RenderWindow& window);
    static void setWindowNoActivate(sf::RenderWindow& window);
    static void moveFrogWindow(sf::RenderWindow& window, int x, int y);

#ifdef __linux__
    // Linux-specific shape masking functions for transparent window handling
    static void applyShapeMask(sf::RenderWindow& window, const sf::Texture& texture, float scale, bool flipped);
    static void applyFilledShapeMask(sf::RenderWindow& window, const sf::Texture& texture);
#endif
};
