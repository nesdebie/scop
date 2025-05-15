#include "obj_loader.h"
#include "vulkan_renderer.h"
#include <iostream>

int main(int ac, char** av) {
    if (ac != 2) {
        std::cerr << "Usage: " << av[0] << " models/<name>.obj" << std::endl;
        return -1;
    }

    const char* objPath = av[1];
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::string textureFile;
    if (!loadOBJ(objPath, vertices, indices, textureFile)) {
        std::cerr << "Failed to load OBJ file." << std::endl;
        return -1;
    }

    VulkanRenderer renderer;
    if (!renderer.init(vertices, indices, textureFile)) {
        std::cerr << "Failed to initialize Vulkan Renderer." << std::endl;
        return -1;
    }
    renderer.run();
    renderer.cleanup();
    return 0;
}
