/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 20:40:29 by nesdebie          #+#    #+#             */
/*   Updated: 2025/04/15 20:51:07 by nesdebie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <GLFW/glfw3.h>
#include "src/obj_loader.hpp"
#include "src/shader.hpp"
#include "src/matrix_math.hpp"
#include "src/bmp_loader.hpp"

#define STB_IMAGE_IMPLEMENTATION

std::vector<Vertex> vertices;
std::vector<unsigned int> indices;
GLuint VAO, VBO, EBO;
GLuint shader;
GLuint textureID;
bool useTexture = false;
float rotation = 0.0f;

void key_callback(GLFWwindow* window, int key, int, int action, int)

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(800, 600, "42 OBJ Viewer", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Load the 3D object from .obj
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    loadOBJ("textures/42.obj", vertices, indices);

    // Set up VAO/VBO
    // Set up shaders
    // Set up matrices

    while (!glfwWindowShouldClose(window)) {
        // Handle input
        // Update rotation
        // Draw object with colors or texture
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
