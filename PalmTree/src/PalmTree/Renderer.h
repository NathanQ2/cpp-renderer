#pragma once

#include "Model.h"
#include "SwapChain.h"
#include "Window.h"


namespace PalmTree {
    class Renderer {
    public:
        Renderer(Window& window, Device& device);
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        [[nodiscard]] VkRenderPass GetSwapChainRenderPass() const { return m_SwapChain->GetRenderPass(); }
        [[nodiscard]] float GetAspectRatio() const { return m_SwapChain->ExtentAspectRatio(); }
        [[nodiscard]] bool IsFrameInProgress() const { return m_IsFrameStarted; }

        [[nodiscard]] VkCommandBuffer GetCurrentCommandBuffer() const {
            assert(m_IsFrameStarted && "Cannot get command buffer when frame not in progress");
            return m_CommandBuffers[m_CurrentFrameIndex];
        }

        VkCommandBuffer BeginFrame();
        void EndFrame();
        void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

        [[nodiscard]] int GetFrameIndex() const {
            assert(m_IsFrameStarted && "Cannot get command buffer when frame not in progress");

            return m_CurrentFrameIndex;
        }

    private:
        void CreateCommandBuffers();
        void FreeCommandBuffers();
        void RecreateSwapChain();

        Window& m_Window;
        Device& m_Device;
        std::unique_ptr<SwapChain> m_SwapChain = std::make_unique<SwapChain>(m_Window, m_Device);
        std::vector<VkCommandBuffer> m_CommandBuffers;

        uint32_t m_CurrentImageIndex;
        int m_CurrentFrameIndex;
        bool m_IsFrameStarted = false;
    };
}
