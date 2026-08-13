#pragma once

#include "PalmTree/Logging/Log.h"
#include "PalmTree/Window.h"
#include "PalmTree/Renderer/RendererBackend.h"

#include "VulkanCommandBuffer.h"
#include "VulkanSwapChain.h"


namespace PalmTree {
    class VulkanRendererBackend : public RendererBackend {
        friend class RendererBackend;
    public:
        static VulkanRendererBackend* Get() {
            PT_CORE_ASSERT(
                RendererBackend::GetAPI() == RendererBackend::API::VULKAN,
                "Vulkan is not the current RendererBackend."
            );
            return s_VulkanInstance;
        }

        VulkanRendererBackend(Window& window);
        ~VulkanRendererBackend() override;

        VulkanRendererBackend(const VulkanRendererBackend&) = delete;
        VulkanRendererBackend& operator=(const VulkanRendererBackend&) = delete;

        [[nodiscard]] VkRenderPass GetSwapChainRenderPass() const { return m_SwapChain->GetRenderPass(); }
        float GetSwapChainAspectRatioImpl() const override { return m_SwapChain->ExtentAspectRatio(); }
        [[nodiscard]] bool IsFrameInProgress() const { return m_IsFrameStarted; }

        CommandBuffer& GetCurrentCommandBufferImpl() override {
            PT_CORE_ASSERT(m_IsFrameStarted, "Cannot get command buffer when frame not in progress");
            return *m_CommandBuffer;
        }

        API GetAPIImpl() override { return API::VULKAN; }

        bool BeginFrameImpl() override;
        void EndFrameImpl() override;
        
        void BeginSwapChainRenderPassImpl() override;
        void EndSwapChainRenderPassImpl() override;
        
        void BeginRenderPassImpl(std::shared_ptr<FrameBuffer> frameBuffer) override;
        void EndRenderPassImpl() override;

        [[nodiscard]] int GetSwapChainFrameIndexImpl() const override {
            PT_CORE_ASSERT(m_IsFrameStarted, "Cannot get command buffer when frame not in progress");

            return m_SwapChainCurrentFrameIndex;
        }

        uint32_t GetImageCount() { return m_SwapChain->GetImageCount(); }

        VulkanDevice& GetDevice() { return *m_Device; }

        VkCommandBuffer GetCurrentVkCommandBuffer() {
            return dynamic_cast<VulkanCommandBuffer&>(GetCurrentCommandBufferImpl()).GetVkCommandBuffer();
        }

        VulkanDescriptorPool& GetDescriptorPool() const { return *m_DescriptorPool; }
    private:
        static VulkanRendererBackend* s_VulkanInstance;

        void CreateCommandBuffers();
        void FreeCommandBuffer();
        void FreeFences();
        
        void FreeSwapChainCommandBuffers();
        void RecreateSwapChain();

        Window& m_Window;
        std::unique_ptr<VulkanDevice> m_Device;
        std::unique_ptr<VulkanDescriptorPool> m_DescriptorPool;
        
        std::shared_ptr<VulkanFrameBuffer> m_CurrentFrameBuffer;
        std::unique_ptr<VulkanCommandBuffer> m_CommandBuffer;
        VkFence m_InFlightFence = VK_NULL_HANDLE;

        // SwapChain
        std::unique_ptr<VulkanSwapChain> m_SwapChain;
        std::vector<std::unique_ptr<VulkanCommandBuffer>> m_SwapChainCommandBuffers;
        uint32_t m_SwapChainCurrentImageIndex;
        int m_SwapChainCurrentFrameIndex;
        
        bool m_IsFrameStarted = false;
    };
}
