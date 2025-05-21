// vulkan_renderer.h
#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include <vector>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <string>
#include <array>
#include <glm/glm.hpp>
#include "vertex.h"

# include <stb_image.h>

#include <stdexcept>
#include <fstream>
#include <cstring>

# define WINDOW_WIDTH 2400
# define WINDOW_HEIGHT 1800
# define WINDOW_DEPTH 42.0f
# define ROTATION_SPEED 0.001f

class VulkanRenderer {
public:
    VulkanRenderer();
    ~VulkanRenderer();

    VulkanRenderer(const VulkanRenderer&) = delete;
    VulkanRenderer& operator=(const VulkanRenderer&) = delete;
    VulkanRenderer(VulkanRenderer&&) noexcept;
    VulkanRenderer& operator=(VulkanRenderer&&) noexcept;

    bool init(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::string& textureFile);
    void run();
    void cleanup();

    glm::vec3 objectCenter = glm::vec3(0.0f);
    float objectRadius = 1.0f;
    float cameraDistance = 2.0f;
    bool textureManuallyApplied = false;
    bool textureOverrideActive = false;
    bool textureWasLoadedInitially = false;
    std::string originalTextureFile;

private:
    void createInstance();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createCommandPool();
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createDescriptorSetLayout();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createCommandBuffers();
    void createSyncObjects();
    void createVertexBuffer(const std::vector<Vertex>& vertices);
    void createIndexBuffer(const std::vector<uint32_t>& indices);
    void createUniformBuffer();
    void updateUniformBuffer();
    void createDescriptorPool();
    void createDescriptorSet();
    void createTextureImage(const std::string& texturePath);
    void createTextureImageView();
    void createTextureSampler();
    void createFallbackWhiteTexture();
    void createFallbackUniformBuffer();
    void createDepthResources();
    VkFormat findDepthFormat();


    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    VkImageView createImageView(VkImage image, VkFormat format);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    void initWindow();
    void initVulkan(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& index, const std::string& textureFile);
    void mainLoop();
    void drawFrame();
    void handleInput();

    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos);

    std::vector<char> readFile(const std::string& filename);
    VkShaderModule createShaderModule(const std::vector<char>& code);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

private:
    GLFWwindow* window = nullptr;
    VkInstance instance = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;
    VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline graphicsPipeline = VK_NULL_HANDLE;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkCommandPool commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> commandBuffers;

    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
    VkBuffer indexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;
    VkBuffer uniformBuffer = VK_NULL_HANDLE;
    VkDeviceMemory uniformBufferMemory = VK_NULL_HANDLE;
    VkBuffer fallbackUniformBuffer = VK_NULL_HANDLE;
    VkDeviceMemory fallbackUniformBufferMemory = VK_NULL_HANDLE;

    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

    VkImage textureImage = VK_NULL_HANDLE;
    VkDeviceMemory textureImageMemory = VK_NULL_HANDLE;
    VkImageView textureImageView = VK_NULL_HANDLE;
    VkSampler textureSampler = VK_NULL_HANDLE;

    VkImage depthImage = VK_NULL_HANDLE;
    VkDeviceMemory depthImageMemory = VK_NULL_HANDLE;
    VkImageView depthImageView = VK_NULL_HANDLE;

    float cameraYaw = 0.0f;
    float cameraPitch = 0.0f;

    double lastMouseX = 0.0, lastMouseY = 0.0;
    bool leftMousePressed = false;
    glm::vec3 modelOffset = glm::vec3(0.0f);
    size_t indexCount = 0;
    std::string textureFile;
};

#endif
