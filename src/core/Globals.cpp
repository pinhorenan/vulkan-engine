#include <vector>

namespace vke {

    // Define se as camadas de validação estão ativadas (ideal para debug)
    bool g_enableValidationLayers = true;

    // Lista das camadas de validação que serão usadas
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

}
