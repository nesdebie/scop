/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputHandler.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 00:00:00 by nesdebie          #+#    #+#             */
/*   Updated: 2025/12/16 00:00:00 by nesdebie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "InputHandler.h"
#include "Camera.h"
#include <cmath>

#define ROTATION_SPEED 0.01f

InputHandler::InputHandler() {
    this->leftMousePressed = false;
    this->prevLState = GLFW_RELEASE;
    this->prevTState = GLFW_RELEASE;
    this->prevPState = GLFW_RELEASE;
    this->lastMouseX = 0.0;
    this->lastMouseY = 0.0;
}

InputHandler::~InputHandler() {}

void InputHandler::init() {
    this->leftMousePressed = false;
    this->prevLState = GLFW_RELEASE;
    this->prevTState = GLFW_RELEASE;
    this->prevPState = GLFW_RELEASE;
    this->lastMouseX = 0.0;
    this->lastMouseY = 0.0;
}

void InputHandler::handleKeyboard(GLFWwindow* window, Camera& camera, float objectRadius,
                                  my_glm::vec3& modelRotation, int& lightMode,
                                  int& isLightOff, int& appliedTexture, bool& textureToggled,
                                  bool& keyInteracted) {
    float adjustedRotation = ROTATION_SPEED * std::log1p(objectRadius + 1.0f) * objectRadius / 10.0f;
    
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.updateYaw(-adjustedRotation);
        keyInteracted = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.updateYaw(adjustedRotation);
        keyInteracted = true;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.updatePitch(adjustedRotation);
        keyInteracted = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.updatePitch(-adjustedRotation);
        keyInteracted = true;
    }
        
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        keyInteracted = true;
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        camera.reset(objectRadius);
        modelRotation = my_glm::vec3(0.0f);
        keyInteracted = true;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        modelRotation.x -= adjustedRotation;
        keyInteracted = true;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        modelRotation.x += adjustedRotation;
        keyInteracted = true;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        modelRotation.y -= adjustedRotation;
        keyInteracted = true;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        modelRotation.y += adjustedRotation;
        keyInteracted = true;
    }

    for (int key = GLFW_KEY_KP_0; key <= GLFW_KEY_KP_9; ++key) {
        if (glfwGetKey(window, key) == GLFW_PRESS) {
            lightMode = key - GLFW_KEY_KP_0;
            keyInteracted = true;
        }
    }

    int lState = glfwGetKey(window, GLFW_KEY_L);
    if (prevLState == GLFW_PRESS && lState == GLFW_RELEASE) {
        isLightOff = 1 - isLightOff;
        keyInteracted = true;
    }
    prevLState = lState;

    int tState = glfwGetKey(window, GLFW_KEY_T);
    if (prevTState == GLFW_PRESS && tState == GLFW_RELEASE) {
        appliedTexture = 1 - appliedTexture;
        keyInteracted = true;
    }
    prevTState = tState;

    int pState = glfwGetKey(window, GLFW_KEY_P);
    if (prevPState == GLFW_PRESS && pState == GLFW_RELEASE) {
        textureToggled = !textureToggled;
        keyInteracted = true;
    }
    prevPState = pState;
}

void InputHandler::handleMouseButton(int button, int action) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        leftMousePressed = (action == GLFW_PRESS);
    }
}

void InputHandler::handleMouseMove(double xpos, double ypos, Camera& camera) {
    if (!leftMousePressed) return;

    double dx = xpos - lastMouseX;
    double dy = ypos - lastMouseY;
    lastMouseX = xpos;
    lastMouseY = ypos;

    float sensitivity = 0.005f;

    my_glm::mat4 view = camera.computeViewMatrix();

    my_glm::vec3 right = { view[0][0], view[1][0], view[2][0] };
    my_glm::vec3 up    = { view[0][1], view[1][1], view[2][1] };

    my_glm::vec3 offset = right * static_cast<float>(dx) * sensitivity + 
                          up * static_cast<float>(-dy) * sensitivity;
    camera.updateOffset(offset);
}

void InputHandler::handleScroll(double yoffset, Camera& camera, bool& keyInteracted) {
    if (leftMousePressed) {
        my_glm::mat4 view = camera.computeViewMatrix();
        my_glm::vec3 forward = {
            -view[0][2], -view[1][2], -view[2][2]
        };

        my_glm::vec3 offset = forward * static_cast<float>(yoffset) * 0.5f;
        camera.updateOffset(offset);
    } else {
        camera.updateDistance(-static_cast<float>(yoffset + 0.1f) * 0.1f);
        keyInteracted = true;
    }
}
