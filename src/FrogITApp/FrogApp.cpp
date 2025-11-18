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

#ifdef _WIN32
#include <Dwmapi.h>
#include <windows.h>
#pragma comment(lib, "Dwmapi.lib")
#endif

#ifdef __linux__
// Warning: CAUSES NAMING COLLISIONS WITH SFML LIBRARY - TODO: make into classes like FrogApp_implLinux
#include <X11/X.h>
#include <X11/Xlib.h>// provides ::Window and XOpenDisplay
/* fix X11 None collision with sf::Style::None if needed */
#undef None
#undef Status
#endif

#ifdef __APPLE__
#include <objc/message.h>
#include <objc/objc.h>
#endif

#include "FrogApp.hpp"

#include "Constants.hpp"
#include "ResourceManager.hpp"

FrogApp::FrogApp() : m_frameSprite{ ResourceManager::getTexture("frame.png") }, m_frog{ "big_croak.png" }
{
    const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    const unsigned int width = m_frameSprite.getTexture().getSize().x;
    const unsigned int height = m_frameSprite.getTexture().getSize().y;

    // Create blank window, no titlebar (options)
    m_window.create(sf::VideoMode(sf::Vector2u(width, height), desktop.bitsPerPixel), "FrogITSettings", sf::Style::None);

    m_window.setIcon(ResourceManager::getImage("logo.png"));

    m_window.setFramerateLimit(FRAMERATE_LIMIT);

    if (!ImGui::SFML::Init(m_window)) {
        std::cout << "Unable to initialize ImGui\n";
        std::cout << "Waiting for input to close the application..." << '\n';
        std::cin.get();
        exit(-1);
    }

    turnWindowBackgroundInvisible(m_window);
    setWindowTopMost(m_window);

    initFrogWindow(m_frog, desktop.bitsPerPixel);
}

FrogApp::~FrogApp() { ImGui::SFML::Shutdown(); }

void FrogApp::processWindowEvents()
{
    while (const std::optional<sf::Event> event = m_window.pollEvent()) {
        ImGui::SFML::ProcessEvent(m_window, *event);

        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }

        // --- Start drag ---
        if (const auto* pressedEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (pressedEvent->button == sf::Mouse::Button::Left && sf::Mouse::getPosition(m_window).y < FRAME_TOP_BORDER_Y) {
                m_isDragging = true;
                m_dragOffset = sf::Mouse::getPosition(m_window);// position inside window
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
    ImGui::SFML::Update(m_window, m_clock.restart());

    m_window.clear(sf::Color::Transparent);

    // Content
    bool open = true;
    ImGui::SetNextWindowPos(ImVec2(FRAME_LEFT_BORDER_X, FRAME_TOP_BORDER_Y), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(FRAME_INNER_X, FRAME_INNER_Y), ImGuiCond_Always);
    ImGui::Begin(
        "UI", &open, static_cast<ImGuiWindowFlags>(static_cast<unsigned int>(ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar)));


    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + UI_CURSOR_OFFSET_Y);
    ImGui::SliderFloat("Volume1", &m_vol, UI_SLIDER_MIN, UI_SLIDER_MAX);
    ImGui::SliderFloat("Frequency1", &m_freq, UI_SLIDER_MIN, UI_SLIDER_MAX);

    const float windowWidth = ImGui::GetWindowSize().x;
    const float buttonWidth = UI_BUTTON_WIDTH_DEFAULT;
    ImGui::SetCursorPos(ImVec2(windowWidth - (UI_DISTANCE_MULT_BUTTON * buttonWidth) - ImGui::GetStyle().WindowPadding.x, ImGui::GetStyle().WindowPadding.y));

    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(200, 50, 50, 255));// red
    if (ImGui::Button("-", ImVec2(buttonWidth, buttonWidth))) {
        minimizeWindow(m_window);
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

    m_frog.getWindow().clear(sf::Color::Transparent);

    m_frog.getWindow().draw(m_frog.getSprite());

    m_frog.getWindow().display();
}

void FrogApp::initFrogWindow(Frog& frog, unsigned int bPP)
{
    auto& frogWindow = frog.getWindow();
    frogWindow.create(sf::VideoMode(frog.getSprite().getTexture().getSize(), bPP), "Frog", sf::Style::None);
    turnWindowBackgroundInvisible(frogWindow);
    setWindowTopMost(frogWindow);
}

void FrogApp::minimizeWindow(sf::RenderWindow& window)
{
#if defined(_WIN32)
    HWND hwnd = window.getNativeHandle();
    ShowWindow(hwnd, SW_MINIMIZE);
#elif defined(__APPLE__)
    /*void* nsWindow = window.getNativeHandle();
    objc_msgSend(nsWindow, sel_getUid("miniaturize:"), nullptr);*/
#elif defined(__linux__)
    /*Display* display = XOpenDisplay(nullptr);
    if (display == nullptr) {
        return;
    }
    xev.type = ClientMessage;
    xev.xclient.window = win;
    xev.xclient.message_type = XInternAtom(display, "WM_CHANGE_STATE", False);
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = IconicState;
    XSendEvent(display, DefaultRootWindow(display), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
    XFlush(display);*/
#endif
}

void FrogApp::turnWindowBackgroundInvisible(sf::RenderWindow& window)
{
    // Turn window invisible
    // TODO: Handle Linux invisible window specifics
#ifdef _WIN32
    MARGINS margins;
    margins.cxLeftWidth = -1;
    SetWindowLong(window.getNativeHandle(), GWL_STYLE, WS_POPUP | WS_VISIBLE);
    DwmExtendFrameIntoClientArea(window.getNativeHandle(), &margins);
#endif

#ifdef __APPLE__

#endif

#ifdef __linux__
    /*Display* display = XOpenDisplay(nullptr);
    if (!display) {
        fprintf(stderr, "Failed to open X display\n");
        return;
    }

    ::Window xwindow = window.getSystemHandle();

    // Set the window opacity (0xFFFFFFFF = opaque, 0x00000000 = fully transparent)
    unsigned long opacity = 0;// 0 = fully transparent
    Atom property = XInternAtom(display, "_NET_WM_WINDOW_OPACITY", False);
    XChangeProperty(display, xwindow, property, XA_CARDINAL, 32, PropModeReplace, (unsigned char*)&opacity, 1);

    XFlush(display);
    XCloseDisplay(display);*/
#endif
}

void FrogApp::setWindowTopMost(sf::RenderWindow& window)
{
#ifdef _WIN32
    SetWindowPos(window.getNativeHandle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
#endif

#ifdef __APPLE__
#endif

#ifdef __linux__
#endif
}
