#pragma once

#include "EntityComponentSystem/EntityComponentSystem.h"
#include "Window.h"
#include "EventSystem/KeyEvents.h"

namespace PalmTree {
    class KeyboardMovementController {
    public:
        struct KeyMappings {
            int MoveLeft = GLFW_KEY_A;
            int MoveRight = GLFW_KEY_D;
            int MoveForward = GLFW_KEY_W;
            int MoveBackward = GLFW_KEY_S;
            int MoveUp = GLFW_KEY_E;
            int MoveDown = GLFW_KEY_Q;
            int LookLeft = GLFW_KEY_LEFT;
            int LookRight = GLFW_KEY_RIGHT;
            int LookUp = GLFW_KEY_UP;
            int LookDown = GLFW_KEY_DOWN;
            int Escape = GLFW_KEY_ESCAPE;
            int LeftButton = GLFW_MOUSE_BUTTON_LEFT;
        };

        explicit KeyboardMovementController(const glm::f64vec2 initialCursorPos) : m_PreviousCursorPosition(
            initialCursorPos
        ) {}

        void MoveInPlaneXZ(GLFWwindow* window, float dt, GameObject& gameObject);

    private:
        KeyMappings m_Keys{};
        float m_MoveSpeed = 3.0f;
        float m_KeyboardLookSpeed = 1.5f;
        float m_MouseLookSpeed = 0.015f;

        glm::f64vec2 m_PreviousCursorPosition;
        
        bool m_Enabled = false;
    };
}
