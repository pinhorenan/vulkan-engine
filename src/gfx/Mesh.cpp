#include "gfx/Mesh.h"

namespace vke {

Mesh::Mesh(VkDevice device, VkPhysicalDevice physicalDevice)
    : m_vertexBuffer(device, physicalDevice)
{}

Mesh::~Mesh() {
  destroy();
}

void Mesh::load(const std::vector<Vertex>& vertices) {
  m_vertexBuffer.create(vertices);
}

void Mesh::recordDrawCommands(VkCommandBuffer commandBuffer) const {
  m_vertexBuffer.bind(commandBuffer);
  vkCmdDraw(commandBuffer, static_cast<uint32_t>(m_vertexBuffer.getVertexCount()), 1, 0, 0);

}

void Mesh::destroy() { m_vertexBuffer.destroy();}

} // namespace vke