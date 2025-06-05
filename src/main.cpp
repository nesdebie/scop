/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 13:09:53 by nesdebie          #+#    #+#             */
/*   Updated: 2025/06/05 10:42:03 by nesdebie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ObjLoader.h"
#include "VulkanRenderer.h"
#include <iostream>

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

static void showControls(){
    std::cout << "________CONTROLS________" << std::endl;
    std::cout << "Camera Controls:" << std::endl;
    std::cout << "Rotate: [w] [a] [s] [d]" << std::endl;
    std::cout << "Zoom: [scroll up] [scroll down]" << std::endl;
    std::cout << "Reset camera: [r]" << std::endl;
    std::cout << "Object Controls:" << std::endl;
    std::cout << "Move: [left click + scroll] [left click + move mouse]" << std::endl;
    std::cout << "Rotate: [up] [down] [left] [right]" << std::endl;
    std::cout << "Other Controls:" << std::endl;
    std::cout << "Enable/Disable lights: [l]" << std::endl;
    std::cout << "Exit: [esc] [q]" << std::endl;
}

int main(int ac, char** av) {
    if (ac != 2)
        return giveGoodUsage(av[0]);

    const char* objPath = av[1];
    std::vector<SubMesh> submeshes;
    if (!loadOBJ(objPath, submeshes))
        return failure(1);

    glm::vec3 minBounds(FLT_MAX), maxBounds(-FLT_MAX);
    for (const auto& sub : submeshes) {
        for (const auto& v : sub.vertices) {
            minBounds = glm::min(minBounds, v.position);
            maxBounds = glm::max(maxBounds, v.position);
        }
    }
    glm::vec3 center = 0.5f * (minBounds + maxBounds);
    glm::vec3 size = maxBounds - minBounds;


    float radius = glm::length(size) * 0.5f;
    VulkanRenderer renderer;
    renderer.objectCenter = center;
    renderer.objectRadius = radius;
    renderer.cameraDistance = radius * 2.2f;

    std::vector<VulkanRenderer::MeshPackage> meshPackages;
    for (const auto& sub : submeshes) {
        meshPackages.push_back({sub.vertices, sub.indices, sub.textureFile, sub.diffuseColor});
    }

    if (!renderer.init(meshPackages)) 
        return failure(2);

    showControls();
    renderer.run();
    renderer.cleanup();
    return 0;
}

