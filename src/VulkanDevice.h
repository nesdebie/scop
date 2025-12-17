#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>

class VulkanDevice {
    public:
        VulkanDevice();
        ~VulkanDevice();

        void init(VkInstance instance, VkSurfaceKHR surface);
        void cleanup();

        VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
        VkDevice getDevice() const { return device; }
        VkQueue getGraphicsQueue() const { return graphicsQueue; }
        VkQueue getPresentQueue() const { return presentQueue; }
        int getGraphicsFamily() const { return graphicsFamily; }

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

    private:
        VkPhysicalDevice physicalDevice;
        VkDevice device;
        VkQueue graphicsQueue;
        VkQueue presentQueue;
        int graphicsFamily;

        void pickPhysicalDevice(VkInstance instance);
        void findQueueFamilies(VkSurfaceKHR surface);
        void createLogicalDevice();
};

#endif
