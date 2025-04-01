#include "core/Engine.h"

// Para lidar com exceções ou logs
#include <stdexcept>
#include <iostream>
#include <vector>
// Se for usar strings da STL
#include <cstring>

namespace vke {

static bool g_enableValidationLayers =
#ifdef _DEBUG
    true;
#else
    false;
#endif

// Para debug, especificar layers:
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

Engine::Engine(const std::string& windowTitle, int width, int height)
    : m_windowTitle(windowTitle), m_width(width), m_height(height) {

    initWindow();
    initVulkan();
}

Engine::~Engine() {
    cleanup();
}

void Engine::run() {
    mainLoop();
}

void Engine::initWindow() {
    if (!glfwInit()) {
        throw std::runtime_error("Falha ao inicializar GLFW!");
    }

    // Diz ao GLFW para não criar um contexto OpenGL
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // desabilita redimensionamento por simplicidade

    m_window = glfwCreateWindow(m_width, m_height, m_windowTitle.c_str(), nullptr, nullptr);
    if (!m_window) {
        throw std::runtime_error("Falha ao criar janela GLFW!");
    }
}

void Engine::initVulkan() {
    createInstance();
    // Caso queira criar o debug messenger:
#ifdef _DEBUG
    // createDebugMessenger();
#endif
    // Aqui você poderia criar surface, escolher physical device etc.
}

void Engine::createInstance() {
    if (g_enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("Validation layers requeridos não estão disponíveis!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Engine App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "VKE";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2; // ou 1.3, dependendo do SDK

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // Extensões do GLFW para criar janelas Vulkan
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    // Se quiser debug em tempo de execução, adiciona extensões de debug
#ifdef _DEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    // Validation layers
    if (g_enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    // Cria a instância Vulkan
    VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Falha ao criar instância Vulkan!");
    }
}

bool Engine::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;
        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound) return false;
    }
    return true;
}

void Engine::mainLoop() {
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();
        // Aqui você chamaria seu drawFrame() caso tivesse a swapchain e pipeline configuradas
    }
}

void Engine::cleanup() {
    // Destruir instância Vulkan
    if (m_instance) {
        vkDestroyInstance(m_instance, nullptr);
        m_instance = VK_NULL_HANDLE;
    }

    // Destruir janela
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }

    glfwTerminate();
}

} // namespace vke
