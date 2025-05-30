/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vertex.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 08:30:49 by nesdebie          #+#    #+#             */
/*   Updated: 2025/05/30 12:04:02 by nesdebie         ###   ########.fr       */
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
    glm::mat4 model;          // 64
    glm::mat4 view;           // 64
    glm::mat4 proj;           // 64

    glm::vec3 cameraPos;      // 12
    float     _pad0;          //  4

    // std140 arrays must be spaced as vec4s:
    glm::vec4 lightPositions[MAX_LIGHTS];   // 8 × 16 = 128 bytes
    glm::vec4 lightIntensities[MAX_LIGHTS]; // 8 × 16 = 128 bytes

    int       numLights;      // 4
    int       isLightOff;     // 4
    int       _pad1[2];       // 8 to round up to 16
};



struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec3 color;
    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
};

#endif