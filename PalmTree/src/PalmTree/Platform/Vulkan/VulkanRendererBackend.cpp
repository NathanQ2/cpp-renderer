#include "VulkanRendererBackend.h"


#include "../../Logging/Log.h"
#include "PalmTree/Application.h"

namespace PalmTree {
    void RendererBackend::InitVulkan() {
        Window& window = Application::Get().GetWindow();

        s_Instance = new VulkanRendererBackend(window);
        VulkanRendererBackend::s_VulkanInstance = dynamic_cast<VulkanRendererBackend*>(s_Instance);
    }

    VulkanRendererBackend* VulkanRendererBackend::s_VulkanInstance = nullptr;

    VulkanRendererBackend::VulkanRendererBackend(Window& window) : m_Window(window) {
        m_Device = std::make_unique<VulkanDevice>(m_Window);
        PT_CORE_TRACE("INIT DEVICE");
        m_SwapChain = std::make_unique<VulkanSwapChain>(m_Window, *m_Device);

        RecreateSwapChain();
        CreateCommandBuffers();

        m_DescriptorPool = VulkanDescriptorPool::Builder(*m_Device)
            .SetMaxSets(RendererConstants::MAX_FRAMES_IN_FLIGHT)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, RendererConstants::MAX_FRAMES_IN_FLIGHT)
            .Build();
    }

    VulkanRendererBackend::~VulkanRendererBackend() {
        FreeCommandBuffers();
    }

    bool VulkanRendererBackend::BeginFrameImpl() {
        PT_CORE_ASSERT(!m_IsFrameStarted, "Can't call begin frame while already in progress!");

        auto result = m_SwapChain->AcquireNextImage(&m_CurrentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            RecreateSwapChain();

            return false;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            PT_CORE_ERROR("Failed to acquire swap chain image!");

            return false;
        }

        m_IsFrameStarted = true;

        VkCommandBuffer commandBuffer = GetCurrentVkCommandBuffer();

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            PT_CORE_ERROR("Failed to begin recording command buffer!");

            return false;
        }

        return true;
    }

    void VulkanRendererBackend::EndFrameImpl() {
        PT_CORE_ASSERT(m_IsFrameStarted, "Can't call end frame while frame is not in progress");

        VkCommandBuffer commandBuffer = GetCurrentVkCommandBuffer();

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            PT_CORE_ERROR("Failed to end command buffer!");

            return;
        }

        auto result = m_SwapChain->SubmitCommandBuffers(&commandBuffer, &m_CurrentImageIndex);

        // TODO: Recreate swap chain when window resized
        // if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.WasWindowResized()) {
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            // m_Window.ResetWindowResizedFlag();
            RecreateSwapChain();
        }
        else if (result != VK_SUCCESS) {
            PT_CORE_ERROR("Failed to present swap chain image!");

            return;
        }

        m_IsFrameStarted = false;
        m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % RendererConstants::MAX_FRAMES_IN_FLIGHT;
    }

    void VulkanRendererBackend::BeginRenderPassImpl() {
        PT_CORE_ASSERT(m_IsFrameStarted, "Can't call BeginSwapChainRenderPass if frame is not in progress!");

        GetCurrentCommandBufferImpl().BeginRenderPass(m_CurrentFrameIndex);
    }

    void VulkanRendererBackend::EndRenderPassImpl() {
        PT_CORE_ASSERT(m_IsFrameStarted, "Can't call EndSwapChainRenderPass if frame is not in progress!");

        GetCurrentCommandBufferImpl().EndRenderPass();
    }

    void VulkanRendererBackend::CreateCommandBuffers() {
        m_CommandBuffers.reserve(RendererConstants::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < RendererConstants::MAX_FRAMES_IN_FLIGHT; i++) {
            m_CommandBuffers.emplace_back(std::make_unique<VulkanCommandBuffer>(*m_Device, *m_SwapChain));
        }
    }

    void VulkanRendererBackend::FreeCommandBuffers() {
        m_CommandBuffers.clear();
    }

    void VulkanRendererBackend::RecreateSwapChain() {
        // auto extent = m_Window.GetExtent();
        // while (extent.width == 0 || extent.height == 0) {
        //     extent = m_Window.GetExtent();
        //     glfwWaitEvents();
        // }

        // vkDeviceWaitIdle(m_Device->device());
        // m_SwapChain = std::make_unique<SwapChain>(m_Window, m_Device);
        m_SwapChain->RecreateSwapChain();
        if (m_SwapChain->GetImageCount() != m_CommandBuffers.size()) {
            // Vulkan will complain if we free 0 command buffers
            if (!m_CommandBuffers.empty())
                FreeCommandBuffers();
            CreateCommandBuffers();
        }
    }
}
