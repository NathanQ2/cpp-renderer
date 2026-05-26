#include "KeyboardMovementController.h"

namespace PalmTree {
    void KeyboardMovementController::moveInPlaneXZ(GLFWwindow* window, float dt, PtGameObject& gameObject) {
        glm::vec3 rotate = glm::vec3(0);
        
        if (glfwGetKey(window, m_Keys.lookRight) == GLFW_PRESS) rotate.y += 1.0f;
        if (glfwGetKey(window, m_Keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.0f;
        
        if (glfwGetKey(window, m_Keys.lookUp) == GLFW_PRESS) rotate.x += 1.0f;
        if (glfwGetKey(window, m_Keys.lookDown) == GLFW_PRESS) rotate.x -= 1.0f;
        
        // Make sure rotate is nonzero 
        if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
            gameObject.transform.rotation += m_LookSpeed * dt * glm::normalize(rotate);
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
