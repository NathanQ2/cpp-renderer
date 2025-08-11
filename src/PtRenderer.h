#pragma once

#include "PtWindow.h"
#include "PtSwapChain.h"
#include "PtModel.h"

#include <memory>
#include <vector>
#include <cassert>

namespace PalmTree {
    class PtRenderer {
    public:
        PtRenderer(PtWindow& window, PtDevice& device);
        ~PtRenderer();

        PtRenderer(const PtRenderer&) = delete;
        PtRenderer& operator=(const PtRenderer&) = delete;

        [[nodiscard]] VkRenderPass GetSwapChainRenderPass() const { return m_SwapChain->getRenderPass(); }
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
    
        PtWindow& m_Window;
        PtDevice& m_Device;
        std::unique_ptr<PtSwapChain> m_SwapChain = std::make_unique<PtSwapChain>(m_Window, m_Device);
        std::vector<VkCommandBuffer> m_CommandBuffers;

        uint32_t m_CurrentImageIndex;
        int m_CurrentFrameIndex;
        bool m_IsFrameStarted = false;
    };
}
