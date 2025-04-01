#include "core/Device.h"

#include <stdexcept>
#include <set>
#include <iostream>
#include <vector>
#include <cstring>  // Para strcmp, se necessário

namespace vke {

    // Construtor: recebe a instância Vulkan e a surface criadas na Engine
    Device::Device(VkInstance instance, VkSurfaceKHR surface)
        : m_instance(instance)
        , m_surface(surface)
    {
        // Seleciona a GPU física e cria o dispositivo lógico
        pickPhysicalDevice();
        createLogicalDevice();
    }

    // Destrutor: destrói o dispositivo lógico, se criado
    Device::~Device() {
        if (m_device != VK_NULL_HANDLE) {
            vkDestroyDevice(m_device, nullptr);
            m_device = VK_NULL_HANDLE;
        }
    }

    // Seleciona uma GPU física que seja adequada para o uso com Vulkan
    void Device::pickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            throw std::runtime_error("No physical devices found with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

        // Seleciona a primeira GPU que satisfaça os critérios
        for (auto& dev : devices) {
            if (isDeviceSuitable(dev)) {
                m_physicalDevice = dev;
                break;
            }
        }

        if (m_physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("Failed to find a suitable GPU!");
        }
    }

    // Verifica se a GPU física possui suporte para as filas necessárias e extensões requeridas
    bool Device::isDeviceSuitable(VkPhysicalDevice device) const {
        QueueFamilyIndices indices = findQueueFamilies(device);
        bool indicesOk = indices.isComplete();
        bool extensionsOk = checkDeviceExtensionSupport(device);

        // Aqui você pode adicionar verificações adicionais (por exemplo, suporte a swap-chain)
        return indicesOk && extensionsOk;
    }

    // Verifica se a GPU suporta todas as extensões listadas em m_requiredExtensions
    bool Device::checkDeviceExtensionSupport(VkPhysicalDevice device) const {
        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> required(m_requiredExtensions.begin(), m_requiredExtensions.end());
        for (const auto& ext : availableExtensions) {
            required.erase(ext.extensionName);
        }

        return required.empty();
    }

    // Procura pelas filas (queues) necessárias: gráficos e apresentação
    QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice device) const {
        QueueFamilyIndices indices;

        uint32_t queueCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, queueFamilies.data());

        // Percorre cada fila para identificar a que suporta gráficos e apresentação
        for (uint32_t i = 0; i < queueCount; i++) {
            // Verifica suporte a gráficos
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                if (!indices.graphicsFamily.has_value()) {
                    indices.graphicsFamily = i;
                }
            }
            // Verifica se a fila suporta apresentação à surface
            VkBool32 presentSupport = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
            if (presentSupport) {
                if (!indices.presentFamily.has_value()) {
                    indices.presentFamily = i;
                }
            }
            // Se ambos os índices foram encontrados, podemos sair do loop
            if (indices.isComplete()) {
                break;
            }
        }

        return indices;
    }

    // Cria o dispositivo lógico a partir da GPU selecionada e configura as filas necessárias
    void Device::createLogicalDevice() {
        QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);
        std::vector<VkDeviceQueueCreateInfo> queueInfos;
        std::set<uint32_t> uniqueFamilies = {
            indices.graphicsFamily.value(),
            indices.presentFamily.value()
        };

        float queuePriority = 1.0f;
        for (uint32_t family : uniqueFamilies) {
            VkDeviceQueueCreateInfo queueInfo{};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.queueFamilyIndex = family;
            queueInfo.queueCount = 1;
            queueInfo.pQueuePriorities = &queuePriority;
            queueInfos.push_back(queueInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
        createInfo.pQueueCreateInfos = queueInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;

        // Configura as extensões necessárias (por exemplo, swap-chain)
        createInfo.enabledExtensionCount = static_cast<uint32_t>(m_requiredExtensions.size());
        createInfo.ppEnabledExtensionNames = m_requiredExtensions.data();

        // Cria o dispositivo lógico
        if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create logical device!");
        }

        // Recupera as filas para gráficos e apresentação
        vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
        vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
    }

} // namespace vke
