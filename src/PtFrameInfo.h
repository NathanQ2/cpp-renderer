#pragma once

#include "PtCamera.h"
#include "PtGameObject.h"

#include <vulkan/vulkan.h>

namespace PalmTree {
    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        PtCamera& camera;
        VkDescriptorSet globalDescriptorSet;
        PtGameObject::Map& gameObjects;
    };
}
