#include "KeyboardMovementController.h"

#include <glm/gtc/constants.hpp>

namespace PalmTree {
    void KeyboardMovementController::MoveInPlaneXZ(GLFWwindow* window, float dt, GameObject& gameObject) {
        glm::vec3 keyboardRotate = glm::vec3(0);

        if (glfwGetKey(window, m_Keys.LookRight) == GLFW_PRESS) keyboardRotate.y += 1.0f;
        if (glfwGetKey(window, m_Keys.LookLeft) == GLFW_PRESS) keyboardRotate.y -= 1.0f;

        if (glfwGetKey(window, m_Keys.LookUp) == GLFW_PRESS) keyboardRotate.x += 1.0f;
        if (glfwGetKey(window, m_Keys.LookDown) == GLFW_PRESS) keyboardRotate.x -= 1.0f;

        // Make sure rotate is nonzero 
        if (glm::dot(keyboardRotate, keyboardRotate) > std::numeric_limits<float>::epsilon()) {
            gameObject.GetTransform().Rotation += m_KeyboardLookSpeed * dt * glm::normalize(keyboardRotate);
        }

        glm::vec3 mouseRotate = glm::vec3(0);

        glm::f64vec2 cursorPos = glm::f64vec2(0);
        glfwGetCursorPos(window, &cursorPos.x, &cursorPos.y);
        glm::f64vec2 cursorDelta = m_PreviousCursorPosition - cursorPos;
        m_PreviousCursorPosition = cursorPos;

        mouseRotate.x += static_cast<float>(cursorDelta.y);
        mouseRotate.y -= static_cast<float>(cursorDelta.x);

        // Make sure rotate is nonzero 
        if (glm::dot(mouseRotate, mouseRotate) > std::numeric_limits<float>::epsilon()) {
            gameObject.GetTransform().Rotation += m_MouseLookSpeed * dt * mouseRotate;
        }

        gameObject.GetTransform().Rotation.x = glm::clamp(gameObject.GetTransform().Rotation.x, -1.5f, 1.5f);
        gameObject.GetTransform().Rotation.y = glm::mod(gameObject.GetTransform().Rotation.y, glm::two_pi<float>());

        float yaw = gameObject.GetTransform().Rotation.y;
        const glm::vec3 forwardDir = glm::vec3(sin(yaw), 0.0f, cos(yaw));
        const glm::vec3 rightDir = glm::vec3(forwardDir.z, 0.0f, -forwardDir.x);
        const glm::vec3 upDir = glm::vec3(0.0f, -1.0f, 0.0f);

        glm::vec3 moveDir = glm::vec3(0);

        if (glfwGetKey(window, m_Keys.MoveForward) == GLFW_PRESS) moveDir += forwardDir;
        if (glfwGetKey(window, m_Keys.MoveBackward) == GLFW_PRESS) moveDir -= forwardDir;

        if (glfwGetKey(window, m_Keys.MoveRight) == GLFW_PRESS) moveDir += rightDir;
        if (glfwGetKey(window, m_Keys.MoveLeft) == GLFW_PRESS) moveDir -= rightDir;

        if (glfwGetKey(window, m_Keys.MoveUp) == GLFW_PRESS) moveDir += upDir;
        if (glfwGetKey(window, m_Keys.MoveDown) == GLFW_PRESS) moveDir -= upDir;

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
            gameObject.GetTransform().Translation += m_MoveSpeed * dt * glm::normalize(moveDir);
        }
    }
}
