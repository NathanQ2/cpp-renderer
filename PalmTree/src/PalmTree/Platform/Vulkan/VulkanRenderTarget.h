#pragma once

#include <vulkan/vulkan.h>

namespace PalmTree {
    class VulkanRenderTarget {
    public:
        virtual ~VulkanRenderTarget() = default;

        virtual VkRenderPass GetRenderPass() = 0;
        virtual VkFramebuffer GetFrameBuffer() = 0;
    };
}
