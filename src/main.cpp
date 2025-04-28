#include "obj_loader.h"
#include "vulkan_renderer.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <model.obj>" << std::endl;
        return -1;
    }

    const char* objPath = argv[1];

    // Parse the OBJ file
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    if (!loadOBJ(objPath, vertices, indices)) {
        std::cerr << "Failed to load OBJ file." << std::endl;
        return -1;
    }

    // Create the Vulkan renderer
    VulkanRenderer renderer;
    if (!renderer.init(vertices, indices)) {
        std::cerr << "Failed to initialize Vulkan Renderer." << std::endl;
        return -1;
    }

    // Main loop
    renderer.run();

    // Cleanup
    renderer.cleanup();

    return 0;
}
