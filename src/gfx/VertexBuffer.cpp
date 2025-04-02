#include "gfx/VertexBuffer.h"

namespace vke {

VertexBuffer::VertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice)
    : m_buffer(device, physicalDevice) {}

VertexBuffer::~VertexBuffer() {
    destroy();
}

void VertexBuffer::create(const std::vector<Vertex>& vertices) {
  m_vertexCount = vertices.size();
  VkDeviceSize size = sizeof(Vertex) * m_vertexCount;

  // Create the buffer with vertex data
  m_buffer.create(size,
                  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  // Upload the vertex data to the buffer
  m_buffer.uploadData(vertices.data(), size);
}