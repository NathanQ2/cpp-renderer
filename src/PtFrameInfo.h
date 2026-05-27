#pragma once

#include "PtCamera.h"

#include <vulkan/vulkan.h>

namespace PalmTree {
    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        PtCamera& camera;
    };
}
