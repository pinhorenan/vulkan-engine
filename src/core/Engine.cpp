#include "core/Engine.h"
#include "core/Device.h"

#include <stdexcept>
#include <vector>
#include <cstring>
#include <iostream>
#include <core/SwapChain.h>

namespace vke {

    Engine::Engine(std::string windowTitle, int width, int height)
        : m_windowTitle(std::move(windowTitle))
        , m_width(width)
        , m_height(height)
    {
        initWindow();
        initVulkan();
    }

    Engine::~Engine() {
        cleanup();
    }

    void Engine::run() const {
        mainLoop();
    }

    void Engine::initWindow() {
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW!");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_window = glfwCreateWindow(m_width, m_height, m_windowTitle.c_str(), nullptr, nullptr);
        if (!m_window) {
            throw std::runtime_error("Failed to create GLFW window!");
        }
    }

    // Função opcional para verificar se a camada de validação desejada está disponível.
    // Atualmente, usado apenas para exemplificar.
    // ReSharper disable once CppMemberFunctionMayBeStatic
    bool Engine::checkValidationLayerSupport() {
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
        bool layerFound = false;
        for (const auto& layerProperties : availableLayers) {
            if (strcmp(validationLayerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        return layerFound;
    }

    void Engine::createInstance() {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = m_windowTitle.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Vulkan Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        // Obtém as extensões necessárias a partir do GLFW
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        // DESCOMENTAR PARA HABILITAR AS CAMADAS DE VALIDAÇÃO
        /*
        if (checkValidationLayerSupport()) {
            const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
            createInfo.enabledLayerCount = 1;
            createInfo.ppEnabledLayerNames = &validationLayerName;
        } else {
            createInfo.enabledLayerCount = 0;
        }
        */
        createInfo.enabledLayerCount = 0; // Sem camadas de validação

        if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
            throw std::runtime_error("Fail to create Vulkan instance!");
        }
    }

    void Engine::createSurface() {
        if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS) {
            throw std::runtime_error("Fail to create window surface!");
        }
    }

    void Engine::initVulkan() {
        createInstance();
        createSurface();
        // Cria o device a partir da instância e da surface criadas
        m_device = std::make_unique<Device>(m_instance, m_surface);

        auto indices = m_device->findQueueFamilies(m_device->physicalDevice());
        m_swapChain = std::make_unique<SwapChain>(
            m_device->device(),
            m_device->physicalDevice(),
            m_surface,
            m_width,
            m_height,
            indices.graphicsFamily.value(),
            indices.presentFamily.value()
        );

        m_renderer = std::make_unique<Renderer>(
            m_device->device(),
            *m_swapChain,
            m_device->graphicsQueue(),
            m_device->presentQueue(),
            indices.graphicsFamily.value()
        );

    }

    void Engine::mainLoop() const {
        while (!glfwWindowShouldClose(m_window)) {
            glfwPollEvents();
            // Chama o drawFrame do renderer
            m_renderer->drawFrame();
        }
    }


    void Engine::cleanup() {
        // Destrói o device antes de destruir a surface e a instância
        m_device.reset();

        if (m_surface) {
            vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
            m_surface = VK_NULL_HANDLE;
        }
        if (m_instance) {
            vkDestroyInstance(m_instance, nullptr);
            m_instance = VK_NULL_HANDLE;
        }
        if (m_window) {
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }
        glfwTerminate();
    }

} // namespace vke
