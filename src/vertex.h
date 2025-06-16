/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vertex.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 08:30:49 by nesdebie          #+#    #+#             */
/*   Updated: 2025/06/16 09:34:07 by nesdebie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VERTEX_H
# define VERTEX_H

# include "my_glm.h"
# include <vulkan/vulkan.h>
# include <array>

#define MAX_LIGHTS 8

struct alignas(16) UniformBufferObject {
    my_glm::mat4 model;                       // offset 0
    my_glm::mat4 view;                        // offset 64
    my_glm::mat4 proj;                        // offset 128

    my_glm::vec3 cameraPos;                   // offset 192 (12 bytes)
    int       lightMode;                       // offset 204 (4 bytes)

    my_glm::vec3 objectCenter;                // offset 208 (12 bytes)
    float     spotCosCutoff;               // offset 220 (4 bytes)

    // std140 requires array-stride of 16 bytes:
    my_glm::vec4 lightPositions[MAX_LIGHTS];  // offset 224 (8×16 = 128 bytes)
    my_glm::vec4 lightIntensities[MAX_LIGHTS];// offset 352 (8×16 = 128 bytes)

    int       numLights;                   // offset 480 (4 bytes)
    int       isLightOff;                  // offset 484 (4 bytes)
    int       _pad1[2];                    // offset 488, 492 (8 bytes)
};




struct Vertex {
    my_glm::vec3 position;
    my_glm::vec3 normal;
    my_glm::vec2 texCoord;
    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
};

#endif