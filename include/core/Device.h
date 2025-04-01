#ifndef VKE_DEVICE_H
#define VKE_DEVICE_H

#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <memory>
#include <optional>

namespace vke {

    // Estrutura que encapsula os índices das filas (graphics e present)
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() const {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    class Device {
    public:
        Device(VkInstance instance, VkSurfaceKHR surface);
        ~Device();

        // Proíbe cópia
        Device(const Device&) = delete;
        Device& operator=(const Device&) = delete;

        // Getters
        VkDevice device() const { return m_device; }
        VkPhysicalDevice physicalDevice() const { return m_physicalDevice; }
        VkQueue graphicsQueue() const { return m_graphicsQueue; }
        VkQueue presentQueue() const { return m_presentQueue; }

    private:
        // Funções auxiliares
        void pickPhysicalDevice();
        bool isDeviceSuitable(VkPhysicalDevice device) const;
        bool checkDeviceExtensionSupport(VkPhysicalDevice device) const;
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;
        void createLogicalDevice();

    private:
        VkInstance m_instance = VK_NULL_HANDLE;
        VkSurfaceKHR m_surface = VK_NULL_HANDLE;

        // GPU selecionada
        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

        // Device lógico criado
        VkDevice m_device = VK_NULL_HANDLE;

        // Filas para gráficos e apresentação
        VkQueue m_graphicsQueue = VK_NULL_HANDLE;
        VkQueue m_presentQueue = VK_NULL_HANDLE;

        // Lista de extensões necessárias (por exemplo, swapchain)
        const std::vector<const char*> m_requiredExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
    };

} // namespace vke

#endif
