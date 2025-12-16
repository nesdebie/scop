/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputHandler.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nesdebie <nesdebie@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 00:00:00 by nesdebie          #+#    #+#             */
/*   Updated: 2025/12/16 00:00:00 by nesdebie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <GLFW/glfw3.h>
#include "my_glm.h"

class Camera;

class InputHandler {
    public:
        InputHandler();
        ~InputHandler();

        void init();
        void handleKeyboard(GLFWwindow* window, Camera& camera, float objectRadius, 
                           my_glm::vec3& modelRotation, int& lightMode,
                           int& isLightOff, int& appliedTexture, bool& textureToggled,
                           bool& keyInteracted);

        void handleMouseButton(int button, int action);
        void handleMouseMove(double xpos, double ypos, Camera& camera);
        void handleScroll(double yoffset, Camera& camera, bool& keyInteracted);

        bool isLeftMousePressed() const { return leftMousePressed; }

    private:
        bool leftMousePressed;
        int prevLState;
        int prevTState;
        int prevPState;
        double lastMouseX;
        double lastMouseY;
};

#endif
