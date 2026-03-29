#pragma once

#include <SFML/Graphics.hpp>
#include <random>

class Frog
{
  public:
    Frog(const std::string& textureFilename);
    void update(float deltaTime, sf::Vector2u desktopSize);
    void jump();
    void updateAnimation();
    void trollUser();
    sf::Sprite& getSprite();
    sf::RenderWindow& getWindow();
    sf::Vector2u getScaledSize() const;
    float getScale() const;
    bool isFacingRight() const;
    bool consumeDirectionChanged();

  private:
    sf::RenderWindow m_window;
    sf::Sprite m_sprite;
    float m_scale = 1.f;

    sf::Vector2f m_position{ 200.f, 200.f };
    sf::Vector2f m_targetPosition{ 500.f, 400.f };
    float m_speed = 120.f;
    float m_pauseTimer = 0.5f;
    bool m_facingRight = true;
    bool m_directionChanged = false;

    std::mt19937 m_rng{ std::random_device{}() };
};