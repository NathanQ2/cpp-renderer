#pragma once

#include "PtModel.h"
#include "PtSwapChain.h"
#include "PtWindow.h"

#include <cassert>
#include <memory>
#include <vector>

namespace PalmTree {
    class PtRenderer {
    public:
        PtRenderer(PtWindow& window, PtDevice& device);
        ~PtRenderer();

        PtRenderer(const PtRenderer&) = delete;
        PtRenderer& operator=(const PtRenderer&) = delete;

        [[nodiscard]] VkRenderPass getSwapChainRenderPass() const { return m_swapChain->getRenderPass(); }
        [[nodiscard]] float getAspectRatio() const { return m_swapChain->extentAspectRatio(); }
        [[nodiscard]] bool isFrameInProgress() const { return m_isFrameStarted; }

        [[nodiscard]] VkCommandBuffer getCurrentCommandBuffer() const {
            assert(m_isFrameStarted && "Cannot get command buffer when frame not in progress");
            return m_commandBuffers[m_currentFrameIndex];
        }

        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

        [[nodiscard]] int getFrameIndex() const {
            assert(m_isFrameStarted && "Cannot get command buffer when frame not in progress");

            return m_currentFrameIndex;
        }

    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();

        PtWindow& m_window;
        PtDevice& m_device;
        std::unique_ptr<PtSwapChain> m_swapChain = std::make_unique<PtSwapChain>(m_window, m_device);
        std::vector<VkCommandBuffer> m_commandBuffers;

        uint32_t m_currentImageIndex;
        int m_currentFrameIndex;
        bool m_isFrameStarted = false;
    };
}
