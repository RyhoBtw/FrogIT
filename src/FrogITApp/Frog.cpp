#include "Frog.hpp"

#include "ResourceManager.hpp"

Frog::Frog(const std::string& textureFilename) 
: m_sprite(ResourceManager::getTexture(textureFilename)) 
{ 
}

void Frog::jump() {}

void Frog::updateAnimation() {}

void Frog::trollUser() {}

sf::Sprite& Frog::getSprite() { return m_sprite; }

sf::RenderWindow& Frog::getWindow() { return m_window; }
