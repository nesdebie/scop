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
