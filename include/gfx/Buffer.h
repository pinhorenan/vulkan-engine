#ifndef VKE_BUFFER_H
#define VKE_BUFFER_H

#include <vulkan/vulkan.h>

namespace vke {

    class Buffer {
    public:
        Buffer(VkDevice device, VkPhysicalDevice physicalDevice);
        ~Buffer();

        // Proíbe cópia
        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        // Cria buffer com alocação de memória
        void create(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
        void destroy();

        // Copia dados do host (CPU) para o buffer (caso memória visível)
        void uploadData(const void* srcData, VkDeviceSize size);

        [[nodiscard]] VkBuffer getBuffer() const { return m_buffer; }
        [[nodiscard]] VkDeviceMemory getMemory() const { return m_memory; }

    private:
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    private:
        VkDevice m_device;
        VkPhysicalDevice m_physicalDevice;

        VkBuffer m_buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_memory = VK_NULL_HANDLE;
    };

} // namespace vke

#endif // VKE_BUFFER_H
