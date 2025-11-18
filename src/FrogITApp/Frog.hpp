#pragma once

#include <SFML/Graphics.hpp>

class Frog
{
  public:
    Frog(const std::string& textureFilename);
    void jump();
    void updateAnimation();
    void trollUser();
    sf::Sprite& getSprite();
    sf::RenderWindow& getWindow();

  private:
    sf::RenderWindow m_window;
    sf::Sprite m_sprite;
};