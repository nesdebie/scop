/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vulkan_renderer.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 08:37:04 by nesdebie          #+#    #+#             */
/*   Updated: 2025/04/29 09:34:34 by nesdebie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VULKAN_RENDERER_H
# define VULKAN_RENDERER_H

# include <vector>
# include <vulkan/vulkan.h>
# include <GLFW/glfw3.h>
# include <string> 
# include <fstream>

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
    void initVulkan();
    void mainLoop();
    void drawFrame();

    VkShaderModule createShaderModule(const std::vector<char>& code);
    std::vector<char> readFile(const std::string& filename);
    void createRenderPass();
    

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

};
#endif
