#ifndef VKE_VERTEX_H
#define VKE_VERTEX_H

#include <array>
#include <vulkan/vulkan.h>

namespace vke {

    struct Vertex {
        float position[2];
        float color[3];

        static VkVertexInputBindingDescription getBindingDescription();
        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
    };

} // namespace vke

#endif // VKE_VERTEX_H
