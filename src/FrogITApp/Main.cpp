#include "FrogApp.hpp"

int main()
{
    // Init App
    FrogApp frogApp{};

    // Run App
    while (frogApp.getWindow().isOpen()) {
        frogApp.processWindowEvents();
        frogApp.render();
    }

    return 0;
}
