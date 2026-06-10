#pragma once

#include "../Model.h"

#include <glm/glm.hpp>

namespace PalmTree {
    struct TransformComponent {
        glm::vec3 Translation{};
        glm::vec3 Scale{1.0f};
        glm::vec3 Rotation;

        glm::mat4 Mat4();
        glm::mat3 NormalMatrix();
    };

    struct PointLightComponent {
        float LightIntensity = 1.0f;
        glm::vec3 Color{};
    };

    struct ModelComponent {
        glm::vec3 Color{};
        std::shared_ptr<Model> Model{};
    };
}
