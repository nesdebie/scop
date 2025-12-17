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
