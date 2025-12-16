/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VulkanBuffer.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 00:00:00 by nesdebie          #+#    #+#             */
/*   Updated: 2025/12/16 00:00:00 by nesdebie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VULKAN_BUFFER_H
#define VULKAN_BUFFER_H

#include <vulkan/vulkan.h>
#include <vector>
#include <cstring>
#include "vertex.h"

class VulkanBuffer {
    public:
        VulkanBuffer();
        ~VulkanBuffer();

        void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice,
                         VkDeviceSize size, VkBufferUsageFlags usage,
                         VkMemoryPropertyFlags properties,
                         VkBuffer& buffer, VkDeviceMemory& bufferMemory);

        void createVertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice,
                               const std::vector<Vertex>& vertices,
                               VkBuffer& buffer, VkDeviceMemory& memory);

        void createIndexBuffer(VkDevice device, VkPhysicalDevice physicalDevice,
                              const std::vector<uint32_t>& indices,
                              VkBuffer& buffer, VkDeviceMemory& memory);

    private:
        uint32_t findMemoryType(VkPhysicalDevice physicalDevice,
                               uint32_t typeFilter,
                               VkMemoryPropertyFlags properties);
};

#endif
