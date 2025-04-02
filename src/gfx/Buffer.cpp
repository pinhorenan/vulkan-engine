#include "gfx/Buffer.h"
#include <stdexcept>
#include <cstring> // memcpy

namespace vke {

Buffer::Buffer(VkDevice device, VkPhysicalDevice physicalDevice)
    : m_device(device), m_physicalDevice(physicalDevice) {}

Buffer::~Buffer() { destroy(); }

void Buffer:create(VkDevice size, VkBufferUsageFlags usage, VkMemoryPropertyFlags, properties) {
  // Creates the buffer
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_device, &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS) {
      throw std::runtime_ERROR("Failed to create buffer!");
    }

    // Allocates memory for the buffer
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_device, m_buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType                       = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize              = memRequirements.size;
    allocInfo.memoryTypeIndex             = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemoru(m_device, &allocInfo, nullptr, &m_memory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate buffer memory!");
    }

    // Binds the buffer to the allocated memory
    vkBindBufferMemory(m_device, m_buffer, m_memory, 0);
}

void Buffer:uploadData(const void* srcData, vkDeviceSize size) {
  void* dstData;
  if (vkMapMemory(m_device, m_memory, 0, size, 0, &dstData) != VK_SUCCESS) {
    throw std::runtime_error("Failed to map buffer memory!");
  }

  std::memcpy(dstData, srcData, static_cast<size_t>(size));
  vkUnmapMemory(m_device, m_memory);
}

void Buffer::destroy() {
  if (m_memory != VK_NULL_HANDLE) {
    vkFreeMemory(m_device, m_memory, nullptr);
    m_memory = VK_NULL_HANDLE;
  }
  if (m_buffer != VK_NULL_HANDLE) {
    vkDestroyBuffer(m_device, m_buffer, nullptr);
    m_buffer = VK_NULL_HANDLE;
  }
}

uint32_t Buffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
    if ((typeFilter & (1 << i)) &&(memProperties.memoryTypes[i].propertyFlags & properties) == properties) { return i; }
  }

  throw std::runtime_error("Failed to find suitable memory type!");
}
