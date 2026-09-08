#pragma once

#include <vulkan/vulkan_core.h>

#include "VulkanRenderTarget.h"
#include "VulkanSwapChain.h"
#include "PalmTree/Renderer/FrameBuffer.h"

namespace PalmTree {
    class VulkanFrameBuffer : public FrameBuffer, public VulkanRenderTarget {
    public:
        VulkanFrameBuffer(const FrameBufferSpecification& spec);

        ~VulkanFrameBuffer() override;

        VulkanFrameBuffer(VulkanFrameBuffer& other) = delete;
        void operator=(VulkanFrameBuffer& other) = delete;

        VulkanFrameBuffer(VulkanFrameBuffer&& other) = delete;
        void operator=(VulkanFrameBuffer&& other) = delete;

        void Invalidate() override;
        void Resize(uint32_t width, uint32_t height) override;

        VkFormat FindDepthFormat();
        VkFormat FindColorFormat();

        VkRenderPass GetRenderPass() override { return m_RenderPass; }
        VkFramebuffer GetFrameBuffer() override { return m_FrameBuffer; }

        uint32_t GetWidth() const override { return m_Spec.Width; }
        uint32_t GetHeight() const override { return m_Spec.Height; }

        const FrameBufferSpecification& GetSpec() const override { return m_Spec; }

        ImTextureID CreateImTextureID() override;
    private:
        void CreateRenderPass();
        void CreateFrameBuffer();
        void CreateImage();
        void CreateDepthImage();
        void CreateSampler();

        void CleanupRenderPass();
        void CleanupFrameBuffer();
        void CleanupImage();
        void CleanupDepthImage();
        void CleanupSampler();

        FrameBufferSpecification m_Spec;

        VulkanDevice& m_Device;

        VkFramebuffer m_FrameBuffer;
        VkRenderPass m_RenderPass;
        VkImage m_Image;
        VkDeviceMemory m_ImageMemory;
        VkImageView m_ImageView;
        VkFormat m_ImageFormat;

        VkImage m_DepthImage;
        VkDeviceMemory m_DepthImageMemory;
        VkImageView m_DepthImageView;
        VkFormat m_DepthImageFormat;

        VkSampler m_Sampler;
    };
}
