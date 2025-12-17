#ifndef VULKAN_PIPELINE_H
#define VULKAN_PIPELINE_H

#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

class VulkanPipeline {
    public:
        VulkanPipeline();
        ~VulkanPipeline();

        void init(VkDevice device, VkRenderPass renderPass, VkExtent2D swapChainExtent);
        void cleanup(VkDevice device);

        VkPipeline getPipeline() const { return graphicsPipeline; }
        VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }
        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        VkPipeline graphicsPipeline;
        VkPipelineLayout pipelineLayout;
        VkDescriptorSetLayout descriptorSetLayout;

        void createDescriptorSetLayout(VkDevice device);
        void createGraphicsPipeline(VkDevice device, VkRenderPass renderPass, VkExtent2D swapChainExtent);

        std::vector<char> readFile(const std::string& filename);
        VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code);
};

#endif
