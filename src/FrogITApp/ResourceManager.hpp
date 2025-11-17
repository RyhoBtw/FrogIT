#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <unordered_map>

class ResourceManager
{
  public:
    static void preloadAssets();
    static const sf::Texture& getTexture(std::string textureFilename);
    static const sf::SoundBuffer& getSound(std::string soundFilename);

  private:
    static std::unordered_map<std::string, sf::Texture> m_textures;
    static std::unordered_map<std::string, sf::SoundBuffer> m_soundBuffers;
};