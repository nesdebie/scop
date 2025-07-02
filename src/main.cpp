/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 13:09:53 by nesdebie          #+#    #+#             */
/*   Updated: 2025/07/02 13:21:26 by nesdebie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ObjLoader.h"
#include "VulkanRenderer.h"
#include <iostream>
#include <cfloat>

static int giveGoodUsage(char *str){
    std::cerr << "Usage: " << str << " models/<name>.obj" << std::endl;
    return -1;  
}

static int failure(int value) {
    switch(value) {
        case 1:
            std::cerr << "Error: Invalid OBJ file." << std::endl;
            break;
        case 2:
            std::cerr << "Error: Failed to initialize Vulkan Renderer." << std::endl;
            break;
    }
    return -1;
}

static void showControls() {
    std::cout << "\n========== CONTROLS ==========\n";

    std::cout << "\n[Camera Controls]\n";
    std::cout << "  - Rotate:          W / A / S / D\n";
    std::cout << "  - Zoom:            Scroll Up / Down\n";
    std::cout << "  - Reset Camera:    R\n";

    std::cout << "\n[Object Controls]\n";
    std::cout << "  - Move:            Left Click + Scroll or Mouse Drag\n";
    std::cout << "  - Rotate:          Arrow Keys (↑ ↓ ← →)\n";

    std::cout << "\n[Lighting Controls]\n";
    std::cout << "  - L:               Toggle full material lighting (Kd-only vs. full)\n";
    std::cout << "  - Numpad 0:        Enable all lights\n";
    std::cout << "  - Numpad 9:        Disable all lights\n";
    std::cout << "  - Numpad 1–8:      Enable only one light source [1–8]\n";

    std::cout << "\n[Other Controls]\n";
    std::cout << "  - Add Texture:     T\n";
    std::cout << "  - Add PNG:         P\n";
    std::cout << "  - Exit:            ESC / Q\n";

    std::cout << "==============================\n\n";
}


int main(int ac, char** av) {
    if (ac != 2)
        return giveGoodUsage(av[0]);

    const char* objPath = av[1];
    std::vector<SubMesh> submeshes;
    if (!loadOBJ(objPath, submeshes))
        return failure(1);

    my_glm::vec3 minBounds(FLT_MAX), maxBounds(-FLT_MAX);
    for (const auto& sub : submeshes) {
        for (const auto& v : sub.vertices) {
            minBounds = my_glm::min(minBounds, v.position);
            maxBounds = my_glm::max(maxBounds, v.position);
        }
    }
    my_glm::vec3 center = 0.5f * (minBounds + maxBounds);
    for (auto& sub : submeshes) {
        for (auto& v : sub.vertices) {
            v.position -= center;
        }
    }
    my_glm::vec3 size = maxBounds - minBounds;


    float radius = my_glm::length(size) * 0.5f;
    VulkanRenderer renderer;
    renderer.objectRadius = radius;
    renderer.cameraDistance = radius * 2.2f;

    std::vector<VulkanRenderer::MeshPackage> meshPackages;
    for (const auto& sub : submeshes) {
        meshPackages.push_back({sub.vertices, sub.indices, sub.textureFile, sub.diffuseColor, sub.hasMapKdInitially});
    }

    if (!renderer.init(meshPackages)) 
        return failure(2);

    showControls();
    renderer.run();
    renderer.cleanup();
    return 0;
}

