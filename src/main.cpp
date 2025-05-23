/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 13:09:53 by nesdebie          #+#    #+#             */
/*   Updated: 2025/05/23 12:31:37 by nesdebie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ObjLoader.h"
#include "VulkanRenderer.h"
#include <iostream>

int main(int ac, char** av) {
    if (ac != 2) {
        std::cerr << "Usage: " << av[0] << " models/<name>.obj" << std::endl;
        return -1;
    }

    const char* objPath = av[1];
    std::vector<SubMesh> submeshes;
    if (!loadOBJ(objPath, submeshes)) {
        std::cerr << "Failed to load OBJ file." << std::endl;
        return -1;
    }

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

    if (!renderer.init(meshPackages)) {
        std::cerr << "Failed to initialize Vulkan Renderer." << std::endl;
        return -1;
    }

    renderer.run();
    renderer.cleanup();
    return 0;
}

