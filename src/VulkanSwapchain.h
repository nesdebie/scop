#ifndef VULKAN_SWAPCHAIN_H
#define VULKAN_SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>

class VulkanSwapchain {
    public:
        VulkanSwapchain();
        ~VulkanSwapchain();

        void init(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, int graphicsFamily);
        void cleanup(VkDevice device);

        VkSwapchainKHR getSwapchain() const { return swapChain; }
        VkFormat getImageFormat() const { return swapChainImageFormat; }
        VkExtent2D getExtent() const { return swapChainExtent; }
        const std::vector<VkImage>& getImages() const { return swapChainImages; }
        const std::vector<VkImageView>& getImageViews() const { return swapChainImageViews; }

    private:
        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::vector<VkImageView> swapChainImageViews;

        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        std::vector<VkSurfaceFormatKHR> surfaceFormats;
        std::vector<VkPresentModeKHR> presentModes;
        VkSurfaceFormatKHR surfaceFormat;
        VkPresentModeKHR presentMode;
        VkExtent2D extent;

        void querySwapchainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
        void chooseSwapchainDetails();
        void createSwapchain(VkDevice device, VkSurfaceKHR surface, int graphicsFamily);
        void createImageViews(VkDevice device);
};

#endif
