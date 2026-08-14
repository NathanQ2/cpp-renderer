#include "ViewportMovementController.h"

#include <glm/gtc/constants.hpp>

namespace PalmTreeEditor {
    void ViewportMovementController::MoveInPlaneXZ(float dt, PalmTree::GameObject& gameObject) {
        if (!m_Enabled && PalmTree::Input::IsMouseButtonDown(m_Keys.RightButton) && m_ShouldCaptureMouse) {
            PalmTree::Input::SetCursorEnabled(false);

            m_Enabled = true;

            m_PreviousMousePosition = PalmTree::Input::GetMousePosition();
        }
        else if (m_Enabled && !PalmTree::Input::IsMouseButtonDown(m_Keys.RightButton) && m_ShouldCaptureMouse) {
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
            glm::vec3 newEuler = gameObject.GetTransform()->EulerAngles();
            if (glm::dot(mouseRotate, mouseRotate) > std::numeric_limits<float>::epsilon()) {
                newEuler += m_MouseLookSpeed * dt * mouseRotate;
            }
            
            newEuler.x = glm::clamp(newEuler.x, -glm::half_pi<float>(), glm::half_pi<float>());
            newEuler.y = glm::mod(newEuler.y, glm::two_pi<float>());

            gameObject.GetTransform()->SetEuler(newEuler);
            
            float yaw = gameObject.GetTransform()->EulerAngles().y;
            float pitch = gameObject.GetTransform()->EulerAngles().x;
            
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
