#ifndef VULKAN_ENGINE_H
#define VULKAN_ENGINE_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <string>
#include <memory>

namespace vke {

    class Device; // Forward declaration

    class Engine {
    public:
        Engine(std::string windowTitle, int width, int height);
        ~Engine();

        // Proíbe cópia
        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;

        void run() const;

    private:
        void initWindow();
        void initVulkan();
        void mainLoop() const;
        void cleanup();

        // Funções Vulkan extras
        void createInstance();
        void createSurface();
        static bool checkValidationLayerSupport();

    private:
        std::string m_windowTitle;
        int m_width;
        int m_height;
        GLFWwindow* m_window = nullptr;

        VkInstance m_instance = VK_NULL_HANDLE;
        VkSurfaceKHR m_surface = VK_NULL_HANDLE;

#ifdef _DEBUG
        VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
#endif

        // Gerenciador do device
        std::unique_ptr<Device> m_device;
    };

} // namespace vke

#endif
