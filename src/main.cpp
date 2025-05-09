#include "obj_loader.h"
#include "vulkan_renderer.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <model.obj>" << std::endl;
        return -1;
    }

    const char* objPath = argv[1];

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::string textureFile;
    if (!loadOBJ(objPath, vertices, indices, textureFile)) {
        std::cerr << "Failed to load OBJ file." << std::endl;
        return -1;
    }
    std::cout << "Loaded " << vertices.size() << " vertices, " << indices.size() << " indices.\n";
    std::cout << "Using texture: " << textureFile << std::endl;
    

    VulkanRenderer renderer;
    if (!renderer.init(vertices, indices, textureFile)) {
        std::cerr << "Failed to initialize Vulkan Renderer." << std::endl;
        return -1;
    }

    renderer.run();

    renderer.cleanup();

    return 0;
}
