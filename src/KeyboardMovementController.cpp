#include "KeyboardMovementController.h"

#include <glm/gtc/constants.hpp>

namespace PalmTree {
    void KeyboardMovementController::moveInPlaneXZ(GLFWwindow* window, float dt, GameObject& gameObject) {
        glm::vec3 keyboardRotate = glm::vec3(0);

        if (glfwGetKey(window, m_keys.lookRight) == GLFW_PRESS) keyboardRotate.y += 1.0f;
        if (glfwGetKey(window, m_keys.lookLeft) == GLFW_PRESS) keyboardRotate.y -= 1.0f;

        if (glfwGetKey(window, m_keys.lookUp) == GLFW_PRESS) keyboardRotate.x += 1.0f;
        if (glfwGetKey(window, m_keys.lookDown) == GLFW_PRESS) keyboardRotate.x -= 1.0f;

        // Make sure rotate is nonzero 
        if (glm::dot(keyboardRotate, keyboardRotate) > std::numeric_limits<float>::epsilon()) {
            gameObject.getTransform().rotation += m_keyboardLookSpeed * dt * glm::normalize(keyboardRotate);
        }

        glm::vec3 mouseRotate = glm::vec3(0);

        glm::f64vec2 cursorPos = glm::f64vec2(0);
        glfwGetCursorPos(window, &cursorPos.x, &cursorPos.y);
        glm::f64vec2 cursorDelta = m_previousCursorPosition - cursorPos;
        m_previousCursorPosition = cursorPos;

        mouseRotate.x += static_cast<float>(cursorDelta.y);
        mouseRotate.y -= static_cast<float>(cursorDelta.x);

        // Make sure rotate is nonzero 
        if (glm::dot(mouseRotate, mouseRotate) > std::numeric_limits<float>::epsilon()) {
            gameObject.getTransform().rotation += m_mouseLookSpeed * dt * mouseRotate;
        }

        gameObject.getTransform().rotation.x = glm::clamp(gameObject.getTransform().rotation.x, -1.5f, 1.5f);
        gameObject.getTransform().rotation.y = glm::mod(gameObject.getTransform().rotation.y, glm::two_pi<float>());

        float yaw = gameObject.getTransform().rotation.y;
        const glm::vec3 forwardDir = glm::vec3(sin(yaw), 0.0f, cos(yaw));
        const glm::vec3 rightDir = glm::vec3(forwardDir.z, 0.0f, -forwardDir.x);
        const glm::vec3 upDir = glm::vec3(0.0f, -1.0f, 0.0f);

        glm::vec3 moveDir = glm::vec3(0);

        if (glfwGetKey(window, m_keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
        if (glfwGetKey(window, m_keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;

        if (glfwGetKey(window, m_keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
        if (glfwGetKey(window, m_keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;

        if (glfwGetKey(window, m_keys.moveUp) == GLFW_PRESS) moveDir += upDir;
        if (glfwGetKey(window, m_keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
            gameObject.getTransform().translation += m_moveSpeed * dt * glm::normalize(moveDir);
        }
    }
}
