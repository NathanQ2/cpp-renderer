#include "KeyboardMovementController.h"

namespace PalmTree {
    void KeyboardMovementController::moveInPlaneXZ(GLFWwindow* window, float dt, PtGameObject& gameObject) {
        glm::vec3 keybaordRotate = glm::vec3(0);
        
        if (glfwGetKey(window, m_Keys.lookRight) == GLFW_PRESS) keybaordRotate.y += 1.0f;
        if (glfwGetKey(window, m_Keys.lookLeft) == GLFW_PRESS) keybaordRotate.y -= 1.0f;
        
        if (glfwGetKey(window, m_Keys.lookUp) == GLFW_PRESS) keybaordRotate.x += 1.0f;
        if (glfwGetKey(window, m_Keys.lookDown) == GLFW_PRESS) keybaordRotate.x -= 1.0f;
        
        // Make sure rotate is nonzero 
        if (glm::dot(keybaordRotate, keybaordRotate) > std::numeric_limits<float>::epsilon()) {
            gameObject.transform.rotation += m_KeyboardLookSpeed * dt * glm::normalize(keybaordRotate);
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
            gameObject.transform.rotation += m_MouseLookSpeed * dt * mouseRotate;
        }
        
        gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
        gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());
        
        float yaw = gameObject.transform.rotation.y;
        const glm::vec3 forwardDir = glm::vec3(sin(yaw), 0.0f, cos(yaw));
        const glm::vec3 rightDir = glm::vec3(forwardDir.z, 0.0f, -forwardDir.x);
        const glm::vec3 upDir = glm::vec3(0.0f, -1.0f, 0.0f);
        
        glm::vec3 moveDir = glm::vec3(0);
        
        if (glfwGetKey(window, m_Keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
        if (glfwGetKey(window, m_Keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
        
        if (glfwGetKey(window, m_Keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
        if (glfwGetKey(window, m_Keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
        
        if (glfwGetKey(window, m_Keys.moveUp) == GLFW_PRESS) moveDir += upDir;
        if (glfwGetKey(window, m_Keys.moveDown) == GLFW_PRESS) moveDir -= upDir;
        
        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
            gameObject.transform.translation += m_MoveSpeed * dt * glm::normalize(moveDir);
        }
    }
}
