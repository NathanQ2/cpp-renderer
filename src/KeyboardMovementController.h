#pragma once

#include "PtGameObject.h"
#include "PtWindow.h"

namespace PalmTree {
    class KeyboardMovementController {
    public:
        struct KeyMappings {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_E;
            int moveDown = GLFW_KEY_Q;
            int lookLeft = GLFW_KEY_LEFT;
            int lookRight = GLFW_KEY_RIGHT;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;
        };
        
        KeyboardMovementController(glm::f64vec2 initialCursorPos) : m_PreviousCursorPosition(initialCursorPos) {}
        
        void moveInPlaneXZ(GLFWwindow* window, float dt, PtGameObject& gameObject);
        
    private:
        KeyMappings m_Keys{};
        float m_MoveSpeed = 3.0f;
        float m_KeyboardLookSpeed = 1.5f;
        float m_MouseLookSpeed = 0.015f;
        
        glm::f64vec2 m_PreviousCursorPosition;
    };
}
