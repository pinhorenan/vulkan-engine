#include "gfx/Model.h"

namespace vke {

Model::Model(VkDevice device, VkPhysicalDevice physicalDevice)
    : m_device(device), m_physicalDevice(physicalDevice) {}

Model::~Model() {
  destroy();
}

void Model::addMesh(const std::vector<Vertex>& vertices) {
  auto mesh = std::make_unique<Mesh>(m_device, m_physicalDevice);
  mesh->load(vertices);
  m_meshes.push_back(std::move(mesh));
}

void Model::recordDrawCommands(VkCommandBuffer commandBuffer) const {
  for (const auto& mesh : m_meshes) {
    mesh->recordDrawCommands(commandBuffer);
  }
}

void Model::destroy() {
  for (auto& mesh : m_meshes) {
    mesh->destroy();
  }
  m_meshes.clear();
}

} // namespace vke