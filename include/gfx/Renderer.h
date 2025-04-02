#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include <vulkan/vulkan.h>
#include <vector>

#include "core/SwapChain.h"
#include "GraphicsPipeline.h"

class Renderer {
public:
    Renderer(
        VkDevice device,
        const SwapChain& swapChain,
        VkQueue graphicsQueue,
        VkQueue presentQueue,
        uint32_t graphicsQueueFamilyIndex
    );

    ~Renderer();

    void createRenderPass();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffers();
    void createSyncObjects();
    void drawFrame() const;

private:
    VkDevice m_device;
    const SwapChain& m_swapChain;
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;
    uint32_t m_graphicsQueueFamilyIndex;

    VkRenderPass m_renderPass = VK_NULL_HANDLE;
    std::vector<VkFramebuffer> m_framebuffers;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> m_commandBuffers;

    VkSemaphore m_imageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore m_renderFinishedSemaphore = VK_NULL_HANDLE;
    VkFence m_inFlightFence = VK_NULL_HANDLE;

    std::unique_ptr<vke::GraphicsPipeline> m_graphicsPipeline;
};

#endif // RENDERER_H
