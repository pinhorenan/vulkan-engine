#ifndef VKE_MESH_H
#define VKE_MESH_H

#include "VertexBuffer.h"
#include <vector>

namespace vke {

    class Mesh {
    public:
        Mesh(VkDevice device, VkPhysicalDevice physicalDevice);
        ~Mesh();

        void load(const std::vector<Vertex>& vertices);
        void recordDrawCommands(VkCommandBuffer commandBuffer) const;
        void destroy();

    private:
        VertexBuffer m_vertexBuffer;
    };

} // namespace vke

#endif // VKE_MESH_H
