#include "Device.h"

void vkCheck(VkResult result, const char* msg);

Device::Device() {
    this->physicalDevice = VK_NULL_HANDLE;
    this->device = VK_NULL_HANDLE;
    this->graphicsQueue = VK_NULL_HANDLE;
    this->presentQueue = VK_NULL_HANDLE;
    this->graphicsFamily = -1;
}

Device::~Device() {}

void Device::init(VkInstance instance, VkSurfaceKHR surface) {
    pickPhysicalDevice(instance);
    findQueueFamilies(surface);
    createLogicalDevice();
}

void Device::cleanup() {
    if (device != VK_NULL_HANDLE) {
        vkDestroyDevice(device, nullptr);
        device = VK_NULL_HANDLE;
    }
}

void Device::pickPhysicalDevice(VkInstance instance) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0)
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    physicalDevice = devices[0];
}

void Device::findQueueFamilies(VkSurfaceKHR surface) {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    int graphicsFamily = -1;
    for (size_t i = 0; i < queueFamilies.size(); i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsFamily = i;
            break;
        }
    }

    if (graphicsFamily == -1)
        throw std::runtime_error("Failed to find a graphics queue family!");

    VkBool32 presentSupport = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, graphicsFamily, surface, &presentSupport);
    if (!presentSupport)
        throw std::runtime_error("Graphics queue family does not support presentation!");

    this->graphicsFamily = graphicsFamily;
}

void Device::createLogicalDevice() {
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = graphicsFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures{};
    const char* deviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = 1;
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions;

    vkCheck(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device), "Failed to create logical device!");
    vkGetDeviceQueue(device, graphicsFamily, 0, &graphicsQueue);
    presentQueue = graphicsQueue;
}

uint32_t Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }
    throw std::runtime_error("Failed to find suitable memory type!");
}
