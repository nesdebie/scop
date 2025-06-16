/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vertex.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 08:30:49 by nesdebie          #+#    #+#             */
/*   Updated: 2025/06/03 09:43:37 by nesdebie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VERTEX_H
# define VERTEX_H
# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <vulkan/vulkan.h>
# include <array>

#define MAX_LIGHTS 8

struct alignas(16) UniformBufferObject {
    glm::mat4 model;                       // offset 0
    glm::mat4 view;                        // offset 64
    glm::mat4 proj;                        // offset 128

    glm::vec3 cameraPos;                   // offset 192 (12 bytes)
    int       lightMode;                       // offset 204 (4 bytes)

    glm::vec3 objectCenter;                // offset 208 (12 bytes)
    float     spotCosCutoff;               // offset 220 (4 bytes)

    // std140 requires array-stride of 16 bytes:
    glm::vec4 lightPositions[MAX_LIGHTS];  // offset 224 (8×16 = 128 bytes)
    glm::vec4 lightIntensities[MAX_LIGHTS];// offset 352 (8×16 = 128 bytes)

    int       numLights;                   // offset 480 (4 bytes)
    int       isLightOff;                  // offset 484 (4 bytes)
    int       _pad1[2];                    // offset 488, 492 (8 bytes)
};




struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
};

#endif