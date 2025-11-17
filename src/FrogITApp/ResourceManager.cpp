#include "ResourceManager.hpp"

#include <iostream>

void ResourceManager::preloadAssets() {}

const sf::Texture& ResourceManager::getTexture(std::string textureFilename)
{
    if (m_textures.find(textureFilename) == m_textures.end()) {
        sf::Texture texture;
        if (!texture.loadFromFile("assets/textures/" + textureFilename)) {
            std::cout << "Failed to load texture " << textureFilename << '\n';
            std::cout << "Waiting for input to close the application..." << '\n';
            std::cin.get();
            std::exit(-1);
        }
        else
        {
            m_textures.emplace(textureFilename, std::move(texture));
        }
    }

    return m_textures.at(textureFilename);
}

const sf::SoundBuffer& ResourceManager::getSound(std::string soundFilename)
{
    if (m_soundBuffers.find(soundFilename) == m_soundBuffers.end()) {
        sf::SoundBuffer soundBuffer;
        if (!soundBuffer.loadFromFile("assets/sounds/" + soundFilename)) {
            std::cout << "Failed to load texture " << soundFilename << '\n';
            std::cout << "Waiting for input to close the application..." << '\n';
            std::cin.get();
            std::exit(-1);
        } else {
            m_soundBuffers.emplace(soundFilename, std::move(soundBuffer));
        }
    }

    return m_soundBuffers.at(soundFilename);
}

std::unordered_map<std::string, sf::Texture> ResourceManager::m_textures;
std::unordered_map<std::string, sf::SoundBuffer> ResourceManager::m_soundBuffers;