#include "Buffer.h"
#include <stdexcept>

void vkCheck(VkResult result, const char* msg);

Buffer::Buffer() {}

Buffer::~Buffer() {}

uint32_t Buffer::findMemoryType(VkPhysicalDevice physicalDevice,
                                      uint32_t typeFilter,
                                      VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }
    throw std::runtime_error("Failed to find suitable memory type!");
}

void Buffer::createBuffer(VkDevice device, VkPhysicalDevice physicalDevice,
                                VkDeviceSize size, VkBufferUsageFlags usage,
                                VkMemoryPropertyFlags properties,
                                VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkCheck(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer), "Failed to create buffer");

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

    vkCheck(vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory), "Failed to allocate buffer memory");
    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void Buffer::createVertexBuffer(VkDevice device, VkPhysicalDevice physicalDevice,
                                      const std::vector<Vertex>& vertices,
                                      VkBuffer& buffer, VkDeviceMemory& memory) {
    if (vertices.empty())
        throw std::runtime_error("Vertex list is empty — cannot create vertex buffer.");

    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vkCheck(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer), "Failed to create vertex buffer!");

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    vkCheck(vkAllocateMemory(device, &allocInfo, nullptr, &memory), "Failed to allocate vertex buffer memory!");
    vkBindBufferMemory(device, buffer, memory, 0);

    void* data;
    vkMapMemory(device, memory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(device, memory);
}

void Buffer::createIndexBuffer(VkDevice device, VkPhysicalDevice physicalDevice,
                                     const std::vector<uint32_t>& indices,
                                     VkBuffer& buffer, VkDeviceMemory& memory) {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vkCheck(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer), "Failed to create index buffer!");

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    vkCheck(vkAllocateMemory(device, &allocInfo, nullptr, &memory), "Failed to allocate index buffer memory!");
    vkBindBufferMemory(device, buffer, memory, 0);

    void* data;
    vkMapMemory(device, memory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(device, memory);
}
