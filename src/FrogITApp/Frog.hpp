#pragma once

#include <SFML/Graphics.hpp>
#include <random>
#include <string>
#include <vector>

class Frog
{
  public:
    Frog(const std::string& textureOpen, const std::string& textureClosed);
    void update(float deltaTime, sf::Vector2u desktopSize);
    void handleClick();
    sf::Sprite& getSprite();
    sf::RenderWindow& getWindow();
    sf::Vector2u getScaledSize() const;
    float getScale() const;
    bool isFacingRight() const;
    bool consumeDirectionChanged();

    bool hasSpeechBubble() const;
    const std::string& getSpeechText() const;
    sf::Vector2f getSpeechBubblePosition() const;
    void updateSpeechBubble(float deltaTime);

  private:
    void pickNextHop(sf::Vector2u desktopSize);

    sf::RenderWindow m_window;
    sf::Sprite m_sprite;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdangling-reference"
    const sf::Texture& m_texOpen;
    const sf::Texture& m_texClosed;
#pragma GCC diagnostic pop
    float m_scale = 1.f;

    sf::Vector2f m_position{ 200.f, 200.f };
    sf::Vector2f m_hopStart{ 200.f, 200.f };
    sf::Vector2f m_hopEnd{ 500.f, 400.f };
    float m_hopProgress = 1.f;   // 0..1, 1 = hop finished
    float m_hopDuration = 0.4f;
    float m_hopHeight = 40.f;
    float m_pauseTimer = 0.5f;
    bool m_facingRight = true;
    bool m_directionChanged = false;
    bool m_isHopping = false;
    int m_hopsRemaining = 0;

    // Speech bubble
    bool m_showSpeech = false;
    float m_speechTimer = 0.f;
    std::string m_speechText;

    std::mt19937 m_rng{ std::random_device{}() };
};