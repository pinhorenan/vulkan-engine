#ifndef VKE_VERTEXBUFFER_H
#define VKE_VERTEXBUFFER_H

#include "Buffer.h"
#include "Vertex.h"
#include <vector>

namespace vke {

    class VertexBuffer {
    public:
        VertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice);
        ~VertexBuffer();

        void create(const std::vector<Vertex>& vertices);
        void bind(VkCommandBuffer commandBuffer) const;
        void destroy();

        [[nodiscard]] size_t getVertexCount() const { return m_vertexCount; }

    private:
        Buffer m_buffer;
        size_t m_vertexCount = 0;
    };

} // namespace vke

#endif // VKE_VERTEXBUFFER_H
