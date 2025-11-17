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

  private:
    sf::Sprite m_sprite;
};