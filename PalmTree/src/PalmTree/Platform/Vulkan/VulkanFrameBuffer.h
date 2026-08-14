#pragma once

#include <vulkan/vulkan_core.h>

#include "VulkanSwapChain.h"
#include "PalmTree/Renderer/FrameBuffer.h"

namespace PalmTree {
    class VulkanFrameBuffer : public FrameBuffer {
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
        
        // VkRenderPass GetVkRenderPass() const { return !m_Spec.SwapChainTarget ? m_RenderPass : m_SwapChain->GetRenderPass(); }
        // VkFramebuffer GetVkFrameBuffer() const { return !m_Spec.SwapChainTarget ? m_FrameBuffer : m_SwapChain->GetFrameBuffer(); }
        VkRenderPass GetVkRenderPass() const { return m_RenderPass; }
        VkFramebuffer GetVkFrameBuffer() const { return m_FrameBuffer; }
        
        const FrameBufferSpecification& GetSpec() const override { return m_Spec; }
    private:
        void CreateRenderPass();
        void CreateFrameBuffer();
        void CreateImage();
        void CreateDepthImage();
        
        void CleanupRenderPass();
        void CleanupFrameBuffer();
        void CleanupImage();
        void CleanupDepthImage();
        
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
    };
}
