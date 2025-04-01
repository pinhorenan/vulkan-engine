#include "core/SwapChain.h"

#include <algorithm>
#include <stdexcept>

// ---------------------------------------------------------------------
// Construtor: recebe parâmetros e chama createSwapChain() para criar tudo
// ---------------------------------------------------------------------
SwapChain::SwapChain(VkDevice device,
                     VkPhysicalDevice physicalDevice,
                     VkSurfaceKHR surface,
                     uint32_t width,
                     uint32_t height,
                     uint32_t graphicsFamily,
                     uint32_t presentFamily)
    : m_device(device),
      m_physicalDevice(physicalDevice),
      m_surface(surface),
      m_width(width),
      m_height(height),
      m_graphicsFamily(graphicsFamily),
      m_presentFamily(presentFamily) {
    createSwapChain();
}

// ---------------------------------------------------------------------
// Destrutor: destrói os image views e a swapchain
// ---------------------------------------------------------------------
SwapChain::~SwapChain() {
    for (auto imageView : m_imageViews) {
        vkDestroyImageView(m_device, imageView, nullptr);
    }
    vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
}

// ---------------------------------------------------------------------
// Métodos estáticos para consultar e escolher parâmetros da swapchain
// ---------------------------------------------------------------------

SwapChainSupportDetails SwapChain::querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    // Procura pelo formato VK_FORMAT_B8G8R8A8_SRGB e cor VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    // Se não encontrar o ideal, retorna o primeiro
    return availableFormats[0];
}

VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    // Tenta usar MAILBOX para baixa latência e evitar tearing
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }
    // FIFO é garantido
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,
                                       uint32_t width,
                                       uint32_t height) {
    // Se não for (UINT32_MAX), use o currentExtent
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        VkExtent2D actualExtent = { width, height };

        actualExtent.width = std::max(capabilities.minImageExtent.width,
                                      std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height,
                                       std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

// ---------------------------------------------------------------------
// Criação efetiva da swapchain e dos image views
// ---------------------------------------------------------------------
void SwapChain::createSwapChain() {
    // Cria a swapchain usando a função interna
    m_swapChain = createSwapChainInternal();

    // Recupera as imagens da swapchain
    m_images = getSwapChainImages(m_swapChain);

    // Define formato e extensão
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_physicalDevice, m_surface);
    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, m_width, m_height);
    m_imageFormat = surfaceFormat.format;
    m_extent = extent;

    // Cria image views para cada imagem da swapchain
    for (auto image : m_images) {
        VkImageView imageView = createImageView(image, m_imageFormat);
        m_imageViews.push_back(imageView);
    }
}

// ---------------------------------------------------------------------
// Função que configura e chama vkCreateSwapchainKHR
// ---------------------------------------------------------------------
VkSwapchainKHR SwapChain::createSwapChainInternal() const {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_physicalDevice, m_surface);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, m_width, m_height);

    // Define a quantidade de imagens na swapchain
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    // Prepara VkSwapchainCreateInfoKHR
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // Se famílias de fila forem diferentes, usa CONCURRENT
    if (m_graphicsFamily != m_presentFamily) {
        uint32_t queueFamilyIndices[] = { m_graphicsFamily, m_presentFamily };
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    // Cria a swapchain
    VkSwapchainKHR swapChain;
    if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        throw std::runtime_error("Fail to create swap chain!");
    }
    return swapChain;
}

// ---------------------------------------------------------------------
// Recupera as imagens da swapchain criada
// ---------------------------------------------------------------------
std::vector<VkImage> SwapChain::getSwapChainImages(VkSwapchainKHR swapChain) const {
    uint32_t imageCount = 0;
    vkGetSwapchainImagesKHR(m_device, swapChain, &imageCount, nullptr);

    std::vector<VkImage> images(imageCount);
    vkGetSwapchainImagesKHR(m_device, swapChain, &imageCount, images.data());

    return images;
}

// ---------------------------------------------------------------------
// Cria um VkImageView para cada imagem da swapchain
// ---------------------------------------------------------------------
VkImageView SwapChain::createImageView(VkImage image, VkFormat format) const {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(m_device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("Fail to create image view!");
    }
    return imageView;
}
