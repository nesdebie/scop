#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include <vector>
#include <string>
#include <array>
#include <stdexcept>
#include <cstring>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "vertex.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanPipeline.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"
#include "Camera.h"
#include "InputHandler.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1440
#define WINDOW_DEPTH 42.0f

class VulkanRenderer {
    public:
        VulkanRenderer();
        ~VulkanRenderer();

        struct MeshPackage {
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;
            std::string textureFile;
            my_glm::vec3 diffuseColor;
            bool hasMapKdInitially;

        };
        
        bool init(const std::vector<MeshPackage>& meshPackages);
        
        void run();
        void cleanup();

        float           objectRadius;
        float           cameraDistance;


    private:
        struct GpuMesh {
            bool            hasMapKdInitially;
            my_glm::vec3    originalDiffuseColor;
            VkBuffer        vertexBuffer;
            VkDeviceMemory  vertexMemory;
            VkBuffer        indexBuffer;
            VkDeviceMemory  indexMemory;
            uint32_t        indexCount;
            std::string     textureFile;
            VkImage         textureImage;
            VkDeviceMemory  textureMemory;
            VkImageView     textureImageView;
            VkSampler       textureSampler;
            VkDescriptorSet descriptorSet;
            VkBuffer        materialBuffer;
            VkDeviceMemory  materialBufferMemory;
        };

        struct alignas(16) MaterialUBO {
            my_glm::vec3    diffuse;
            float           specularExponent;
            my_glm::vec3    ambient;
            float           dissolve;
            my_glm::vec3    specular;
            float           refractionIndex;
            my_glm::vec3    emissive;
            int             illumModel;
            int             useTexture;
        };
        
        std::vector<GpuMesh> gpuMeshes;

        GLFWwindow* window = nullptr;

        VkInstance          instance;
        VkSurfaceKHR        surface;
        
        VulkanDevice        vulkanDevice;
        VulkanSwapchain     vulkanSwapchain;
        VulkanPipeline      vulkanPipeline;
        VulkanBuffer        vulkanBuffer;
        VulkanTexture       vulkanTexture;
        Camera              camera;
        InputHandler        inputHandler;

        VkRenderPass renderPass;
        std::vector<VkFramebuffer> swapChainFrameBuffers;
        VkCommandPool commandPool;
        std::vector<VkCommandBuffer> commandBuffers;

        VkBuffer uniformBuffer;
        VkDeviceMemory uniformBufferMemory;
        VkBuffer fallbackUniformBuffer;
        VkDeviceMemory fallbackUniformBufferMemory;

        VkDescriptorPool descriptorPool;

        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;

        int isLightOff;
        int appliedTexture;
        bool firstFrameDrawn;

        my_glm::vec3 modelRotation;
        int lightMode;
        bool textureToggled;
        bool keyInteracted;
        
        void initWindow();
        void mainLoop();
        void handleInput();
        void drawFrame();
        void updateUniformBuffer();
        void toggleTexture();

        /* VULKAN INIT HELPER FUNCTIONS */
        void createInstance();
        void createSurface();
        void createCommandPool();
        void createRenderPass();
        void createFramebuffers();
        void createCommandBuffers();
        void createUniformBuffer();
        void createDescriptorPool();
        void createDescriptorSet(GpuMesh & mesh);
        void createFallbackUniformBuffer();
        void createDepthResources();

        static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos);

        void destroyDescriptorPool();
};
#endif
