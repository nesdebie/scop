#ifndef VULKAN_TEXTURE_H
#define VULKAN_TEXTURE_H

#include <vulkan/vulkan.h>
#include <string>
#include <stdexcept>
#include <cstring>

class Texture {
    public:
        Texture();
        ~Texture();

        void createTextureImage(VkDevice device, VkPhysicalDevice physicalDevice,
                               VkCommandPool commandPool, VkQueue graphicsQueue,
                               const std::string& path, VkImage& image,
                               VkDeviceMemory& memory, VkImageView& view,
                               VkSampler& sampler);

        VkImageView createImageView(VkDevice device, VkImage image, VkFormat format);

        void createImage(VkDevice device, VkPhysicalDevice physicalDevice,
                        uint32_t width, uint32_t height, VkFormat format,
                        VkImageTiling tiling, VkImageUsageFlags usage,
                        VkMemoryPropertyFlags properties,
                        VkImage& image, VkDeviceMemory& imageMemory);

        void transitionImageLayout(VkDevice device, VkCommandPool commandPool,
                                  VkQueue graphicsQueue, VkImage image,
                                  VkImageLayout oldLayout, VkImageLayout newLayout);

    private:

        void copyBufferToImage(VkDevice device, VkCommandPool commandPool,
                              VkQueue graphicsQueue, VkBuffer buffer,
                              VkImage image, uint32_t width, uint32_t height);

        uint32_t findMemoryType(VkPhysicalDevice physicalDevice,
                               uint32_t typeFilter,
                               VkMemoryPropertyFlags properties);
};

#endif
