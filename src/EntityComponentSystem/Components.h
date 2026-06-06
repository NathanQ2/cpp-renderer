#pragma once

#include "../PtModel.h"

#include <glm/glm.hpp>

namespace PalmTree {
    struct TransformComponent {
        glm::vec3 translation{};
        glm::vec3 scale{1.0f};
        glm::vec3 rotation;

        glm::mat4 mat4();
        glm::mat3 normalMatrix();
    };

    struct PointLightComponent {
        float lightIntensity = 1.0f;
        glm::vec3 color{};
    };

    struct ModelComponent {
        glm::vec3 color{};
        std::shared_ptr<PtModel> model{};
    };
}
