/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VulkanTexture.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 00:00:00 by nesdebie          #+#    #+#             */
/*   Updated: 2025/12/16 00:00:00 by nesdebie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VULKAN_TEXTURE_H
#define VULKAN_TEXTURE_H

#include <vulkan/vulkan.h>
#include <string>
#include <stdexcept>
#include <cstring>

class VulkanTexture {
    public:
        VulkanTexture();
        ~VulkanTexture();

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
