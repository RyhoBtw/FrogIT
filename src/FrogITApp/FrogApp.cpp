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
#include <iostream>
#include <optional>
#include <vector>

#ifdef _WIN32
#include <Dwmapi.h>
#include <windows.h>
#pragma comment(lib, "Dwmapi.lib")
#endif

#ifdef __linux__
// Warning: CAUSES NAMING COLLISIONS WITH SFML LIBRARY
#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#undef None
#undef Status
#undef Bool
#endif

#ifdef __APPLE__
#include <objc/message.h>
#include <objc/objc.h>
#endif

#include "FrogApp.hpp"

#include "Constants.hpp"
#include "ResourceManager.hpp"

#ifdef __linux__
// Apply X11 Shape mask so transparent pixels are physically clipped away.
// When flipped=true, the mask is horizontally mirrored to match a flipped sprite.
static void applyShapeMask(sf::RenderWindow& window, const sf::Texture& texture, float scale, bool flipped)
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) return;

    ::Window xwindow = window.getNativeHandle();

    sf::Image image = texture.copyToImage();
    auto imgSize = image.getSize();

    auto scaledW = static_cast<unsigned int>(static_cast<float>(imgSize.x) * scale);
    auto scaledH = static_cast<unsigned int>(static_cast<float>(imgSize.y) * scale);
    if (scaledW == 0 || scaledH == 0) { XCloseDisplay(display); return; }

    Pixmap mask = XCreatePixmap(display, xwindow, scaledW, scaledH, 1);
    GC gc = XCreateGC(display, mask, 0, nullptr);

    XSetForeground(display, gc, 0);
    XFillRectangle(display, mask, gc, 0, 0, scaledW, scaledH);

    XSetForeground(display, gc, 1);
    for (unsigned int y = 0; y < scaledH; ++y) {
        auto texY = std::min(static_cast<unsigned int>(static_cast<float>(y) / scale), imgSize.y - 1);
        unsigned int runStart = 0;
        bool inRun = false;
        for (unsigned int x = 0; x <= scaledW; ++x) {
            bool opaque = false;
            if (x < scaledW) {
                auto texX = std::min(static_cast<unsigned int>(static_cast<float>(x) / scale), imgSize.x - 1);
                if (flipped) texX = imgSize.x - 1 - texX;
                opaque = image.getPixel(sf::Vector2u(texX, texY)).a > 64;
            }
            if (opaque && !inRun) {
                runStart = x;
                inRun = true;
            } else if (!opaque && inRun) {
                XFillRectangle(display, mask, gc, static_cast<int>(runStart), static_cast<int>(y), x - runStart, 1);
                inRun = false;
            }
        }
    }

    XShapeCombineMask(display, xwindow, ShapeBounding, 0, 0, mask, ShapeSet);

    XFreeGC(display, gc);
    XFreePixmap(display, mask);
    XFlush(display);
    XCloseDisplay(display);
}

// For the settings window: precise frame outline with filled interior.
// Uses flood-fill from edges to find exterior transparent pixels, then the mask
// is everything NOT exterior (opaque frame pixels + enclosed interior).
static void applyFilledShapeMask(sf::RenderWindow& window, const sf::Texture& texture)
{
    Display* display = XOpenDisplay(nullptr);
    if (!display) return;

    ::Window xwindow = window.getNativeHandle();

    sf::Image image = texture.copyToImage();
    auto imgSize = image.getSize();
    if (imgSize.x == 0 || imgSize.y == 0) { XCloseDisplay(display); return; }

    auto w = imgSize.x;
    auto h = imgSize.y;

    // Build boolean grid: true = opaque (part of frame border)
    std::vector<bool> opaque(static_cast<size_t>(w) * h, false);
    for (unsigned int y = 0; y < h; ++y)
        for (unsigned int x = 0; x < w; ++x)
            opaque[static_cast<size_t>(y) * w + x] = image.getPixel(sf::Vector2u(x, y)).a > 64;

    // Flood-fill from edges to mark exterior transparent pixels
    std::vector<bool> exterior(static_cast<size_t>(w) * h, false);
    std::vector<std::pair<unsigned int, unsigned int>> stack;
    // Seed all transparent edge pixels
    for (unsigned int x = 0; x < w; ++x) {
        if (!opaque[x]) stack.emplace_back(x, 0u);
        if (!opaque[static_cast<size_t>(h - 1) * w + x]) stack.emplace_back(x, h - 1);
    }
    for (unsigned int y = 1; y + 1 < h; ++y) {
        if (!opaque[static_cast<size_t>(y) * w]) stack.emplace_back(0u, y);
        if (!opaque[static_cast<size_t>(y) * w + w - 1]) stack.emplace_back(w - 1, y);
    }

    while (!stack.empty()) {
        auto [sx, sy] = stack.back();
        stack.pop_back();
        auto idx = static_cast<size_t>(sy) * w + sx;
        if (exterior[idx]) continue;
        exterior[idx] = true;
        if (sx > 0     && !opaque[idx - 1]  && !exterior[idx - 1])  stack.emplace_back(sx - 1, sy);
        if (sx + 1 < w && !opaque[idx + 1]  && !exterior[idx + 1])  stack.emplace_back(sx + 1, sy);
        if (sy > 0     && !opaque[idx - w]  && !exterior[idx - w])  stack.emplace_back(sx, sy - 1);
        if (sy + 1 < h && !opaque[idx + w]  && !exterior[idx + w])  stack.emplace_back(sx, sy + 1);
    }

    // Shape mask = NOT exterior (opaque border + interior)
    Pixmap mask = XCreatePixmap(display, xwindow, w, h, 1);
    GC gc = XCreateGC(display, mask, 0, nullptr);

    XSetForeground(display, gc, 0);
    XFillRectangle(display, mask, gc, 0, 0, w, h);

    XSetForeground(display, gc, 1);
    for (unsigned int y = 0; y < h; ++y) {
        unsigned int runStart = 0;
        bool inRun = false;
        for (unsigned int x = 0; x <= w; ++x) {
            bool visible = (x < w) && !exterior[static_cast<size_t>(y) * w + x];
            if (visible && !inRun) {
                runStart = x;
                inRun = true;
            } else if (!visible && inRun) {
                XFillRectangle(display, mask, gc, static_cast<int>(runStart),
                               static_cast<int>(y), x - runStart, 1);
                inRun = false;
            }
        }
    }

    XShapeCombineMask(display, xwindow, ShapeBounding, 0, 0, mask, ShapeSet);

    XFreeGC(display, gc);
    XFreePixmap(display, mask);
    XFlush(display);
    XCloseDisplay(display);
}
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

    turnWindowBackgroundInvisible(m_window);
#ifndef __linux__
    setWindowTopMost(m_window);
#endif

#ifdef __linux__
    applyFilledShapeMask(m_window, m_frameSprite.getTexture());
#endif

    // Spawn initial frog
    updateFrogCount(m_frogCount);

    if (!m_font.openFromFile("assets/fonts/tuffy.ttf")) {
        std::cout << "Warning: Could not load font\n";
    }
}

FrogApp::~FrogApp() { ImGui::SFML::Shutdown(); }

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

    for (auto& frog : m_frogs) {
        frog->update(dt.asSeconds(), m_desktopSize);

#ifdef __linux__
        if (frog->consumeDirectionChanged()) {
            applyShapeMask(frog->getWindow(), frog->getSprite().getTexture(),
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
    ImGui::SliderFloat("Volume", &m_vol, UI_SLIDER_MIN, UI_SLIDER_MAX);
    ImGui::SliderFloat("Frequency", &m_freq, UI_SLIDER_MIN, UI_SLIDER_MAX);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::InputInt("Frogs", &m_frogCount)) {
        m_frogCount = std::clamp(m_frogCount, 1, 200);
        updateFrogCount(m_frogCount);
    }

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

    for (auto& frog : m_frogs) {
        frog->getWindow().clear(sf::Color::Transparent);
        frog->getWindow().draw(frog->getSprite());
        frog->getWindow().display();

        renderSpeechBubble(*frog);
    }
}

void FrogApp::renderSpeechBubble(Frog& frog)
{
    if (frog.hasSpeechBubble()) {
        // Only one speech window at a time — take over if a new frog speaks
        if (&frog != m_activeSpeaker) {
            if (m_speechWindowOpen) {
                m_speechWindow.close();
                m_speechWindowOpen = false;
            }
            m_activeSpeaker = &frog;
        }

        if (!m_speechWindowOpen) {
            sf::Text text(m_font, frog.getSpeechText(), 14);
            auto bounds = text.getLocalBounds();
            unsigned int bubbleW = static_cast<unsigned int>(bounds.size.x + 24.f);
            unsigned int bubbleH = static_cast<unsigned int>(bounds.size.y + 20.f);

            m_speechWindow.create(sf::VideoMode(sf::Vector2u(bubbleW, bubbleH), 32), "Speech", sf::Style::None);
            turnWindowBackgroundInvisible(m_speechWindow);
            setWindowTopMost(m_speechWindow);
            m_speechWindowOpen = true;
        }

        auto bubblePos = frog.getSpeechBubblePosition();
        auto speechSize = m_speechWindow.getSize();
        m_speechWindow.setPosition(sf::Vector2i(
            static_cast<int>(bubblePos.x - static_cast<float>(speechSize.x) / 2.f),
            static_cast<int>(bubblePos.y - static_cast<float>(speechSize.y))));

        while (m_speechWindow.pollEvent()) {}

        m_speechWindow.clear(sf::Color(60, 60, 60, 230));

        sf::RectangleShape bg(sf::Vector2f(static_cast<float>(speechSize.x), static_cast<float>(speechSize.y)));
        bg.setFillColor(sf::Color(245, 245, 220));
        bg.setOutlineColor(sf::Color(80, 130, 80));
        bg.setOutlineThickness(2.f);
        m_speechWindow.draw(bg);

        sf::Text text(m_font, frog.getSpeechText(), 14);
        text.setFillColor(sf::Color(40, 40, 40));
        text.setPosition({ 12.f, 6.f });
        m_speechWindow.draw(text);

        m_speechWindow.display();
    } else if (m_speechWindowOpen && m_activeSpeaker == &frog) {
        m_speechWindow.close();
        m_speechWindowOpen = false;
        m_activeSpeaker = nullptr;
    }
}

void FrogApp::initFrogWindow(Frog& frog, unsigned int bPP)
{
    auto& frogWindow = frog.getWindow();
    frogWindow.create(sf::VideoMode(frog.getScaledSize(), bPP), "Frog", sf::Style::None);
    turnWindowBackgroundInvisible(frogWindow);
    setWindowTopMost(frogWindow);

#ifdef __linux__
    applyShapeMask(frogWindow, frog.getSprite().getTexture(), frog.getScale(), false);
#endif
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

void FrogApp::minimizeWindow(sf::RenderWindow& window)
{
#if defined(_WIN32)
    HWND hwnd = window.getNativeHandle();
    ShowWindow(hwnd, SW_MINIMIZE);
#elif defined(__APPLE__)
    /*void* nsWindow = window.getNativeHandle();
    objc_msgSend(nsWindow, sel_getUid("miniaturize:"), nullptr);*/
#elif defined(__linux__)
    Display* display = XOpenDisplay(nullptr);
    if (display == nullptr) {
        return;
    }
    XIconifyWindow(display, window.getNativeHandle(), DefaultScreen(display));
    XFlush(display);
    XCloseDisplay(display);
#endif
}

void FrogApp::turnWindowBackgroundInvisible(sf::RenderWindow& window)
{
#ifdef _WIN32
    MARGINS margins;
    margins.cxLeftWidth = -1;
    SetWindowLong(window.getNativeHandle(), GWL_STYLE, WS_POPUP | WS_VISIBLE);
    DwmExtendFrameIntoClientArea(window.getNativeHandle(), &margins);
#endif

#ifdef __APPLE__

#endif

#ifdef __linux__
    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        return;
    }

    ::Window xwindow = window.getNativeHandle();

    // Skip taskbar and pager so the overlay windows don't clutter the taskbar
    Atom wmState = XInternAtom(display, "_NET_WM_STATE", False);
    Atom skipTaskbar = XInternAtom(display, "_NET_WM_STATE_SKIP_TASKBAR", False);
    Atom skipPager = XInternAtom(display, "_NET_WM_STATE_SKIP_PAGER", False);
    Atom atoms[2] = { skipTaskbar, skipPager };
    XChangeProperty(display, xwindow, wmState, XA_ATOM, 32, PropModeAppend,
        reinterpret_cast<unsigned char*>(atoms), 2);

    XFlush(display);
    XCloseDisplay(display);
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
    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        return;
    }

    ::Window xwindow = window.getNativeHandle();

    Atom wmState = XInternAtom(display, "_NET_WM_STATE", False);
    Atom wmStateAbove = XInternAtom(display, "_NET_WM_STATE_ABOVE", False);

    XEvent xev = {};
    xev.type = ClientMessage;
    xev.xclient.window = xwindow;
    xev.xclient.message_type = wmState;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = 1;  // _NET_WM_STATE_ADD
    xev.xclient.data.l[1] = static_cast<long>(wmStateAbove);
    xev.xclient.data.l[2] = 0;

    XSendEvent(display, DefaultRootWindow(display), False,
        SubstructureRedirectMask | SubstructureNotifyMask, &xev);

    XFlush(display);
    XCloseDisplay(display);
#endif
}
