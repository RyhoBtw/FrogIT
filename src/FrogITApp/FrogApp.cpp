#include "FrogApp.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowEnums.hpp>

#include <imgui-SFML.h>
#include <imgui.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <optional>
#include <vector>

#include "Constants.hpp"
#include "ResourceManager.hpp"
#include "WindowHandling.hpp"

#ifdef _WIN32
#include <windows.h>
#endif


FrogApp::FrogApp() : m_frameSprite{ ResourceManager::getTexture("frame.png") }
{
    const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    const unsigned int width = m_frameSprite.getTexture().getSize().x;
    const unsigned int height = m_frameSprite.getTexture().getSize().y;

    // Create blank window, no titlebar (options)
    m_window.create(sf::VideoMode(sf::Vector2u(width, height), desktop.bitsPerPixel), "FrogITSettings", sf::Style::None);

    m_window.setIcon(ResourceManager::getImage("logo.png"));

    m_window.setFramerateLimit(FRAMERATE_LIMIT);

    m_desktopSize = desktop.size;
    m_bitsPerPixel = desktop.bitsPerPixel;

    if (!ImGui::SFML::Init(m_window)) {
        std::cout << "Unable to initialize ImGui\n";
        std::cout << "Waiting for input to close the application..." << '\n';
        std::cin.get();
        exit(-1);
    }

    WindowHandling::turnWindowBackgroundInvisible(m_window);
#ifndef __linux__
    WindowHandling::setWindowTopMost(m_window);
#endif

#ifdef __linux__
    WindowHandling::applyFilledShapeMask(m_window, m_frameSprite.getTexture());
#endif

    // Spawn initial frog
    updateFrogCount(m_frogCount);

    if (!m_font.openFromFile("assets/fonts/tuffy.ttf")) {
        std::cout << "Warning: Could not load font\n";
    }
}

FrogApp::~FrogApp()
{
    if (m_speechWindowOpen) {
        m_speechWindow.close();
    }
    if (m_tongueWindowOpen) {
        m_tongueWindow.close();
    }
    ImGui::SFML::Shutdown();
}

void FrogApp::processWindowEvents()
{
    // Process frog window events
    for (auto& frog : m_frogs) {
        while (const std::optional<sf::Event> frogEvent = frog->getWindow().pollEvent()) {
            if (frogEvent->is<sf::Event::Closed>()) {
                m_window.close();
            }
            if (const auto* pressed = frogEvent->getIf<sf::Event::MouseButtonPressed>()) {
                if (pressed->button == sf::Mouse::Button::Left) {
                    frog->handleClick();
                }
            }
        }
    }

    while (const std::optional<sf::Event> event = m_window.pollEvent()) {
        ImGui::SFML::ProcessEvent(m_window, *event);

        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }

        // --- Start drag (only if ImGui doesn't want the mouse, e.g. not clicking a button) ---
        if (const auto* pressedEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (pressedEvent->button == sf::Mouse::Button::Left
                && sf::Mouse::getPosition(m_window).y < FRAME_TOP_BORDER_Y
                && !ImGui::GetIO().WantCaptureMouse) {
                m_isDragging = true;
                m_dragOffset = sf::Mouse::getPosition(m_window);
            }
        }

        // --- Stop drag ---
        if (const auto* releasedEvent = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (releasedEvent->button == sf::Mouse::Button::Left) {
                m_isDragging = false;
            }
        }

        // --- Apply raw mouse delta while dragging ---
        if (m_isDragging) {
            if (event->is<sf::Event::MouseMovedRaw>() || event->is<sf::Event::MouseMoved>()) {
                m_window.setPosition(sf::Mouse::getPosition() - m_dragOffset);
            }
        }
    }
}

const sf::RenderWindow& FrogApp::getWindow() { return m_window; }

void FrogApp::render()
{
    sf::Time dt = m_clock.restart();
    ImGui::SFML::Update(m_window, dt);

    m_soundScapes.update(dt.asSeconds());

    for (auto& frog : m_frogs) {
        frog->update(dt.asSeconds(), m_desktopSize);

#ifdef __linux__
        if (frog->consumeDirectionChanged()) {
            WindowHandling::applyShapeMask(frog->getWindow(), frog->getSprite().getTexture(),
                           frog->getScale(), !frog->isFacingRight());
        }
#endif
    }

    m_window.clear(sf::Color::Transparent);

    // Content
    bool open = true;
    ImGui::SetNextWindowPos(ImVec2(FRAME_LEFT_BORDER_X, FRAME_TOP_BORDER_Y), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(FRAME_INNER_X, FRAME_INNER_Y), ImGuiCond_Always);
    ImGui::Begin(
        "UI", &open, static_cast<ImGuiWindowFlags>(static_cast<unsigned int>(ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar)));

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + UI_CURSOR_OFFSET_Y);
    m_settingsUI.render(m_soundScapes);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::InputInt("Frogs", &m_frogCount)) {
        m_frogCount = std::clamp(m_frogCount, FROG_COUNT_MIN, FROG_COUNT_MAX);
        updateFrogCount(m_frogCount);
    }

    const float windowWidth = ImGui::GetWindowSize().x;
    const float buttonWidth = UI_BUTTON_WIDTH_DEFAULT;
    ImGui::SetCursorPos(ImVec2(windowWidth - (UI_DISTANCE_MULT_BUTTON * buttonWidth) - ImGui::GetStyle().WindowPadding.x, ImGui::GetStyle().WindowPadding.y));

    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(200, 50, 50, 255));// red
    if (ImGui::Button("-", ImVec2(buttonWidth, buttonWidth))) {
        WindowHandling::minimizeWindow(m_window);
    }

    ImGui::SetCursorPos(ImVec2(windowWidth - buttonWidth - ImGui::GetStyle().WindowPadding.x, ImGui::GetStyle().WindowPadding.y));
    if (ImGui::Button("X", ImVec2(buttonWidth, buttonWidth))) {
        m_window.close();
    }
    ImGui::PopStyleColor();

    ImGui::End();
    
    m_window.draw(m_frameSprite);

    ImGui::SFML::Render(m_window);
    m_window.display();

    renderTongues();

    for (auto& frog : m_frogs) {
        frog->getWindow().clear(sf::Color::Transparent);
        frog->getWindow().draw(frog->getSprite());
        frog->getWindow().display();

        frog->renderSpeechBubble(m_speechWindow,
                                 m_speechWindowOpen,
                                 m_activeSpeaker,
                                 m_font,
                                 [](sf::RenderWindow& w) { WindowHandling::turnWindowBackgroundInvisible(w); },
                                 [](sf::RenderWindow& w) { WindowHandling::setWindowTopMost(w); });
    }
}

void FrogApp::initFrogWindow(Frog& frog, unsigned int bPP)
{
    auto& frogWindow = frog.getWindow();
    frogWindow.create(sf::VideoMode(frog.getScaledSize(), bPP), "Frog", sf::Style::None);
    WindowHandling::turnWindowBackgroundInvisible(frogWindow);
    WindowHandling::setWindowTopMost(frogWindow);
    WindowHandling::setWindowNoActivate(frogWindow);

#ifdef __linux__
    WindowHandling::applyShapeMask(frogWindow, frog.getSprite().getTexture(), frog.getScale(), false);
#endif
}

void FrogApp::renderTongues()
{
    // Collect frogs with active tongues
    bool anyTongue = false;
    for (auto& frog : m_frogs) {
        if (frog->isTongueActive()) { anyTongue = true; break; }
    }

    if (!anyTongue) {
        if (m_tongueWindowOpen) {
            m_tongueWindow.close();
            m_tongueWindowOpen = false;
        }
        return;
    }

    // Create/ensure tongue overlay window (fullscreen, click-through)
    if (!m_tongueWindowOpen) {
        // Extend 1px past every screen edge: an overlay that *exactly* matches the
        // desktop resolution makes DWM treat it as exclusive fullscreen, which
        // disables composition and turns the transparent background black.
        m_tongueWindow.create(
            sf::VideoMode(sf::Vector2u(m_desktopSize.x + 2, m_desktopSize.y + 2), m_bitsPerPixel),
            "TongueOverlay", sf::Style::None);
        WindowHandling::turnWindowBackgroundInvisible(m_tongueWindow);
        WindowHandling::setWindowTopMost(m_tongueWindow);
        WindowHandling::setWindowNoActivate(m_tongueWindow);
        m_tongueWindow.setPosition(sf::Vector2i(-1, -1));
        m_tongueWindowOpen = true;
    }

    // Drain events so the window doesn't freeze
    while (m_tongueWindow.pollEvent()) {}

    sf::Vector2i mousePos = sf::Mouse::getPosition();

    m_tongueWindow.clear(sf::Color::Transparent);

    for (auto& frog : m_frogs) {
        if (!frog->isTongueActive()) continue;

        sf::Vector2f mouth = frog->getTongueMouthPosition();
        sf::Vector2f tip(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

        // Direction from mouth to cursor
        sf::Vector2f dir = tip - mouth;
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);

        // Pull cursor toward frog via WinAPI
#ifdef _WIN32
        if (len > 1.f) {
            sf::Vector2f norm = dir / len;
            float pull = TONGUE_PULL_SPEED / FRAMERATE_LIMIT;
            float moveDist = std::min(pull, len * 0.15f); // gentle pull
            sf::Vector2f newPos = tip - norm * moveDist;
            SetCursorPos(static_cast<int>(newPos.x), static_cast<int>(newPos.y));
            tip = newPos;
        }
#endif

        // Draw tongue as a thick line using a quad
        if (len > 1.f) {
            sf::Vector2f norm = dir / len;
            sf::Vector2f perp(-norm.y, norm.x);
            float half = TONGUE_WIDTH / 2.f;

            sf::VertexArray quad(sf::PrimitiveType::TriangleStrip, 4);
            quad[0].position = mouth + perp * half;
            quad[1].position = mouth - perp * half;
            quad[2].position = tip   + perp * half;
            quad[3].position = tip   - perp * half;

            // Tongue colour: pink-red, slightly transparent at tip
            sf::Color baseColour(220, 60, 100, 230);
            sf::Color tipColour (220, 60, 100, 160);
            quad[0].color = baseColour;
            quad[1].color = baseColour;
            quad[2].color = tipColour;
            quad[3].color = tipColour;

            m_tongueWindow.draw(quad);

            // Tip circle (small dot)
            sf::CircleShape dot(TONGUE_WIDTH * 0.9f);
            dot.setFillColor(tipColour);
            dot.setOrigin({ TONGUE_WIDTH * 0.9f, TONGUE_WIDTH * 0.9f });
            dot.setPosition(tip);
            m_tongueWindow.draw(dot);
        }
    }

    m_tongueWindow.display();
}

void FrogApp::updateFrogCount(int newCount)
{
    int current = static_cast<int>(m_frogs.size());
    if (newCount > current) {
        for (int i = current; i < newCount; ++i) {
            auto frog = std::make_unique<Frog>("big_croak.png", "big_closed.png");
            frog->randomizePosition(m_desktopSize);
            initFrogWindow(*frog, m_bitsPerPixel);
            m_frogs.push_back(std::move(frog));
        }
    } else if (newCount < current) {
        // Close speech bubble if its speaker is being removed
        for (int i = newCount; i < current; ++i) {
            if (m_activeSpeaker == m_frogs[static_cast<size_t>(i)].get()) {
                if (m_speechWindowOpen) {
                    m_speechWindow.close();
                    m_speechWindowOpen = false;
                }
                m_activeSpeaker = nullptr;
            }
            m_frogs[static_cast<size_t>(i)]->getWindow().close();
        }
        m_frogs.resize(static_cast<size_t>(newCount));
    }
}

