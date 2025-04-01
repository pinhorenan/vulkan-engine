#ifndef VKE_SWAPCHAIN_H
#define VKE_SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include <vector>

// Mesma struct original para armazenar os detalhes de suporte da SwapChain
struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class SwapChain {
public:
    // Construtor que recebe os mesmos parâmetros necessários para criar a swapchain
    SwapChain(VkDevice device,
              VkPhysicalDevice physicalDevice,
              VkSurfaceKHR surface,
              uint32_t width,
              uint32_t height,
              uint32_t graphicsFamily,
              uint32_t presentFamily);

    // Destrutor para liberar os recursos (swapchain e image views)
    ~SwapChain();

    // Acesso aos recursos criados
    [[nodiscard]] VkSwapchainKHR getSwapChain() const { return m_swapChain; }
    [[nodiscard]] VkFormat getImageFormat() const { return m_imageFormat; }
    [[nodiscard]] VkExtent2D getExtent() const { return m_extent; }
    [[nodiscard]] const std::vector<VkImageView>& getImageViews() const { return m_imageViews; }

private:
    // --- Métodos auxiliares estáticos ---
    static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,
                                       uint32_t width,
                                       uint32_t height);

    // --- Criação e configuração da swapchain ---
    void createSwapChain();
    [[nodiscard]] VkSwapchainKHR createSwapChainInternal() const;
    std::vector<VkImage> getSwapChainImages(VkSwapchainKHR swapChain) const;
    VkImageView createImageView(VkImage image, VkFormat format) const;

private:
    // Recursos Vulkan recebidos ou calculados
    VkDevice m_device;
    VkPhysicalDevice m_physicalDevice;
    VkSurfaceKHR m_surface;
    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_graphicsFamily;
    uint32_t m_presentFamily;

    // SwapChain propriamente dita
    VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
    VkFormat m_imageFormat{};
    VkExtent2D m_extent{};

    // As imagens da swapchain e seus respectivos image views
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
};

#endif // VKE_SWAPCHAIN_H
