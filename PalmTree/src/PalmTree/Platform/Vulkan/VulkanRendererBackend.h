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

        [[nodiscard]] bool IsFrameInProgress() const { return m_IsFrameStarted; }

        CommandBuffer& GetCurrentCommandBufferImpl() override {
            PT_CORE_ASSERT(m_IsFrameStarted, "Cannot get command buffer when frame is not in progress");

            return *m_CommandBuffers[m_InFlightFrameIndex];
        }

        API GetAPIImpl() override { return API::VULKAN; }

        bool BeginFrameImpl() override;
        void EndFrameImpl() override;

        void BeginRenderPassImpl(RenderTarget& target) override;
        void EndRenderPassImpl() override;

        void BeginSwapChainRenderPassImpl() override;
        void EndSwapChainRenderPassImpl() override;

        SwapChain& GetSwapChainImpl() override { return m_SwapChain; }

        int GetInFlightFrameIndexImpl() override {
            PT_CORE_ASSERT(m_IsFrameStarted, "Cannot get command buffer when frame not in progress");

            return m_InFlightFrameIndex;
        }

        VulkanDevice& GetDevice() { return *m_Device; }

        VkCommandBuffer GetCurrentVkCommandBuffer() {
            return m_CommandBuffers[m_InFlightFrameIndex]
                       ? m_CommandBuffers[m_InFlightFrameIndex]->GetVkCommandBuffer()
                       : VK_NULL_HANDLE;
        }

        VulkanDescriptorPool& GetDescriptorPool() const { return *m_DescriptorPool; }
    private:
        static VulkanRendererBackend* s_VulkanInstance;

        void CreateCommandBuffers();
        void FreeCommandBuffers();

        void RecreateSwapChain();

        Window& m_Window;
        std::unique_ptr<VulkanDevice> m_Device;
        std::unique_ptr<VulkanDescriptorPool> m_DescriptorPool;

        uint32_t m_InFlightFrameIndex = 0;

        VulkanSwapChain m_SwapChain;
        std::vector<std::unique_ptr<VulkanCommandBuffer>> m_CommandBuffers;

        bool m_IsFrameStarted = false;
    };
}
