#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include <vector>
#include <string>
#include <array>
#include <stdexcept>
#include <fstream>
#include <cstring>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stb_image.h>

#include "vertex.h"

#define WINDOW_WIDTH 2400
#define WINDOW_HEIGHT 1800
#define WINDOW_DEPTH 42.0f
#define ROTATION_SPEED 0.001f

class VulkanRenderer {
    public:
        VulkanRenderer();
        ~VulkanRenderer();

        VulkanRenderer(const VulkanRenderer&) = delete;
        VulkanRenderer& operator=(const VulkanRenderer&) = delete;
        VulkanRenderer(VulkanRenderer&&) noexcept = default;
        VulkanRenderer& operator=(VulkanRenderer&&) noexcept = default;

        struct MeshPackage {
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;
            std::string textureFile;
            glm::vec3 diffuseColor;
        };
        
        bool init(const std::vector<MeshPackage>& meshPackages);
        
        void run();
        void cleanup();

        glm::vec3 objectCenter = glm::vec3(0.0f);
        float objectRadius = 1.0f;
        float cameraDistance = 2.0f;
        bool textureOverrideActive = false;
        bool textureWasLoadedInitially = false;
        float lightIntensity = 1.0f;
        bool useDefaultTexture = false;

    private:
        struct GpuMesh {
            VkBuffer vertexBuffer;
            VkDeviceMemory vertexMemory;
            VkBuffer indexBuffer;
            VkDeviceMemory indexMemory;
            uint32_t indexCount;
            std::string textureFile;
            VkImage textureImage;
            VkDeviceMemory textureMemory;
            VkImageView textureImageView;
            VkSampler textureSampler;
            VkDescriptorSet descriptorSet;
            VkBuffer materialBuffer;
            VkDeviceMemory materialBufferMemory;
        };

        struct alignas(16) MaterialUBO {
            glm::vec3 diffuse;
            float specularExponent;
            glm::vec3 ambient;
            float dissolve;
            glm::vec3 specular;
            float refractionIndex;
            glm::vec3 emissive;
            int illumModel;
            int useTexture;
        };
        
        
        std::vector<GpuMesh> gpuMeshes;

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

        int graphicsFamily = -1;
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        std::vector<VkSurfaceFormatKHR> surfaceFormats;
        std::vector<VkPresentModeKHR> presentModes;
        VkSurfaceFormatKHR surfaceFormat;
        VkPresentModeKHR presentMode;
        VkExtent2D extent;
    private:
        void initWindow();
        void mainLoop();
        void handleInput();
        void drawFrame();
        void updateUniformBuffer();
        void toggleTexture();
        void prepareTexture(const std::string& textureFilePath);

        /* VULKAN INIT HELPER FUNCTIONS */
        void createInstance();
        void createSurface();
        void pickPhysicalDevice();
        void findQueueFamilies();
        void createLogicalDevice();
        void createCommandPool();
        void querySwapchainSupport();
        void chooseSwapchainDetails();
        void createSwapchain();
        void createImageViews();
        void createRenderPass();
        void createGraphicsPipeline();
        void createFramebuffers();
        void createCommandBuffers();
        void createVertexBuffer(const std::vector<Vertex>& vertices, VkBuffer& buffer, VkDeviceMemory& memory);
        void createIndexBuffer(const std::vector<uint32_t>& indices, VkBuffer& buffer, VkDeviceMemory& memory);
        void createUniformBuffer();
        void createDescriptorSetLayout();
        void createDescriptorPool();
        void createDescriptorSet(GpuMesh & mesh);
        void createTextureImage(const std::string& path, VkImage& image, VkDeviceMemory& memory, VkImageView& view, VkSampler& sampler);
        void createTextureImageView();
        void createTextureSampler();
        void createFallbackWhiteTexture(VkImage& image, VkDeviceMemory& memory, VkImageView& view, VkSampler& sampler);
        void createFallbackUniformBuffer();
        void createDepthResources();

        VkFormat findDepthFormat();
        VkImageView createImageView(VkImage image, VkFormat format);

        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                        VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                        VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
                        VkImage& image, VkDeviceMemory& imageMemory);
        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

        static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos);

        std::vector<char> readFile(const std::string& filename);
        VkShaderModule createShaderModule(const std::vector<char>& code);
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};
#endif
