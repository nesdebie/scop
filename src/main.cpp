#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>    // for EXIT_FAILURE
#include <ctime>      // for time(), srand()
#include "objLoader.hpp"
#include "matrix.hpp"

// Simple Vulkan + GLFW 3D OBJ Viewer
// Missing Vulkan init/cleanup functions are standard boilerplate.

GLFWwindow* window;
VkInstance instance;
VkPhysicalDevice physicalDevice;
VkDevice device;
VkQueue graphicsQueue;
VkSurfaceKHR surface;
VkSwapchainKHR swapchain;
VkExtent2D swapchainExtent;

std::vector<Vertex> vertices;
std::vector<uint32_t> indices;

// Uniform data
float rotationAngle = 0.0f;
float blend = 0.0f;
float targetBlend = 0.0f;

void initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(800, 600, "OBJ Viewer", nullptr, nullptr);
    glfwSetKeyCallback(window, [](GLFWwindow* win, int key, int sc, int action, int mods) {
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            if (key == GLFW_KEY_W)        { /* move object/camera +Y */ }
            else if (key == GLFW_KEY_S)   { /* move object/camera -Y */ }
            else if (key == GLFW_KEY_A)   { /* move object/camera -X */ }
            else if (key == GLFW_KEY_D)   { /* move object/camera +X */ }
            else if (key == GLFW_KEY_Q)   { /* move object/camera +Z */ }
            else if (key == GLFW_KEY_E)   { /* move object/camera -Z */ }
            else if (key == GLFW_KEY_T) {
                // toggle between color mode and textured mode
                targetBlend = (targetBlend < 0.5f ? 1.0f : 0.0f);
            }
        }
    });
}

void mainLoop() {
    auto last = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // update timing
        auto now = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(now - last).count();
        last = now;

        // rotate continuously
        rotationAngle += dt;

        // smooth blend towards targetBlend
        blend += (targetBlend - blend) * dt * 4.0f;

        // updateUniforms(rotationAngle, blend);
        // drawFrame();
    }

    vkDeviceWaitIdle(device);
}

int main(int argc, char** argv) {
    // seed RNG for per-face coloring
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // require an OBJ file path argument
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <model.obj>\n";
        return EXIT_FAILURE;
    }
    const char* objPath = argv[1];

    initWindow();
    // Standard Vulkan setup:
    //   createInstance();
    //   pickPhysicalDevice();
    //   createLogicalDevice();
    //   createSwapchain();
    //   createRenderPass();
    //   createDescriptorSetLayout();
    //   createGraphicsPipeline();
    //   createFramebuffers();
    //   createCommandPool();
    //   createVertexBuffer(vertices);
    //   createIndexBuffer(indices);
    //   createUniformBuffers();
    //   createDescriptorPool();
    //   createDescriptorSets();
    //   createCommandBuffers();
    //   createSyncObjects();

    if (!loadOBJ(objPath, vertices, indices)) {
        std::cerr << "Failed to load OBJ: \"" << objPath << "\"\n";
        return EXIT_FAILURE;
    }

    // after loading, you'd upload vertices & indices to GPU here...

    mainLoop();

    // cleanup:
    //   cleanupSwapchain();
    //   cleanupUniformBuffers();
    //   cleanupDescriptorPool();
    //   cleanupDescriptorSetLayout();
    //   cleanupBuffers();
    //   cleanupCommandPool();
    //   cleanupFramebuffers();
    //   cleanupPipeline();
    //   cleanupRenderPass();
    //   cleanupDevice();
    //   cleanupInstance();
    //   glfwDestroyWindow(window);
    //   glfwTerminate();

    return 0;
}
