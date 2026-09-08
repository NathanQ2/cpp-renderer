#pragma once

#include "VulkanDevice.h"
#include "VulkanRenderTarget.h"
#include "PalmTree/Window.h"
#include "PalmTree/Renderer/SwapChain.h"


namespace PalmTree {
    class VulkanSwapChain : public SwapChain, public VulkanRenderTarget {
    public:
        VulkanSwapChain(
            Window& window,
            VulkanDevice& device
        );

        ~VulkanSwapChain();

        VulkanSwapChain(const VulkanSwapChain&) = delete;
        VulkanSwapChain& operator=(const VulkanSwapChain&) = delete;

        VkFramebuffer GetFrameBuffer() override { return m_SwapChainFramebuffers[m_CurrentImageIndex]; }
        VkRenderPass GetRenderPass() override { return m_RenderPass; }
        VkImageView GetImageView(int index) const { return m_SwapChainImageViews[index]; }
        void RecreateSwapChain();
        void CleanupSwapChain();
        size_t GetImageCount() const { return m_SwapChainImages.size(); }
        VkFormat GetSwapChainImageFormat() const { return m_SwapChainImageFormat; }
        VkExtent2D GetSwapChainExtent() const { return m_SwapChainExtent; }
        uint32_t GetWidth() const override { return m_SwapChainExtent.width; }
        uint32_t GetHeight() const override { return m_SwapChainExtent.height; }

        float ExtentAspectRatio() {
            return static_cast<float>(m_SwapChainExtent.width) / static_cast<float>(m_SwapChainExtent.height);
        }

        VkFormat FindDepthFormat();

        VkResult AcquireNextImage();
        VkResult SubmitCommandBuffers(const VkCommandBuffer* buffers);

        bool CompareSwapFormats(VkFormat otherDepthFormat, VkFormat otherImageFormat) const {
            return otherDepthFormat == m_SwapChainDepthFormat && otherImageFormat == m_SwapChainImageFormat;
        }
    private:
        void Init();
        void CleanupSyncObjects();
        void CreateSwapChain();
        void CreateImageViews();
        void CreateDepthResources();
        void CreateRenderPass();
        void CreateFramebuffers();
        void CreateSyncObjects();

        // Helper functions
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
            const std::vector<VkSurfaceFormatKHR>& availableFormats
        );
        VkPresentModeKHR ChooseSwapPresentMode(
            const std::vector<VkPresentModeKHR>& availablePresentModes
        );
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        VkFormat m_SwapChainImageFormat;
        VkFormat m_SwapChainDepthFormat;
        VkExtent2D m_SwapChainExtent;

        std::vector<VkFramebuffer> m_SwapChainFramebuffers;
        VkRenderPass m_RenderPass;

        std::vector<VkImage> m_DepthImages;
        std::vector<VkDeviceMemory> m_DepthImageMemories;
        std::vector<VkImageView> m_DepthImageViews;
        std::vector<VkImage> m_SwapChainImages;
        std::vector<VkImageView> m_SwapChainImageViews;

        Window& m_Window;
        VulkanDevice& m_Device;

        VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

        std::vector<VkSemaphore> m_ImageAvailableSemaphores;
        std::vector<VkSemaphore> m_RenderFinishedSemaphores;
        std::vector<VkFence> m_InFlightFences;
        std::vector<VkFence> m_ImagesInFlight;
        size_t m_CurrentFrame = 0;

        uint32_t m_CurrentImageIndex = 0;
    };
}
