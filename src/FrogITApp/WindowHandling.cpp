#include "WindowHandling.hpp"

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <vector>

#ifdef _WIN32
#include <Dwmapi.h>
#include <windows.h>
#pragma comment(lib, "Dwmapi.lib")
#endif

#ifdef __linux__
#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#endif

#ifdef __APPLE__
#include <objc/message.h>
#include <objc/objc.h>
#endif

void WindowHandling::minimizeWindow(sf::RenderWindow& window)
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

void WindowHandling::turnWindowBackgroundInvisible(sf::RenderWindow& window)
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

// Prevents the window from receiving focus and appearing in the Alt-Tab list
void WindowHandling::setWindowNoActivate(sf::RenderWindow& window)
{
#ifdef _WIN32
    HWND hwnd = window.getNativeHandle();
    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT);
#endif

#ifdef __linux__
    Display* display = XOpenDisplay(nullptr);
    if (!display)
        return;

    ::Window xwindow = window.getNativeHandle();

    // Tell the WM this window should never receive input focus
    XWMHints* hints = XAllocWMHints();
    if (hints) {
        hints->flags = InputHint;
        hints->input = False;
        XSetWMHints(display, xwindow, hints);
        XFree(hints);
    }

    // Also mark it as a notification-type window (no focus, no taskbar)
    Atom windowType = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
    Atom typeNotification = XInternAtom(display, "_NET_WM_WINDOW_TYPE_NOTIFICATION", False);
    XChangeProperty(display, xwindow, windowType, XA_ATOM, 32, PropModeReplace, reinterpret_cast<unsigned char*>(&typeNotification), 1);

    XFlush(display);
    XCloseDisplay(display);
#endif
}


// Moves the OS window without triggering focus
void WindowHandling::moveFrogWindow(sf::RenderWindow& window, int x, int y)
{
#ifdef _WIN32
    SetWindowPos(window.getNativeHandle(), nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
#else
    window.setPosition(sf::Vector2i(x, y));
#endif
}

void WindowHandling::setWindowTopMost(sf::RenderWindow& window)
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

#ifdef __linux__
void WindowHandling::applyShapeMask(sf::RenderWindow& window, const sf::Texture& texture, float scale, bool flipped)
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

void WindowHandling::applyFilledShapeMask(sf::RenderWindow& window, const sf::Texture& texture)
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
