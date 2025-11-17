#include "FrogApp.hpp"

#include <iostream>

int main()
{
    FrogApp frogApp;

    bool initialized = frogApp.init();

    if (!initialized) {
        std::cout << "Failed to initialize FrogApp!\n";
        return -1;
    }

    while (frogApp.isRunning()) { 
        frogApp.processWindowEvents();
        forgApp.render();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
