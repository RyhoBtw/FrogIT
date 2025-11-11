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
#include <X11/X.h>
#include <X11/Xlib.h>// provides ::Window and XOpenDisplay
/* fix X11 None collision with sf::Style::None if needed */
#undef None
#endif

#ifdef __APPLE__
#include <objc/message.h>
#include <objc/objc.h>
#endif

constexpr int FRAME_TOP_BORDER_Y = 116;
constexpr int FRAME_LEFT_BORDER_X = 64;
constexpr int FRAME_INNER_X = 311;
constexpr int FRAME_INNER_Y = 441;
constexpr unsigned int FRAMERATE_LIMIT = 60U;
constexpr float UI_CURSOR_OFFSET_Y = 24.0F;
constexpr float UI_BUTTON_WIDTH_DEFAULT = 20.0F;
constexpr float UI_SLIDER_MIN = 0.0F;
constexpr float UI_SLIDER_MAX = 10.0F;
constexpr float UI_DISTANCE_MULT_BUTTON = 2.5F;

void minimizeWindow(sf::RenderWindow &window)
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
    const ::Window win = window.getNativeHandle();
    XIconifyWindow(display, win, DefaultScreen(display));
    XFlush(display);
    XCloseDisplay(display);
#endif
}

void processWindowEvents(sf::RenderWindow &window, bool &dragging, sf::Vector2i &dragOffset)
{
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        ImGui::SFML::ProcessEvent(window, *event);

        if (event->is<sf::Event::Closed>()) { window.close(); }

        // --- Start drag ---
        if (const auto *pressedEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (pressedEvent->button == sf::Mouse::Button::Left
                && sf::Mouse::getPosition(window).y < FRAME_TOP_BORDER_Y) {
                dragging = true;
                dragOffset = sf::Mouse::getPosition(window);// position inside window
            }
        }

        // --- Stop drag ---
        if (const auto *releasedEvent = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (releasedEvent->button == sf::Mouse::Button::Left) { dragging = false; }
        }

        // --- Apply raw mouse delta while dragging ---
        if (dragging) {
            if (event->is<sf::Event::MouseMovedRaw>() || event->is<sf::Event::MouseMoved>()) {
                window.setPosition(sf::Mouse::getPosition() - dragOffset);
            }
        }
    }
}

int main()
{
    // Load custom frame
    sf::Texture frameTexture;
    if (!frameTexture.loadFromFile("frame.png")) {
        std::cout << "Unable to load frame.png\n";
        return -1;
    }
    const sf::Sprite frameSprite{ frameTexture };

    const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    const unsigned int width = frameTexture.getSize().x;
    const unsigned int height = frameTexture.getSize().y;

    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(width, height), desktop.bitsPerPixel),
        "FrogIT",
        sf::Style::None// removes OS title bar and borders
    );

    window.setFramerateLimit(FRAMERATE_LIMIT);

    if (!ImGui::SFML::Init(window)) { return -1; }

    // TODO: Handle Linux glass-window specifics if needed
#ifdef _WIN32
    MARGINS margins;
    margins.cxLeftWidth = -1;
    SetWindowLong(window.getNativeHandle(), GWL_STYLE, WS_POPUP | WS_VISIBLE);
    DwmExtendFrameIntoClientArea(window.getNativeHandle(), &margins);
#endif

    sf::Clock clock;
    bool dragging = false;
    sf::Vector2i dragOffset = { 0, 0 };
    float vol = 0.0F;
    float freq = 0.0F;

    while (window.isOpen()) {
        processWindowEvents(window, dragging, dragOffset);

        ImGui::SFML::Update(window, clock.restart());

        window.clear(sf::Color::Transparent);

        // Content
        bool open = true;
        ImGui::SetNextWindowPos(ImVec2(FRAME_LEFT_BORDER_X, FRAME_TOP_BORDER_Y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(FRAME_INNER_X, FRAME_INNER_Y), ImGuiCond_Always);
        ImGui::Begin("UI",
            &open,
            static_cast<ImGuiWindowFlags>(ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize
                                          | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar));

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + UI_CURSOR_OFFSET_Y);
        ImGui::SliderFloat("Volume1", &vol, UI_SLIDER_MIN, UI_SLIDER_MAX);
        ImGui::SliderFloat("Frequency1", &freq, UI_SLIDER_MIN, UI_SLIDER_MAX);

        const float windowWidth = ImGui::GetWindowSize().x;
        const float buttonWidth = UI_BUTTON_WIDTH_DEFAULT;
        ImGui::SetCursorPos(ImVec2(
            windowWidth - (UI_DISTANCE_MULT_BUTTON * buttonWidth) - ImGui::GetStyle().WindowPadding.x, ImGui::GetStyle().WindowPadding.y));

        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(200, 50, 50, 255)); // red
        if (ImGui::Button("-", ImVec2(buttonWidth, buttonWidth))) { minimizeWindow(window); }

        ImGui::SetCursorPos(
            ImVec2(windowWidth - buttonWidth - ImGui::GetStyle().WindowPadding.x, ImGui::GetStyle().WindowPadding.y));
        if (ImGui::Button("X", ImVec2(buttonWidth, buttonWidth))) { window.close(); }
        ImGui::PopStyleColor();

        ImGui::End();

        window.draw(frameSprite);

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
