#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <GLFW/glfw3.h>
#include "my_glm.h"

#define BASE_ROTATION_SPEED 0.005f

class Camera;

/**
 * Processes user input from keyboard and mouse.
 * 
 * Handles camera controls, model rotation, lighting modes, and texture
 * toggling through keyboard and mouse interactions.
 */
class InputHandler {
    public:
        InputHandler();
        ~InputHandler();

        void init();
        void handleKeyboard(GLFWwindow* window, Camera& camera, float objectRadius, float cameraDistance,
                           my_glm::vec3& modelRotation, int& lightMode,
                           int& isLightOff, int& appliedTexture, bool& textureToggled,
                           bool& keyInteracted);

        void handleMouseButton(int button, int action, double xpos, double ypos);
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
