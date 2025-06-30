/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VulkanRenderer.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 09:33:21 by nesdebie          #+#    #+#             */
/*   Updated: 2025/06/30 09:05:28 by nesdebie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
#include <stb_image.h>

#include "vertex.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1440
#define WINDOW_DEPTH 42.0f
#define ROTATION_SPEED 0.01f

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

        my_glm::vec3    objectCenter;
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
        VkPhysicalDevice    physicalDevice;
        VkDevice            device;
        VkQueue             graphicsQueue;
        VkQueue             presentQueue;

        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;

        VkRenderPass renderPass;
        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;
        std::vector<VkImageView> swapChainImageViews;
        std::vector<VkFramebuffer> swapChainFrameBuffers;
        VkCommandPool commandPool;
        std::vector<VkCommandBuffer> commandBuffers;

        VkBuffer uniformBuffer;
        VkDeviceMemory uniformBufferMemory;
        VkBuffer fallbackUniformBuffer;
        VkDeviceMemory fallbackUniformBufferMemory;

        VkDescriptorSetLayout descriptorSetLayout;
        VkDescriptorPool descriptorPool;
        VkDescriptorSet descriptorSet;

        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;

        float cameraYaw;
        float cameraPitch;
        double lastMouseX;
        double lastMouseY;
        bool leftMousePressed;
        int isLightOff;
        int appliedTexture;
        int prevLState;
        int prevTState;
        int prevPState;
        my_glm::vec3 modelOffset;

        int graphicsFamily;
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        std::vector<VkSurfaceFormatKHR> surfaceFormats;
        std::vector<VkPresentModeKHR> presentModes;
        VkSurfaceFormatKHR surfaceFormat;
        VkPresentModeKHR presentMode;
        VkExtent2D extent;

        my_glm::vec3 modelRotation;
        int lightMode;
        bool textureToggled;
        
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
        void createFallbackWhiteTexture(VkImage& image, VkDeviceMemory& memory, VkImageView& view, VkSampler& sampler);
        void createFallbackUniformBuffer();
        void createDepthResources();

        VkImageView createImageView(VkImage image, VkFormat format);

        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                        VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                        VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
                        VkImage& image, VkDeviceMemory& imageMemory);
        void transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

        static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos);

        std::vector<char> readFile(const std::string& filename);
        VkShaderModule createShaderModule(const std::vector<char>& code);
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        void destroyDescriptorPool();
        my_glm::mat4 computeViewMatrix() const;
};
#endif
