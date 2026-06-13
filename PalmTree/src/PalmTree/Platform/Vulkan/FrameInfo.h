#pragma once

#include "../../Camera.h"
#include "../../EntityComponentSystem/GameObject.h"

#include <vulkan/vulkan.h>

#define MAX_LIGHTS 10

namespace PalmTree {
    struct PointLight {
        glm::vec4 Position{};
        glm::vec4 Color{};
    };

    struct GlobalUBO {
        glm::mat4 Projection = glm::mat4(1.0f);
        glm::mat4 View = glm::mat4(1.0f);
        glm::mat4 InverseView = glm::mat4(1.0f);
        glm::vec4 AmbientLightColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.02f);
        PointLight PointLights[MAX_LIGHTS];
        int NumLights;
    };

    struct FrameInfo {
        int FrameIndex;
        float FrameTime;
        VkCommandBuffer CommandBuffer;
        Camera& Camera;
        VkDescriptorSet GlobalDescriptorSet;
    };
}
