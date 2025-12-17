#include "Renderer.h"

void vkCheck(VkResult result, const char* msg) {
    if (result != VK_SUCCESS) {
        throw std::runtime_error(msg);
    }
}

Renderer::Renderer() {
    this->instance = VK_NULL_HANDLE;
    this->surface = VK_NULL_HANDLE;
    this->renderPass = VK_NULL_HANDLE;
    this->commandPool = VK_NULL_HANDLE;
    this->uniformBuffer = VK_NULL_HANDLE;
    this->uniformBufferMemory = VK_NULL_HANDLE;
    this->fallbackUniformBuffer = VK_NULL_HANDLE;
    this->fallbackUniformBufferMemory = VK_NULL_HANDLE;
    this->descriptorPool = VK_NULL_HANDLE;
    this->depthImage = VK_NULL_HANDLE;
    this->depthImageMemory = VK_NULL_HANDLE;
    this->depthImageView = VK_NULL_HANDLE;
    this->isLightOff = 0;
    this->appliedTexture = 0;
    this->lightMode = 0;
    this->textureToggled = false;
    this->firstFrameDrawn = false;
    this->keyInteracted = false;
}

Renderer::~Renderer() {}

void Renderer::cleanup() {
    vkDeviceWaitIdle(vulkanDevice.getDevice());
    
    for (auto& mesh : gpuMeshes) {
        if (mesh.textureSampler != VK_NULL_HANDLE) {
            vkDestroySampler(vulkanDevice.getDevice(), mesh.textureSampler, nullptr);
        }
        if (mesh.textureImageView != VK_NULL_HANDLE) {
            vkDestroyImageView(vulkanDevice.getDevice(), mesh.textureImageView, nullptr);
        }
        if (mesh.textureImage != VK_NULL_HANDLE) {
            vkDestroyImage(vulkanDevice.getDevice(), mesh.textureImage, nullptr);
        }
        if (mesh.textureMemory != VK_NULL_HANDLE) {
            vkFreeMemory(vulkanDevice.getDevice(), mesh.textureMemory, nullptr);
        }
        if (mesh.vertexBuffer != VK_NULL_HANDLE) {
            vkDestroyBuffer(vulkanDevice.getDevice(), mesh.vertexBuffer, nullptr);
        }
        if (mesh.vertexMemory != VK_NULL_HANDLE) {
            vkFreeMemory(vulkanDevice.getDevice(), mesh.vertexMemory, nullptr);
        }
        if (mesh.indexBuffer != VK_NULL_HANDLE) {
            vkDestroyBuffer(vulkanDevice.getDevice(), mesh.indexBuffer, nullptr);
        }
        if (mesh.indexMemory != VK_NULL_HANDLE) {
            vkFreeMemory(vulkanDevice.getDevice(), mesh.indexMemory, nullptr);
        }
        if (mesh.materialBuffer != VK_NULL_HANDLE) {
            vkDestroyBuffer(vulkanDevice.getDevice(), mesh.materialBuffer, nullptr);
        }
        if (mesh.materialBufferMemory != VK_NULL_HANDLE) {
            vkFreeMemory(vulkanDevice.getDevice(), mesh.materialBufferMemory, nullptr);
        }
    }
    gpuMeshes.clear();

    vkDestroyBuffer(vulkanDevice.getDevice(), uniformBuffer, nullptr);
    vkFreeMemory(vulkanDevice.getDevice(), uniformBufferMemory, nullptr);
    vkDestroyBuffer(vulkanDevice.getDevice(), fallbackUniformBuffer, nullptr);
    vkFreeMemory(vulkanDevice.getDevice(), fallbackUniformBufferMemory, nullptr);

    vkDestroyImageView(vulkanDevice.getDevice(), depthImageView, nullptr);
    vkDestroyImage(vulkanDevice.getDevice(), depthImage, nullptr);
    vkFreeMemory(vulkanDevice.getDevice(), depthImageMemory, nullptr);

    for (auto framebuffer : swapChainFrameBuffers) {
        vkDestroyFramebuffer(vulkanDevice.getDevice(), framebuffer, nullptr);
    }
    swapChainFrameBuffers.clear();

    vulkanPipeline.cleanup(vulkanDevice.getDevice());
    vkDestroyRenderPass(vulkanDevice.getDevice(), renderPass, nullptr);

    destroyDescriptorPool();

    if (!commandBuffers.empty()) {
        vkFreeCommandBuffers(vulkanDevice.getDevice(), commandPool, 
                            static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
        commandBuffers.clear();
    }

    vkDestroyCommandPool(vulkanDevice.getDevice(), commandPool, nullptr);
    vulkanSwapchain.cleanup(vulkanDevice.getDevice());
    vulkanDevice.cleanup();
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool Renderer::init(const std::vector<MeshPackage>& meshPackages) {
    initWindow();
    createInstance();
    createSurface();
    vulkanDevice.init(instance, surface);
    createCommandPool();
    vulkanSwapchain.init(vulkanDevice.getPhysicalDevice(), vulkanDevice.getDevice(), 
                         surface, vulkanDevice.getGraphicsFamily());
    createDepthResources();
    createRenderPass();
    vulkanPipeline.init(vulkanDevice.getDevice(), renderPass, vulkanSwapchain.getExtent());
    createUniformBuffer();
    createFallbackUniformBuffer();

    for (const auto& pkg : meshPackages) {
        GpuMesh mesh;
        mesh.indexCount = pkg.indices.size();
        mesh.textureFile = pkg.textureFile;
        mesh.hasMapKdInitially = pkg.hasMapKdInitially;
        mesh.originalDiffuseColor = pkg.diffuseColor;
        mesh.textureImageView = VK_NULL_HANDLE;
        mesh.textureSampler = VK_NULL_HANDLE;
        mesh.textureMemory = VK_NULL_HANDLE;
        mesh.textureImage = VK_NULL_HANDLE;
        
        vulkanBuffer.createVertexBuffer(vulkanDevice.getDevice(), vulkanDevice.getPhysicalDevice(),
                                       pkg.vertices, mesh.vertexBuffer, mesh.vertexMemory);
        vulkanBuffer.createIndexBuffer(vulkanDevice.getDevice(), vulkanDevice.getPhysicalDevice(),
                                      pkg.indices, mesh.indexBuffer, mesh.indexMemory);
        
        if (!pkg.textureFile.empty()) {
            vulkanTexture.createTextureImage(vulkanDevice.getDevice(), vulkanDevice.getPhysicalDevice(),
                                            commandPool, vulkanDevice.getGraphicsQueue(),
                                            "models/" + pkg.textureFile, mesh.textureImage,
                                            mesh.textureMemory, mesh.textureImageView, mesh.textureSampler);
        }
    
        MaterialUBO mat{};
        mat.diffuse = pkg.textureFile.empty() ? pkg.diffuseColor : my_glm::vec3(0.0f);
        mat.useTexture = pkg.textureFile.empty() ? 0 : 1;

        vulkanBuffer.createBuffer(vulkanDevice.getDevice(), vulkanDevice.getPhysicalDevice(),
                                 sizeof(MaterialUBO), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                 mesh.materialBuffer, mesh.materialBufferMemory);

        void* data;
        vkMapMemory(vulkanDevice.getDevice(), mesh.materialBufferMemory, 0, sizeof(MaterialUBO), 0, &data);
        memcpy(data, &mat, sizeof(MaterialUBO));
        vkUnmapMemory(vulkanDevice.getDevice(), mesh.materialBufferMemory);
        gpuMeshes.push_back(mesh);
    }
    
    createDescriptorPool();
    for (auto& mesh : gpuMeshes) {
        createDescriptorSet(mesh);
    }
    createFramebuffers();
    createCommandBuffers();
    
    camera.init(objectRadius);
    inputHandler.init();
    
    return true;
}

void Renderer::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "scop is dope", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mouseMoveCallback);
}

void Renderer::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    (void)xoffset;
    auto* renderer = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
    if (!renderer) return;
    renderer->inputHandler.handleScroll(yoffset, renderer->camera, renderer->keyInteracted);
}

void Renderer::mouseButtonCallback(GLFWwindow* window, int button, int action, int /*mods*/) {
    auto* renderer = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
    if (!renderer) return;
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    renderer->inputHandler.handleMouseButton(button, action, xpos, ypos);
}

void Renderer::mouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
    auto* renderer = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
    if (!renderer) return;
    renderer->inputHandler.handleMouseMove(xpos, ypos, renderer->camera);
}

void Renderer::createInstance() {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "OBJ Viewer";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    vkCheck(vkCreateInstance(&createInfo, nullptr, &instance), "Failed to create Vulkan instance!");
}

void Renderer::createSurface() {
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
        throw std::runtime_error("Failed to create window surface!");
}

void Renderer::createCommandPool() {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = vulkanDevice.getGraphicsFamily();
    poolInfo.flags = 0;
    vkCheck(vkCreateCommandPool(vulkanDevice.getDevice(), &poolInfo, nullptr, &commandPool), 
            "Failed to create command pool!");
}

void Renderer::createDepthResources() {
    VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
    vulkanTexture.createImage(vulkanDevice.getDevice(), vulkanDevice.getPhysicalDevice(),
                             vulkanSwapchain.getExtent().width, vulkanSwapchain.getExtent().height,
                             depthFormat, VK_IMAGE_TILING_OPTIMAL,
                             VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
    depthImageView = vulkanTexture.createImageView(vulkanDevice.getDevice(), depthImage, depthFormat);
    vulkanTexture.transitionImageLayout(vulkanDevice.getDevice(), commandPool,
                                        vulkanDevice.getGraphicsQueue(), depthImage,
                                        VK_IMAGE_LAYOUT_UNDEFINED,
                                        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void Renderer::createRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = vulkanSwapchain.getImageFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = VK_FORMAT_D32_SFLOAT;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    vkCheck(vkCreateRenderPass(vulkanDevice.getDevice(), &renderPassInfo, nullptr, &renderPass), 
            "Failed to create render pass!");
}

void Renderer::createUniformBuffer() {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    vulkanBuffer.createBuffer(vulkanDevice.getDevice(), vulkanDevice.getPhysicalDevice(),
                             bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             uniformBuffer, uniformBufferMemory);
}

void Renderer::createFallbackUniformBuffer() {
    int flag = 1;
    VkDeviceSize bufferSize = sizeof(int);
    vulkanBuffer.createBuffer(vulkanDevice.getDevice(), vulkanDevice.getPhysicalDevice(),
                             bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             fallbackUniformBuffer, fallbackUniformBufferMemory);
    void* data;
    vkMapMemory(vulkanDevice.getDevice(), fallbackUniformBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, &flag, bufferSize);
    vkUnmapMemory(vulkanDevice.getDevice(), fallbackUniformBufferMemory);
}

void Renderer::createDescriptorPool() {
    uint32_t meshCount = static_cast<uint32_t>(gpuMeshes.size());
    std::array<VkDescriptorPoolSize, 3> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = meshCount;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = meshCount;
    poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[2].descriptorCount = meshCount;
    
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = meshCount;

    vkCheck(vkCreateDescriptorPool(vulkanDevice.getDevice(), &poolInfo, nullptr, &descriptorPool), 
            "Failed to create descriptor pool!");
}

void Renderer::createDescriptorSet(GpuMesh& mesh) {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    VkDescriptorSetLayout layout = vulkanPipeline.getDescriptorSetLayout();
    allocInfo.pSetLayouts = &layout;
    vkCheck(vkAllocateDescriptorSets(vulkanDevice.getDevice(), &allocInfo, &mesh.descriptorSet), 
            "Failed to allocate descriptor set");

    VkDescriptorBufferInfo uboInfo = { uniformBuffer, 0, sizeof(UniformBufferObject) };
    VkDescriptorBufferInfo materialInfo = { mesh.materialBuffer, 0, sizeof(MaterialUBO) };

    VkDescriptorImageInfo imageInfo{};
    if (!mesh.textureFile.empty()) {
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = mesh.textureImageView;
        imageInfo.sampler = mesh.textureSampler;
    } else {
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = VK_NULL_HANDLE;
        imageInfo.sampler = VK_NULL_HANDLE;
    }

    std::array<VkWriteDescriptorSet, 3> descriptorWrites{};
    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = mesh.descriptorSet;
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &uboInfo;

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = mesh.descriptorSet;
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pImageInfo = &imageInfo;

    descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[2].dstSet = mesh.descriptorSet;
    descriptorWrites[2].dstBinding = 2;
    descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[2].descriptorCount = 1;
    descriptorWrites[2].pBufferInfo = &materialInfo;

    std::vector<VkWriteDescriptorSet> writes = { descriptorWrites[0], descriptorWrites[2] };
    if (imageInfo.imageView != VK_NULL_HANDLE && imageInfo.sampler != VK_NULL_HANDLE)
        writes.insert(writes.begin() + 1, descriptorWrites[1]);
    
    vkUpdateDescriptorSets(vulkanDevice.getDevice(), static_cast<uint32_t>(writes.size()), 
                          writes.data(), 0, nullptr);
}

void Renderer::createFramebuffers() {
    const std::vector<VkImageView>& imageViews = vulkanSwapchain.getImageViews();
    swapChainFrameBuffers.resize(imageViews.size());
    
    for (size_t i = 0; i < imageViews.size(); i++) {
        std::array<VkImageView, 2> attachments = { imageViews[i], depthImageView };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = vulkanSwapchain.getExtent().width;
        framebufferInfo.height = vulkanSwapchain.getExtent().height;
        framebufferInfo.layers = 1;

        vkCheck(vkCreateFramebuffer(vulkanDevice.getDevice(), &framebufferInfo, nullptr, 
                                   &swapChainFrameBuffers[i]), "Failed to create framebuffer!");
    }
}

void Renderer::createCommandBuffers() {
    commandBuffers.resize(swapChainFrameBuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(vulkanDevice.getDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate command buffers!");

    for (size_t i = 0; i < commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(commandBuffers[i], &beginInfo);

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFrameBuffers[i];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = vulkanSwapchain.getExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, 
                         vulkanPipeline.getPipeline());

        for (const auto& mesh : gpuMeshes) {
            vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                                   vulkanPipeline.getPipelineLayout(), 0, 1,
                                   &mesh.descriptorSet, 0, nullptr);
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &mesh.vertexBuffer, offsets);
            vkCmdBindIndexBuffer(commandBuffers[i], mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(commandBuffers[i], mesh.indexCount, 1, 0, 0, 0);
        }

        vkCmdEndRenderPass(commandBuffers[i]);
        vkEndCommandBuffer(commandBuffers[i]);
    }
}

void Renderer::run() {
    mainLoop();
}

void Renderer::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        handleInput();
        drawFrame();
    }
    vkDeviceWaitIdle(vulkanDevice.getDevice());
}

void Renderer::handleInput() {
    inputHandler.handleKeyboard(window, camera, objectRadius, modelRotation, lightMode,
                                isLightOff, appliedTexture, textureToggled, keyInteracted);
    
    if (textureToggled != (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)) {
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE && textureToggled) {
            toggleTexture();
        }
    }
}

void Renderer::toggleTexture() {
    for (auto& mesh : gpuMeshes) {
        if (mesh.hasMapKdInitially)
            continue;

        if (textureToggled) {
            mesh.textureFile = "tex/default.png";
            vulkanTexture.createTextureImage(vulkanDevice.getDevice(), vulkanDevice.getPhysicalDevice(),
                                            commandPool, vulkanDevice.getGraphicsQueue(),
                                            "models/" + mesh.textureFile, mesh.textureImage,
                                            mesh.textureMemory, mesh.textureImageView, mesh.textureSampler);
        } else {
            mesh.textureFile.clear();
            mesh.textureImageView = VK_NULL_HANDLE;
            mesh.textureSampler = VK_NULL_HANDLE;
        }

        MaterialUBO mat{};
        mat.useTexture = textureToggled ? 1 : 0;
        mat.diffuse = textureToggled ? my_glm::vec3(0.0f) : mesh.originalDiffuseColor;

        void* data;
        vkMapMemory(vulkanDevice.getDevice(), mesh.materialBufferMemory, 0, sizeof(MaterialUBO), 0, &data);
        memcpy(data, &mat, sizeof(MaterialUBO));
        vkUnmapMemory(vulkanDevice.getDevice(), mesh.materialBufferMemory);
    }

    destroyDescriptorPool();
    createDescriptorPool();
    for (auto& mesh : gpuMeshes)
        createDescriptorSet(mesh);
    createCommandBuffers();
}

void Renderer::destroyDescriptorPool() {
    if (descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(vulkanDevice.getDevice(), descriptorPool, nullptr);
        descriptorPool = VK_NULL_HANDLE;
    }
}

void Renderer::drawFrame() {
    auto* renderer = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));

    if (!this->keyInteracted && !renderer->inputHandler.isLeftMousePressed() && firstFrameDrawn)
        return;
        
    updateUniformBuffer();
    uint32_t imageIndex;
    vkAcquireNextImageKHR(vulkanDevice.getDevice(), vulkanSwapchain.getSwapchain(),
                         UINT64_MAX, VK_NULL_HANDLE, VK_NULL_HANDLE, &imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

    vkQueueSubmit(vulkanDevice.getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.swapchainCount = 1;
    VkSwapchainKHR swapChain = vulkanSwapchain.getSwapchain();
    presentInfo.pSwapchains = &swapChain;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pWaitSemaphores = nullptr;

    vkQueuePresentKHR(vulkanDevice.getPresentQueue(), &presentInfo);
    vkQueueWaitIdle(vulkanDevice.getPresentQueue());
    firstFrameDrawn = true;
    this->keyInteracted = false;
}

void Renderer::updateUniformBuffer() {
    UniformBufferObject ubo{};

    ubo.model = my_glm::mat4(1.0f);
    ubo.model = my_glm::rotate(ubo.model, modelRotation.x, my_glm::vec3(1,0,0));
    ubo.model = my_glm::rotate(ubo.model, modelRotation.y, my_glm::vec3(0,1,0));
    ubo.model = my_glm::rotate(ubo.model, modelRotation.z, my_glm::vec3(0,0,1));
    ubo.model = my_glm::translate(ubo.model, camera.getOffset());
    
    my_glm::vec3 objectCenter = {0.0f, 0.0f, 0.0f};
    my_glm::vec3 cameraPos = camera.getCameraPosition();

    ubo.view = camera.computeViewMatrix();
    ubo.proj = my_glm::perspective(
                   my_glm::radians(45.0f),
                   float(vulkanSwapchain.getExtent().width) / vulkanSwapchain.getExtent().height,
                   0.1f, WINDOW_DEPTH
               );
    ubo.proj[1][1] *= -1;
    ubo.cameraPos = cameraPos;
    ubo.lightMode = lightMode;
    ubo.objectCenter = objectCenter;
    ubo.spotCosCutoff = cos(my_glm::radians(90.0f));
    
    int idx = 0;
    for (int xi = -1; xi <= 1; xi += 2) {
        for (int yi = -1; yi <= 1; yi += 2) {
            for (int zi = -1; zi <= 1; zi += 2) {
                my_glm::vec3 dir = my_glm::normalize(my_glm::vec3(float(xi), float(yi), float(zi)));
                my_glm::vec3 worldPos = objectCenter + objectRadius * dir;
                ubo.lightPositions[idx] = my_glm::vec4(worldPos, 1.0f);
                ubo.lightIntensities[idx] = my_glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
                ++idx;
            }
        }
    }
    ubo.numLights = idx;
    ubo.isLightOff = this->isLightOff;
    ubo.tex = appliedTexture;
    ubo._pad1 = 0;

    void* data;
    vkMapMemory(vulkanDevice.getDevice(), uniformBufferMemory, 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(vulkanDevice.getDevice(), uniformBufferMemory);
}
