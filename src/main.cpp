#include "../include/core/Engine.h"
#include <iostream>
#include <cstdlib>

int main() {
    std::cout << "Starting Vulkan application...\n";
    try {
        // Cria engine com título e dimensões de janela
        vke::Engine engine("Vulkan Window", 800, 600);
        engine.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Application finished successfully.\n";
    return EXIT_SUCCESS;
}
