/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 00:00:00 by nesdebie          #+#    #+#             */
/*   Updated: 2025/12/16 00:00:00 by nesdebie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Camera.h"
#include <cmath>

Camera::Camera() {
    this->cameraYaw = 0.0f;
    this->cameraPitch = 0.0f;
    this->cameraDistance = 1.0f;
    this->modelOffset = my_glm::vec3(0.0f);
}

Camera::~Camera() {}

void Camera::init(float objectRadius) {
    this->cameraYaw = 0.0f;
    this->cameraPitch = 0.0f;
    this->cameraDistance = objectRadius * 2.2f;
    this->modelOffset = my_glm::vec3(0.0f);
}

void Camera::reset(float objectRadius) {
    this->cameraYaw = 0.0f;
    this->cameraPitch = 0.0f;
    this->cameraDistance = objectRadius * 2.2f;
    this->modelOffset = my_glm::vec3(0.0f);
}

my_glm::mat4 Camera::computeViewMatrix() const {
    my_glm::vec3 cameraOffset = {
        cameraDistance * std::cos(cameraPitch) * std::sin(cameraYaw),
        cameraDistance * std::sin(cameraPitch),
        cameraDistance * std::cos(cameraPitch) * std::cos(cameraYaw)
    };
    my_glm::vec3 cameraPos = modelOffset + cameraOffset;
    return my_glm::lookAt(cameraPos, modelOffset, {0.0f, 1.0f, 0.0f});
}

my_glm::vec3 Camera::getCameraPosition() const {
    my_glm::vec3 cameraPos = {
        cameraDistance * std::cos(cameraPitch) * std::sin(cameraYaw),
        cameraDistance * std::sin(cameraPitch),
        cameraDistance * std::cos(cameraPitch) * std::cos(cameraYaw)
    };
    return cameraPos;
}

void Camera::updateYaw(float delta) {
    cameraYaw += delta;
}

void Camera::updatePitch(float delta) {
    cameraPitch += delta;
    cameraPitch = my_glm::clamp(cameraPitch, -my_glm::half_pi() + 0.01f, my_glm::half_pi() - 0.01f);
}

void Camera::updateDistance(float delta) {
    cameraDistance += delta;
    cameraDistance = my_glm::clamp(cameraDistance, 0.5f, 20.0f);
}

void Camera::updateOffset(const my_glm::vec3& delta) {
    modelOffset += delta;
}
