#include "core/Engine.h"
#include <iostream>

int main() {
    try {
        // Cria engine com título e dimensões de janela
        vke::Engine engine("Minha Janela Vulkan", 800, 600);
        engine.run();
    } catch (const std::exception& e) {
        std::cerr << "Erro fatal: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
