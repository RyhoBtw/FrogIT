#pragma once

#include <SFML/Graphics.hpp>

class WindowHandling
{
  public:
    static void minimizeWindow(sf::RenderWindow& window);
    static void turnWindowBackgroundInvisible(sf::RenderWindow& window);
    static void setWindowTopMost(sf::RenderWindow& window);

#ifdef __linux__
    static void applyShapeMask(sf::RenderWindow& window, const sf::Texture& texture, float scale, bool flipped);
    static void applyFilledShapeMask(sf::RenderWindow& window, const sf::Texture& texture);
#endif
};
