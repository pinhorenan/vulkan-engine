#ifndef RENDERER_H
#define RENDERER_H

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>

#include "core/SwapChain.h"

/**
 * Classe que gerencia o pipeline de desenho:
 *  - Render Pass
 *  - Framebuffers
 *  - Command Buffers
 *  - Semáforos / Fences
 *  - drawFrame()
 */
class Renderer {
public:
    /**
     * Construtor recebe:
     * - device: o dispositivo lógico Vulkan
     * - swapChain: referência à swapchain (com imagens, image views, formato e extensão)
     * - graphicsQueue, presentQueue: filas para enviar comandos e apresentar
     * - graphicsQueueFamilyIndex: índice da fila de gráficos (para criar command pool)
     */
    Renderer(
        VkDevice device,
        const SwapChain& swapChain,
        VkQueue graphicsQueue,
        VkQueue presentQueue,
        uint32_t graphicsQueueFamilyIndex
    );

    ~Renderer();

    /**
     * Cria a render pass básica usando a imagem da swapchain como attachment de cor.
     */
    void createRenderPass();

    /**
     * Cria um framebuffer para cada ImageView da swapchain.
     */
    void createFramebuffers();

    /**
     * Cria o command pool para alocar command buffers.
     */
    void createCommandPool();

    /**
     * Aloca e grava command buffers para cada framebuffer,
     * iniciando/encerrando a render pass.
     */
    void createCommandBuffers();

    /**
     * Cria semáforos e fence para sincronizar as operações de renderização.
     */
    void createSyncObjects();

    /**
     * Principal função de desenho: adquire uma imagem da swapchain,
     * envia o command buffer para a GPU e apresenta a imagem na tela.
     */
    void drawFrame();

private:
    // Recursos e parâmetros principais
    VkDevice m_device;
    const SwapChain& m_swapChain;
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;
    uint32_t m_graphicsQueueFamilyIndex;

    // Render pass
    VkRenderPass m_renderPass = VK_NULL_HANDLE;

    // Framebuffers (um para cada image view da swapchain)
    std::vector<VkFramebuffer> m_framebuffers;

    // Command pool e command buffers
    VkCommandPool m_commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> m_commandBuffers;

    // Objetos de sincronização
    VkSemaphore m_imageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore m_renderFinishedSemaphore = VK_NULL_HANDLE;
    VkFence m_inFlightFence = VK_NULL_HANDLE;
};

#endif // RENDERER_H
