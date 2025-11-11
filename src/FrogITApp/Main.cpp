#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <Dwmapi.h>

#pragma comment(lib, "Dwmapi.lib")
#endif

int main()
{
    // Load custom frame
    sf::Texture frameTexture;
    if (!frameTexture.loadFromFile("frame.png")) {
        std::cout << "Unable to load frame.png\n";
        return -1;
    }
    sf::Sprite frameSprite{ frameTexture };

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    auto window =
        sf::RenderWindow(sf::VideoMode({ frameTexture.getSize().x, frameTexture.getSize().y }, desktop.bitsPerPixel),
            "FrogIT",
            sf::Style::None// removes OS title bar and borders
        );
    window.setFramerateLimit(60);
    if (!ImGui::SFML::Init(window)) return -1;
    
    // TODO Handle Linux glass-window
#ifdef _WIN32
    MARGINS margins;
    margins.cxLeftWidth = -1;

    SetWindowLong(window.getNativeHandle(), GWL_STYLE, WS_POPUP | WS_VISIBLE);
    DwmExtendFrameIntoClientArea(window.getNativeHandle(), &margins);
#endif

    sf::Clock clock;
    auto dragging = false;
    sf::Vector2i dragOffset = { 0, 0 };
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);
            if (event->is<sf::Event::Closed>()) { window.close(); }

            // --- Start drag ---
            if (const auto *e = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (e->button == sf::Mouse::Button::Left) {
                    dragging = true;
                    dragOffset = sf::Mouse::getPosition(window);// position inside window
                }
            }

            // --- Stop drag ---
            if (const auto *e = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (e->button == sf::Mouse::Button::Left) { dragging = false; }
            }

            // --- Apply raw mouse delta while dragging ---
            if (dragging) {
                if (event->is<sf::Event::MouseMovedRaw>() || event->is<sf::Event::MouseMoved>()) {
                    window.setPosition(sf::Mouse::getPosition() - dragOffset);
                }
            }
        }

        ImGui::SFML::Update(window, clock.restart());

        window.clear(sf::Color::Transparent);

        // Content

        window.draw(frameSprite);

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}