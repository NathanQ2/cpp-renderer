#pragma once

#include "VulkanCommandBuffer.h"
#include "../../Logging/Log.h"
#include "../../Model.h"
#include "VulkanSwapChain.h"
#include "../../Window.h"
#include "PalmTree/Renderer/RendererBackend.h"


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
        float GetAspectRatioImpl() const override { return m_SwapChain->ExtentAspectRatio(); }
        [[nodiscard]] bool IsFrameInProgress() const { return m_IsFrameStarted; }

        CommandBuffer& GetCurrentCommandBufferImpl() override {
            PT_CORE_ASSERT(m_IsFrameStarted, "Cannot get command buffer when frame not in progress");
            return *m_CommandBuffers[m_CurrentFrameIndex];
        }

        API GetAPIImpl() override { return API::VULKAN; }

        bool BeginFrameImpl() override;
        void EndFrameImpl() override;
        void BeginRenderPassImpl() override;
        void EndRenderPassImpl() override;

        [[nodiscard]] int GetFrameIndexImpl() const override {
            PT_CORE_ASSERT(m_IsFrameStarted, "Cannot get command buffer when frame not in progress");

            return m_CurrentFrameIndex;
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
        void FreeCommandBuffers();
        void RecreateSwapChain();

        Window& m_Window;
        std::unique_ptr<VulkanDevice> m_Device;
        std::unique_ptr<VulkanSwapChain> m_SwapChain;
        std::unique_ptr<VulkanDescriptorPool> m_DescriptorPool;
        std::vector<std::unique_ptr<VulkanCommandBuffer>> m_CommandBuffers;

        uint32_t m_CurrentImageIndex;
        int m_CurrentFrameIndex;
        bool m_IsFrameStarted = false;
    };
}
