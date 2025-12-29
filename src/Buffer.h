#ifndef VULKAN_BUFFER_H
#define VULKAN_BUFFER_H

#include <vulkan/vulkan.h>
#include <vector>
#include <cstring>
#include "vertex.h"

/**
 * Manages Vulkan buffer creation and memory allocation.
 * 
 * Handles creation of vertex and index buffers for GPU memory,
 * including memory type selection based on device properties.
 */
class Buffer {
    public:
        Buffer();
        ~Buffer();

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
