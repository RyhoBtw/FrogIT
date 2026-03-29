#include "Frog.hpp"

#include "Constants.hpp"
#include "ResourceManager.hpp"

#include <cmath>

Frog::Frog(const std::string& textureFilename) 
: m_sprite(ResourceManager::getTexture(textureFilename)) 
{ 
    float textureHeight = static_cast<float>(m_sprite.getTexture().getSize().y);
    m_scale = FROG_TARGET_HEIGHT / textureHeight;
    m_sprite.setScale({ m_scale, m_scale });
}

void Frog::update(float deltaTime, sf::Vector2u desktopSize)
{
    if (!m_window.isOpen()) return;

    // Pause between jumps
    if (m_pauseTimer > 0.f) {
        m_pauseTimer -= deltaTime;
        return;
    }

    sf::Vector2f diff = m_targetPosition - m_position;
    float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

    if (distance < 15.f) {
        auto scaled = getScaledSize();
        std::uniform_real_distribution<float> distX(0.f, static_cast<float>(desktopSize.x - scaled.x));
        std::uniform_real_distribution<float> distY(0.f, static_cast<float>(desktopSize.y - scaled.y));
        m_targetPosition = { distX(m_rng), distY(m_rng) };

        std::uniform_real_distribution<float> pauseDist(0.5f, 3.0f);
        m_pauseTimer = pauseDist(m_rng);
        return;
    }

    // Move toward target
    sf::Vector2f direction = diff / distance;
    m_position += direction * m_speed * deltaTime;

    // Flip sprite based on movement direction
    auto texSize = m_sprite.getTexture().getSize();
    if (direction.x < 0 && m_facingRight) {
        m_sprite.setScale({ -m_scale, m_scale });
        m_sprite.setPosition({ static_cast<float>(texSize.x) * m_scale, 0.f });
        m_facingRight = false;
        m_directionChanged = true;
    } else if (direction.x >= 0 && !m_facingRight) {
        m_sprite.setScale({ m_scale, m_scale });
        m_sprite.setPosition({ 0.f, 0.f });
        m_facingRight = true;
        m_directionChanged = true;
    }

    // Move the OS window to follow the frog across the desktop
    m_window.setPosition(sf::Vector2i(
        static_cast<int>(m_position.x),
        static_cast<int>(m_position.y)));
}

void Frog::jump() {}

void Frog::updateAnimation() {}

void Frog::trollUser() {}

sf::Sprite& Frog::getSprite() { return m_sprite; }

sf::RenderWindow& Frog::getWindow() { return m_window; }

sf::Vector2u Frog::getScaledSize() const
{
    auto texSize = m_sprite.getTexture().getSize();
    return sf::Vector2u(
        static_cast<unsigned int>(static_cast<float>(texSize.x) * m_scale),
        static_cast<unsigned int>(static_cast<float>(texSize.y) * m_scale));
}

float Frog::getScale() const { return m_scale; }

bool Frog::isFacingRight() const { return m_facingRight; }

bool Frog::consumeDirectionChanged()
{
    if (m_directionChanged) {
        m_directionChanged = false;
        return true;
    }
    return false;
}
