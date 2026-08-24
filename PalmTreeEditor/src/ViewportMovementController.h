#pragma once

#include <PalmTree.h>

namespace PalmTreeEditor {
    class ViewportMovementController {
    public:
        struct KeyMappings {
            int MoveLeft = PT_KEY_A;
            int MoveRight = PT_KEY_D;
            int MoveForward = PT_KEY_W;
            int MoveBackward = PT_KEY_S;
            int MoveUp = PT_KEY_E;
            int MoveDown = PT_KEY_Q;
            int LookLeft = PT_KEY_LEFT;
            int LookRight = PT_KEY_RIGHT;
            int LookUp = PT_KEY_UP;
            int LookDown = PT_KEY_DOWN;
            int Escape = PT_KEY_ESCAPE;
            int LeftButton = PT_MOUSE_BUTTON_LEFT;
            int RightButton = PT_MOUSE_BUTTON_RIGHT;
            int SpeedMode = PT_KEY_LEFT_SHIFT;
        };

        explicit ViewportMovementController() {
            m_PreviousMousePosition = PalmTree::Input::GetMousePosition();
        }

        void MoveInPlaneXZ(float dt, PalmTree::GameObject& gameObject);
        
        void SetShouldCaptureMouse(bool capture) { m_ShouldCaptureMouse = capture; }
    private:
        KeyMappings m_Keys{};
        float m_DefaultMoveSpeed = 3.0f;
        float m_MoveSpeed = m_DefaultMoveSpeed;
        float m_SpeedMultiplier = 2.0f;
        float m_MouseLookSpeed = 0.04f;

        glm::vec2 m_PreviousMousePosition;

        bool m_ShouldCaptureMouse;

        bool m_Enabled = false;
    };
}
