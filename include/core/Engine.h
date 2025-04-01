#ifndef VULKAN_ENGINE_H
#define VULKAN_ENGINE_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <string>

namespace vke { // vke = Vulkan Engine, por exemplo

    class Engine {
    public:
        Engine(const std::string& windowTitle, int width, int height);
        ~Engine();

        // Proíbe cópia
        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;

        void run();

    private:
        void initWindow();
        void initVulkan();
        void mainLoop();
        void cleanup();

        // Funções Vulkan extras se precisar
        void createInstance();
        bool checkValidationLayerSupport();

    private:
        std::string m_windowTitle;
        int m_width;
        int m_height;
        GLFWwindow* m_window = nullptr;

        VkInstance m_instance = VK_NULL_HANDLE;
        // Caso precise de debug messenger
#ifdef _DEBUG
        VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
#endif
    };

} // namespace vke

#endif
