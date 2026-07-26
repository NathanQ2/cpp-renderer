#include "ptpch.h"
#include "Components.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../Logging/Log.h"

namespace PalmTree {
    void TransformComponent::SetEuler(glm::vec3 euler) {
        Rotation = glm::eulerAngleYXZ(euler.y, euler.x, euler.z);
    }

    // Matrix corresponds to Translate * Ry * Rx * Rz * Scale
    // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
    // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
    glm::mat4 TransformComponent::TransformationMatrix() const {
        glm::mat4 scale = glm::scale(glm::identity<glm::mat4>(), Scale);
        glm::mat4 translation = glm::translate(glm::identity<glm::mat4>(), Translation);

        return translation * Rotation * scale;
    }

    glm::vec3 TransformComponent::EulerAngles() const {
        glm::vec3 eulerAngles;

        glm::extractEulerAngleYXZ(Rotation, eulerAngles.y, eulerAngles.x, eulerAngles.z);

        return eulerAngles;
    }

    glm::mat3 TransformComponent::NormalMatrix() const {
        const glm::vec3 invScale = 1.0f / Scale;

        return glm::mat3{
            {
                invScale.x * Rotation[0][0],
                invScale.x * Rotation[0][1],
                invScale.x * Rotation[0][2],
            },
            {
                invScale.y * Rotation[1][0],
                invScale.y * Rotation[1][1],
                invScale.y * Rotation[1][2],
            },
            {
                invScale.z * Rotation[2][0],
                invScale.z * Rotation[2][1],
                invScale.z * Rotation[2][2],
            },
        };
    }
}
