#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>

#ifdef _WIN32
#include <Dwmapi.h>
#include <windows.h>

#pragma comment(lib, "Dwmapi.lib")
#endif

#ifdef __linux__
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#endif

#ifdef __APPLE__
#include <objc/message.h>
#include <objc/objc.h>
#endif

const int FRAME_TOP_BORDER_Y = 116;
const int FRAME_LEFT_BORDER_X = 64;
const int FRAME_INNER_X = 311;
const int FRAME_INNER_Y = 441;

void minimizeWindow(sf::RenderWindow &window)
{
#if defined(_WIN32)
    HWND hwnd = window.getNativeHandle();
    ShowWindow(hwnd, SW_MINIMIZE);
#elif defined(__APPLE__)
    void *nsWindow = window.getNativeHandle();
    objc_msgSend(nsWindow, sel_getUid("miniaturize:"), nullptr);
#elif defined(__linux__)
    Display *display = XOpenDisplay(NULL);
    if (!display) return;
    ::Window win = window.getNativeHandle();
    XIconifyWindow(display, win, DefaultScreen(display));
    XFlush(display);
    XCloseDisplay(display);
#endif
}

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
        sf::RenderWindow(
        sf::VideoMode(sf::Vector2u(frameTexture.getSize().x, frameTexture.getSize().y), desktop.bitsPerPixel),
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
    float vol = 0.0f, freq = 0.0f;
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);
            if (event->is<sf::Event::Closed>()) { window.close(); }

            // --- Start drag ---
            if (const auto *e = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (e->button == sf::Mouse::Button::Left && sf::Mouse::getPosition(window).y < 115) {
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
        bool open = true;
        ImGui::SetNextWindowPos(ImVec2(FRAME_LEFT_BORDER_X, FRAME_TOP_BORDER_Y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(FRAME_INNER_X, FRAME_INNER_Y), ImGuiCond_Always);
        ImGui::Begin("UI",
            &open,
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse
                | ImGuiWindowFlags_NoTitleBar);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 24);
        ImGui::SliderFloat("Volume1", &vol, 0.0f, 10.0f);
        ImGui::SliderFloat("Frequency1", &freq, 0.0f, 10.0f);

        float windowWidth = ImGui::GetWindowSize().x;
        float buttonWidth = 20.0f;
        ImGui::SetCursorPos(ImVec2(
            windowWidth - (3 * buttonWidth) - ImGui::GetStyle().WindowPadding.x, ImGui::GetStyle().WindowPadding.y));
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(200, 50, 50, 255));// red
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
}