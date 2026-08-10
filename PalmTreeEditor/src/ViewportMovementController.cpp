#include "ViewportMovementController.h"

#include <glm/gtc/constants.hpp>

namespace PalmTreeEditor {
    void ViewportMovementController::MoveInPlaneXZ(float dt, PalmTree::GameObject& gameObject) {
        bool shouldCaptureMouse = m_ShouldCaptureMouse();
        if (!m_Enabled && PalmTree::Input::IsMouseButtonDown(m_Keys.RightButton) && shouldCaptureMouse) {
            PalmTree::Input::SetCursorEnabled(false);

            m_Enabled = true;

            m_PreviousMousePosition = PalmTree::Input::GetMousePosition();
        }
        else if (m_Enabled && !PalmTree::Input::IsMouseButtonDown(m_Keys.RightButton) && shouldCaptureMouse) {
            m_Enabled = false;
            PalmTree::Input::SetCursorEnabled(true);
        }

        if (m_Enabled) {
            glm::vec3 mouseRotate = glm::vec3(0);

            glm::vec2 cursorPos = PalmTree::Input::GetMousePosition();
            glm::vec2 cursorDelta = m_PreviousMousePosition - cursorPos;
            m_PreviousMousePosition = cursorPos;

            mouseRotate.x += cursorDelta.y;
            mouseRotate.y -= cursorDelta.x;

            // Make sure rotate is nonzero 
            if (glm::dot(mouseRotate, mouseRotate) > std::numeric_limits<float>::epsilon()) {
                gameObject.GetTransform()->Rotation += m_MouseLookSpeed * dt * mouseRotate;
            }
            
            gameObject.GetTransform()->Rotation.x = glm::clamp(gameObject.GetTransform()->Rotation.x, -1.5f, 1.5f);
            gameObject.GetTransform()->Rotation.y = glm::mod(gameObject.GetTransform()->Rotation.y, glm::two_pi<float>());

            float yaw = gameObject.GetTransform()->Rotation.y;
            float pitch = gameObject.GetTransform()->Rotation.x;
            
            const glm::vec3 forwardDir = glm::vec3(sin(yaw) * cos(pitch), -sin(pitch), cos(yaw) * cos(pitch));
            const glm::vec3 rightDir = glm::vec3(forwardDir.z, 0.0f, -forwardDir.x);
            const glm::vec3 upDir = glm::vec3(0.0f, -1.0f, 0.0f);

            glm::vec3 moveDir = glm::vec3(0);

            if (PalmTree::Input::IsKeyDown(m_Keys.MoveForward)) moveDir += forwardDir;
            if (PalmTree::Input::IsKeyDown(m_Keys.MoveBackward)) moveDir -= forwardDir;

            if (PalmTree::Input::IsKeyDown(m_Keys.MoveRight)) moveDir += rightDir;
            if (PalmTree::Input::IsKeyDown(m_Keys.MoveLeft)) moveDir -= rightDir;

            if (PalmTree::Input::IsKeyDown(m_Keys.MoveUp)) moveDir += upDir;
            if (PalmTree::Input::IsKeyDown(m_Keys.MoveDown)) moveDir -= upDir;
            
            if (PalmTree::Input::IsKeyDown(m_Keys.SpeedMode)) m_MoveSpeed = m_DefaultMoveSpeed * m_SpeedMultiplier;
            else m_MoveSpeed = m_DefaultMoveSpeed;

            if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
                gameObject.GetTransform()->Translation += m_MoveSpeed * dt * glm::normalize(moveDir);
            }
        }
    }
}
