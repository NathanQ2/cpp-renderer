#include "KeyboardMovementController.h"

#include <glm/gtc/constants.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace Sandbox {
    void KeyboardMovementController::MoveInPlaneXZ(float dt, PalmTree::GameObject& gameObject) {
        if (PalmTree::Input::IsMouseButtonDown(m_Keys.LeftButton) && m_ShouldCaptureMouse()) {
            PalmTree::Input::SetCursorEnabled(false);

            m_LookEnabled = true;

            m_PreviousMousePosition = PalmTree::Input::GetMousePosition();
        }

        if (PalmTree::Input::IsKeyDown(m_Keys.Escape)) {
            PalmTree::Input::SetCursorEnabled(true);

            m_LookEnabled = false;
        }

        if (m_LookEnabled) {
            glm::vec3 mouseRotate = glm::vec3(0);

            glm::vec2 cursorPos = PalmTree::Input::GetMousePosition();
            glm::vec2 cursorDelta = m_PreviousMousePosition - cursorPos;
            m_PreviousMousePosition = cursorPos;

            mouseRotate.x += cursorDelta.y;
            mouseRotate.y -= cursorDelta.x;

            // Make sure rotate is nonzero 
            if (glm::dot(mouseRotate, mouseRotate) > std::numeric_limits<float>::epsilon()) {
                glm::vec3 newEuler = gameObject.GetTransform().EulerAngles() + m_MouseLookSpeed * dt * mouseRotate;
                
                gameObject.GetTransform().SetEuler(newEuler);
            }
        }

        glm::vec3 keyboardRotate = glm::vec3(0);

        if (PalmTree::Input::IsKeyDown(m_Keys.LookRight)) keyboardRotate.y += 1.0f;
        if (PalmTree::Input::IsKeyDown(m_Keys.LookLeft)) keyboardRotate.y -= 1.0f;

        if (PalmTree::Input::IsKeyDown(m_Keys.LookUp)) keyboardRotate.x += 1.0f;
        if (PalmTree::Input::IsKeyDown(m_Keys.LookDown)) keyboardRotate.x -= 1.0f;

        // Make sure rotate is nonzero 
        if (glm::dot(keyboardRotate, keyboardRotate) > std::numeric_limits<float>::epsilon()) {
            glm::vec3 newEuler = gameObject.GetTransform().EulerAngles() += m_KeyboardLookSpeed * dt * glm::normalize(keyboardRotate);
            
            gameObject.GetTransform().SetEuler(newEuler);
        }

        gameObject.GetTransform().SetEuler({
            glm::clamp(gameObject.GetTransform().EulerAngles().x, -1.5f, 1.5f),
            glm::mod(gameObject.GetTransform().EulerAngles().y, glm::two_pi<float>()),
            gameObject.GetTransform().EulerAngles().z
        });

        float yaw = gameObject.GetTransform().EulerAngles().y;
        const glm::vec3 forwardDir = glm::vec3(sin(yaw), 0.0f, cos(yaw));
        const glm::vec3 rightDir = glm::vec3(forwardDir.z, 0.0f, -forwardDir.x);
        const glm::vec3 upDir = glm::vec3(0.0f, -1.0f, 0.0f);

        glm::vec3 moveDir = glm::vec3(0);

        if (PalmTree::Input::IsKeyDown(m_Keys.MoveForward)) moveDir += forwardDir;
        if (PalmTree::Input::IsKeyDown(m_Keys.MoveBackward)) moveDir -= forwardDir;

        if (PalmTree::Input::IsKeyDown(m_Keys.MoveRight)) moveDir += rightDir;
        if (PalmTree::Input::IsKeyDown(m_Keys.MoveLeft)) moveDir -= rightDir;

        if (PalmTree::Input::IsKeyDown(m_Keys.MoveUp)) moveDir += upDir;
        if (PalmTree::Input::IsKeyDown(m_Keys.MoveDown)) moveDir -= upDir;

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
            gameObject.GetTransform().Translation += m_MoveSpeed * dt * glm::normalize(moveDir);
        }
    }
}
