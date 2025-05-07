/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vulkan_renderer.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 08:37:04 by nesdebie          #+#    #+#             */
/*   Updated: 2025/05/07 09:07:44 by nesdebie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VULKAN_RENDERER_H
# define VULKAN_RENDERER_H

# include <vector>
# include <vulkan/vulkan.h>
# include <GLFW/glfw3.h>
# include <string> 
# include <fstream>
# include <stdexcept>
# include <iostream>
# include <vector>
# include <cstring>

# include "vertex.h"

class VulkanRenderer {
public:
    VulkanRenderer();
    ~VulkanRenderer();

    bool init(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    void run();
    void cleanup();

private:
    void initWindow();
    void initVulkan(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& index);
    void mainLoop();
    void drawFrame();
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkShaderModule createShaderModule(const std::vector<char>& code);
    std::vector<char> readFile(const std::string& filename);
    void createRenderPass();

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    size_t indexCount = 0;

    void createVertexBuffer(const std::vector<Vertex>& vertices);
    void createIndexBuffer(const std::vector<uint32_t>& indices);
    void createDescriptorSetLayout();
    void createUniformBuffer();
    void updateUniformBuffer();
    void createDescriptorPool();
    void createDescriptorSet();



    // Window
    GLFWwindow* window;
    const int WIDTH = 1600;
    const int HEIGHT = 1200;

    // Vulkan
    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

    // VulkanRenderer class members
    VkRenderPass renderPass;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    // Command Buffers
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    // Graphics Pipeline
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    // MVP Uniform Buffer
    VkBuffer uniformBuffer;
    VkDeviceMemory uniformBufferMemory;

    // Descriptor Layout and Set
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet;

};
#endif
