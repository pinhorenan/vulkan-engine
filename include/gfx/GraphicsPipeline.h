#ifndef VKE_GRAPHICSPIPELINE_H
#define VKE_GRAPHICSPIPELINE_H

#include <vulkan/vulkan.h>
#include <string>
#include <vector>

namespace vke {

// Classe que encapsula a criação e gerenciamento do pipeline gráfico
class GraphicsPipeline {
public:
    /**
     * Construtor
     * @param device: o dispositivo lógico Vulkan
     * @param renderPass: a render pass com a qual o pipeline se integrará
     * @param swapChainExtent: a extensão (dimensões) da swapchain
     */
    GraphicsPipeline(VkDevice device, VkRenderPass renderPass, VkExtent2D swapChainExtent);

    ~GraphicsPipeline();

    /// Retorna o pipeline gráfico criado
    [[nodiscard]] VkPipeline getPipeline() const { return m_graphicsPipeline; }

    /// Retorna o pipeline layout
    [[nodiscard]] VkPipelineLayout getPipelineLayout() const { return m_pipelineLayout; }

private:
    /// Cria o pipeline gráfico (inclui criação dos módulos de shader, layout e pipeline propriamente dito)
    void createPipeline(VkRenderPass renderPass, VkExtent2D swapChainExtent);

    /// Cria um módulo de shader a partir do código SPIR-V
    VkShaderModule createShaderModule(const std::vector<char>& code);

    /// Função utilitária para carregar um arquivo (por exemplo, o shader compilado) para um vetor de bytes
    static std::vector<char> readFile(const std::string& filename);

private:
    VkDevice m_device;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
};

} // namespace vke

#endif // VKE_GRAPHICSPIPELINE_H
