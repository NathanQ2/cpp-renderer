#pragma once

#include "PtDevice.h"
#include "PtWindow.h"

#include <vector>

namespace PalmTree {
    class PtSwapChain {
    public:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

        PtSwapChain(PtWindow& window, PtDevice& device) : m_window{window}, m_device{device} { init(); }

        ~PtSwapChain() {
            cleanupSwapChain();
            cleanupSyncObjects();
        }

        PtSwapChain(const PtSwapChain&) = delete;
        PtSwapChain& operator=(const PtSwapChain&) = delete;

        VkFramebuffer getFrameBuffer(int index) { return m_swapChainFramebuffers[index]; }
        VkRenderPass getRenderPass() { return m_renderPass; }
        VkImageView getImageView(int index) { return m_swapChainImageViews[index]; }
        void recreateSwapChain();
        void cleanupSwapChain();
        size_t imageCount() { return m_swapChainImages.size(); }
        VkFormat getSwapChainImageFormat() { return m_swapChainImageFormat; }
        VkExtent2D getSwapChainExtent() { return m_swapChainExtent; }
        uint32_t width() { return m_swapChainExtent.width; }
        uint32_t height() { return m_swapChainExtent.height; }

        float extentAspectRatio() {
            return static_cast<float>(m_swapChainExtent.width) / static_cast<float>(m_swapChainExtent.height);
        }

        VkFormat findDepthFormat();

        VkResult acquireNextImage(uint32_t* imageIndex);
        VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

        bool compareSwapFormats(VkFormat otherDepthFormat, VkFormat otherImageFormat) const {
            return otherDepthFormat == m_swapChainDepthFormat && otherImageFormat == m_swapChainImageFormat;
        }

    private:
        void init();
        void cleanupSyncObjects();
        void createSwapChain();
        void createImageViews();
        void createDepthResources();
        void createRenderPass();
        void createFramebuffers();
        void createSyncObjects();

        // Helper functions
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(
            const std::vector<VkSurfaceFormatKHR>& availableFormats
        );
        VkPresentModeKHR chooseSwapPresentMode(
            const std::vector<VkPresentModeKHR>& availablePresentModes
        );
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        VkFormat m_swapChainImageFormat;
        VkFormat m_swapChainDepthFormat;
        VkExtent2D m_swapChainExtent;

        std::vector<VkFramebuffer> m_swapChainFramebuffers;
        VkRenderPass m_renderPass;

        std::vector<VkImage> m_depthImages;
        std::vector<VkDeviceMemory> m_depthImageMemories;
        std::vector<VkImageView> m_depthImageViews;
        std::vector<VkImage> m_swapChainImages;
        std::vector<VkImageView> m_swapChainImageViews;

        PtWindow& m_window;
        PtDevice& m_device;

        VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;

        std::vector<VkSemaphore> m_imageAvailableSemaphores;
        std::vector<VkSemaphore> m_renderFinishedSemaphores;
        std::vector<VkFence> m_inFlightFences;
        std::vector<VkFence> m_imagesInFlight;
        size_t m_currentFrame = 0;
    };
}
