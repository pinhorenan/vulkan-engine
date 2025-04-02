#ifndef VKE_MODEL_H
#define VKE_MODEL_H

#include "gfx/Mesh.h"
#include <memory>
#include <vector>

namespace vke {

class Model {
  public:
    Model(VkDevice device, VkPhysicalDevice physicalDevice);
    ~Model();

    void addMesh(const std::vector<Vertex>& vertices);
    void recordDrawCommands(VkCommandBuffer commandBuffer) const;
    void destroy();

  private:
    VkDevice m_device;
    VkPhysicalDevice m_physicalDevice;

    std::vector<std::unique_ptr<Mesh>> m_meshes;
  };

  } // namespace vke

#endif //VKE_MODEL_H
