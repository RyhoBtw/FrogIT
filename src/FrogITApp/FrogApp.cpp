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

#include <iostream>
#include <optional>

#include "Constants.hpp"

#ifdef _WIN32
#include <Dwmapi.h>
#include <windows.h>
#pragma comment(lib, "Dwmapi.lib")
#endif

#ifdef __linux__
#include <X11/X.h>
#include <X11/Xlib.h>// provides ::Window and XOpenDisplay
/* fix X11 None collision with sf::Style::None if needed */
#undef None
#endif

#ifdef __APPLE__
#include <objc/message.h>
#include <objc/objc.h>
#endif

bool FrogApp::init()
{
    // Load custom frame
    sf::Texture frameTexture;
    if (!frameTexture.loadFromFile("frame.png")) {
        std::cout << "Unable to load frame.png\n";
        return false;
    }

    m_frameSprite.;

    const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    const unsigned int width = frameTexture.getSize().x;
    const unsigned int height = frameTexture.getSize().y;

    // Create blank window, no titlebar
    m_window.create(sf::VideoMode(sf::Vector2u(width, height), desktop.bitsPerPixel),
        "FrogIT",
        sf::Style::None
    );

    m_window.setFramerateLimit(FRAMERATE_LIMIT);

    if (!ImGui::SFML::Init(m_window)) {
        std::cout << "Unable to load ImGui\n";
        return false;
    }

    // Turn windows invisible
    // TODO: Handle Linux invisible window specifics
#ifdef _WIN32
    MARGINS margins;
    margins.cxLeftWidth = -1;
    SetWindowLong(m_window.getNativeHandle(), GWL_STYLE, WS_POPUP | WS_VISIBLE);
    DwmExtendFrameIntoClientArea(m_window.getNativeHandle(), &margins);
#endif

#ifdef __linux__
    Display *display = XOpenDisplay(nullptr);
    if (!display) {
        fprintf(stderr, "Failed to open X display\n");
        return;
    }

    ::Window xwindow = m_window.getSystemHandle();

    // Set the window opacity (0xFFFFFFFF = opaque, 0x00000000 = fully transparent)
    unsigned long opacity = 0;// 0 = fully transparent
    Atom property = XInternAtom(display, "_NET_WM_WINDOW_OPACITY", False);
    XChangeProperty(display, xwindow, property, XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&opacity, 1);

    XFlush(display);
    XCloseDisplay(display);
#endif
}

void FrogApp::processWindowEvents()
{
    while (const std::optional<sf::Event> event = m_window.pollEvent()) {
        ImGui::SFML::ProcessEvent(m_window, *event);

        if (event->is<sf::Event::Closed>()) { m_window.close(); }

        // --- Start drag ---
        if (const auto *pressedEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (pressedEvent->button == sf::Mouse::Button::Left
                && sf::Mouse::getPosition(m_window).y < FRAME_TOP_BORDER_Y) {
                m_isDragging = true;
                m_dragOffset = sf::Mouse::getPosition(m_window);// position inside window
            }
        }

        // --- Stop drag ---
        if (const auto *releasedEvent = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (releasedEvent->button == sf::Mouse::Button::Left) { m_isDragging = false; }
        }

        // --- Apply raw mouse delta while dragging ---
        if (m_isDragging) {
            if (event->is<sf::Event::MouseMovedRaw>() || event->is<sf::Event::MouseMoved>()) {
                m_window.setPosition(sf::Mouse::getPosition() - m_dragOffset);
            }
        }
    }
}

void FrogApp::render()
{
    ImGui::SFML::Update(m_window, m_clock.restart());

    m_window.clear(sf::Color::Transparent);

    // Content
    bool open = true;
    ImGui::SetNextWindowPos(ImVec2(FRAME_LEFT_BORDER_X, FRAME_TOP_BORDER_Y), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(FRAME_INNER_X, FRAME_INNER_Y), ImGuiCond_Always);
    ImGui::Begin("UI",
        &open,
        static_cast<ImGuiWindowFlags>(
            static_cast<unsigned int>(ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse
                                      | ImGuiWindowFlags_NoTitleBar)));


    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + UI_CURSOR_OFFSET_Y);
    ImGui::SliderFloat("Volume1", &m_vol, UI_SLIDER_MIN, UI_SLIDER_MAX);
    ImGui::SliderFloat("Frequency1", &m_freq, UI_SLIDER_MIN, UI_SLIDER_MAX);

    const float windowWidth = ImGui::GetWindowSize().x;
    const float buttonWidth = UI_BUTTON_WIDTH_DEFAULT;
    ImGui::SetCursorPos(
        ImVec2(windowWidth - (UI_DISTANCE_MULT_BUTTON * buttonWidth) - ImGui::GetStyle().WindowPadding.x,
            ImGui::GetStyle().WindowPadding.y));

    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(200, 50, 50, 255));// red
    if (ImGui::Button("-", ImVec2(buttonWidth, buttonWidth))) { minimizeWindow(m_window); }

    ImGui::SetCursorPos(
        ImVec2(windowWidth - buttonWidth - ImGui::GetStyle().WindowPadding.x, ImGui::GetStyle().WindowPadding.y));
    if (ImGui::Button("X", ImVec2(buttonWidth, buttonWidth))) { m_window.close(); }
    ImGui::PopStyleColor();

    ImGui::End();

    m_window.draw(frameSprite);

    ImGui::SFML::Render(m_window);
    m_window.display();
}

void FrogApp::minimizeWindow(sf::RenderWindow &window)
{
#if defined(_WIN32)
    HWND hwnd = window.getNativeHandle();
    ShowWindow(hwnd, SW_MINIMIZE);
#elif defined(__APPLE__)
    void *nsWindow = window.getNativeHandle();
    objc_msgSend(nsWindow, sel_getUid("miniaturize:"), nullptr);
#elif defined(__linux__)
    Display *display = XOpenDisplay(nullptr);
    if (display == nullptr) { return; }
    xev.type = ClientMessage;
    xev.xclient.window = win;
    xev.xclient.message_type = XInternAtom(display, "WM_CHANGE_STATE", False);
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = IconicState;
    XSendEvent(display, DefaultRootWindow(display), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
    XFlush(display);
#endif
}

bool FrogApp::isRunning() { return m_isRunning; }