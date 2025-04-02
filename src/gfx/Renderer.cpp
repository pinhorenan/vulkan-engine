#include "gfx/Renderer.h"
#include "gfx/GraphicsPipeline.h"
#include "gfx/Model.h"
#include "gfx/Vertex.h"

#include <array>
#include <vector>

Renderer::Renderer(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    const SwapChain& swapChain,
    VkQueue graphicsQueue,
    VkQueue presentQueue,
    uint32_t graphicsQueueFamilyIndex
)
    : m_device(device),
      m_physicalDevice(physicalDevice),
      m_swapChain(swapChain),
      m_graphicsQueue(graphicsQueue),
      m_presentQueue(presentQueue),
      m_graphicsQueueFamilyIndex(graphicsQueueFamilyIndex)
{
    // Creates a render pass,
    createRenderPass();
    createFramebuffers();
    createCommandPool();

    // Creates a model and a graphics pipeline
    m_graphicsPipeline = std::make_unique<vke::GraphicsPipeline>(m_device, m_renderPass, m_swapChain.getExtent());

    //
    m_model = std::make_unique<vke::Model>(m_device, m_physicalDevice);
    std::vector<vke::Vertex> triangleVertices = {
        { { 0.0f,  -0.5f }, { 1.0f, 0.0f, 0.0f } },
        { { 0.5f,   0.5f }, { 0.0f, 1.0f, 0.0f } },
        { { -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f } }
    };
    m_model->addMesh(triangleVertices);

    // Create command buffers and synchronization objects
    createCommandBuffers();
    createSyncObjects();
}

Renderer::~Renderer() {
    // Espera a fila terminar antes de destruir recursos
    vkDeviceWaitIdle(m_device);

    // Limpa sincronização
    vkDestroySemaphore(m_device, m_imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(m_device, m_renderFinishedSemaphore, nullptr);
    vkDestroyFence(m_device, m_inFlightFence, nullptr);

    // Destrói command pool (isso libera command buffers também)
    if (m_commandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(m_device, m_commandPool, nullptr);
    }

    // Destrói framebuffers
    for (auto framebuffer : m_framebuffers) {
        vkDestroyFramebuffer(m_device, framebuffer, nullptr);
    }

    // Destrói render pass
    if (m_renderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(m_device, m_renderPass, nullptr);
    }
}

// ------------------------------------------------------
// Criação da render pass
// ------------------------------------------------------
void Renderer::createRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_swapChain.getImageFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;     // Limpa antes de desenhar
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;   // Armazena para apresentar
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0; // Índice do attachment no vetor de attachments
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    // (Opcional) Dependências de subpass, se precisar sincronizar com estágios anteriores/posteriores
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;                       // Subpass "antes" (externo)
    dependency.dstSubpass = 0;                                        // Nosso subpass
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create render pass!");
    }
}

// ------------------------------------------------------
// Cria um framebuffer para cada image view da swapchain
// ------------------------------------------------------
void Renderer::createFramebuffers() {
    const auto& imageViews = m_swapChain.getImageViews();
    m_framebuffers.resize(imageViews.size());

    for (size_t i = 0; i < imageViews.size(); i++) {
        VkImageView attachments[] = { imageViews[i] };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = m_swapChain.getExtent().width;
        framebufferInfo.height = m_swapChain.getExtent().height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer!");
        }
    }
}

// ------------------------------------------------------
// Cria command pool para alocar command buffers
// ------------------------------------------------------
void Renderer::createCommandPool() {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    // Se quiser resetar command buffers individualmente, use:
    // poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = m_graphicsQueueFamilyIndex;

    if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool!");
    }
}

// ------------------------------------------------------
// Cria e grava command buffers para cada framebuffer
// ------------------------------------------------------
void Renderer::createCommandBuffers() {
    m_commandBuffers.resize(m_framebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

    if (vkAllocateCommandBuffers(m_device, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Falha ao alocar command buffers!");
    }

    // Para cada command buffer, vamos iniciar a render pass e encerrar
    for (size_t i = 0; i < m_commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        // (Pode ser necessário ajustar conforme seu caso)

        if (vkBeginCommandBuffer(m_commandBuffers[i], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Falha ao iniciar gravação do command buffer!");
        }

        // Início da render pass
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_renderPass;
        renderPassInfo.framebuffer = m_framebuffers[i];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = m_swapChain.getExtent();

        VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(m_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Vincula o pipeline gráfico
        vkCmdBindPipeline(m_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline->getPipeline());

        // Para teste, desenha um triângulo simples (3 vértices, 1 instância)
        m_model->recordDrawCommands(m_commandBuffers[i]);

        vkCmdEndRenderPass(m_commandBuffers[i]);

        // Encerra gravação
        if (vkEndCommandBuffer(m_commandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Falha ao gravar command buffer!");
        }
    }
}

// ------------------------------------------------------
// Cria semáforos e fence para sincronizar renderização
// ------------------------------------------------------
void Renderer::createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // já sinalizada pra evitar deadlock inicial

    if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("Falha ao criar semáforos/fence!");
    }
}

// ------------------------------------------------------
// Realiza o desenho de um frame (adquire imagem, submete
// command buffer, apresenta na tela)
// ------------------------------------------------------
void Renderer::drawFrame() const {
    // Espera o frame anterior terminar
    vkWaitForFences(m_device, 1, &m_inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(m_device, 1, &m_inFlightFence);

    // Adquire índice da próxima imagem da swapchain
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
        m_device,
        m_swapChain.getSwapChain(),
        UINT64_MAX,
        m_imageAvailableSemaphore,   // sinalizado quando a swapchain está pronta
        VK_NULL_HANDLE,
        &imageIndex
    );

    // Exemplos de tratamento
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        // SwapChain precisa ser recriada (janela redimensionada, etc.)
        // (Você chamaria uma função de recriação da swapchain)
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Falha ao adquirir imagem da swapchain!");
    }

    // Prepara a submissão do command buffer
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // Esperamos até a imagem estar disponível
    VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    // Enviamos para a GPU o command buffer correspondente ao imageIndex
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffers[imageIndex];

    // Sinalizamos que terminamos de desenhar
    VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    // Submete à fila gráfica
    if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("Falha ao submeter draw command buffer!");
    }

    // Apresenta a imagem na tela
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores; // espera a renderização acabar

    VkSwapchainKHR swapChains[] = { m_swapChain.getSwapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(m_presentQueue, &presentInfo);

    // Novamente, podemos tratar VK_ERROR_OUT_OF_DATE_KHR (swapchain desatualizada)
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        // Recria swapchain se necessário
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Falha ao apresentar imagem na swapchain!");
    }
}
