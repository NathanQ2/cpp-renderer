#include "ptpch.h"
#include "VulkanCommandBuffer.h"

#include "VulkanFrameBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanPipeline.h"
#include "VulkanRendererBackend.h"
#include "VulkanVertexBuffer.h"

namespace PalmTree {
    VulkanCommandBuffer::VulkanCommandBuffer(
        const VulkanDevice& device
    ) : m_Device(device) {
        const VkCommandBufferAllocateInfo info {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = device.GetCommandPool(),
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        if (vkAllocateCommandBuffers(device.GetDevice(), &info, &m_CommandBuffer) != VK_SUCCESS) {
            PT_CORE_ERROR("Failed to allocate command buffers!");
        }
    }

    VulkanCommandBuffer::~VulkanCommandBuffer() {
        if (m_CommandBuffer) {
            vkFreeCommandBuffers(m_Device.GetDevice(), m_Device.GetCommandPool(), 1, &m_CommandBuffer);
        }
    }

    void VulkanCommandBuffer::BindPipeline(const std::weak_ptr<Pipeline> pipeline) {
        const std::shared_ptr vulkanPipeline = std::dynamic_pointer_cast<VulkanPipeline>(pipeline.lock());
        PT_CORE_ASSERT(vulkanPipeline != nullptr, "A valid VulkanPipeline must be provided!")

        vkCmdBindPipeline(
            m_CommandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            vulkanPipeline->GetVkPipeline()
        );

        m_Pipeline = vulkanPipeline;
    }

    void VulkanCommandBuffer::BindDescriptorSet(const DescriptorSet& set) {
        const std::shared_ptr<VulkanPipeline> pipeline = m_Pipeline.lock();
        PT_CORE_ASSERT(
            pipeline != nullptr,
            "A valid VulkanPipeline must be bound to the current VulkanCommandBuffer to bind a DescriptorSet!"
        );


        VkDescriptorSet vkDescriptorSet = dynamic_cast<const VulkanDescriptorSet&>(set).GetVkDescriptorSet();
        vkCmdBindDescriptorSets(
            m_CommandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipeline->GetPipelineLayout(),
            0,
            1,
            &vkDescriptorSet,
            0,
            nullptr
        );
    }

    void VulkanCommandBuffer::PushConstants(
        uint32_t offset,
        uint32_t size,
        void* data
    ) {
        const std::shared_ptr<VulkanPipeline> shared = m_Pipeline.lock();
        PT_CORE_ASSERT(
            shared != nullptr,
            "A valid VulkanPipeline must be bound to the current VulkanCommandBuffer to bind add push constants!"
        );

        vkCmdPushConstants(
            m_CommandBuffer,
            shared->GetPipelineLayout(),
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            offset,
            size,
            data
        );
    }

    void VulkanCommandBuffer::BindVertexBuffer(const VertexBuffer& vertex) {
        VkBuffer buffers[]{dynamic_cast<const VulkanVertexBuffer&>(vertex).GetVkBuffer()};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, buffers, offsets);
    }

    void VulkanCommandBuffer::BindIndexBuffer(const IndexBuffer& index) {
        VkBuffer buffer = dynamic_cast<const VulkanIndexBuffer&>(index).GetVkBuffer();

        vkCmdBindIndexBuffer(m_CommandBuffer, buffer, 0, VK_INDEX_TYPE_UINT32);
    }
    
    void VulkanCommandBuffer::BeginRenderPass(std::shared_ptr<FrameBuffer> frameBuffer) {
        std::shared_ptr<VulkanFrameBuffer> vulkFrameBuffer = std::dynamic_pointer_cast<VulkanFrameBuffer>(frameBuffer);
        
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = vulkFrameBuffer->GetVkRenderPass();
        renderPassInfo.framebuffer = vulkFrameBuffer->GetVkFrameBuffer();
        
        uint32_t width = frameBuffer->GetWidth();
        uint32_t height = frameBuffer->GetHeight();
        VkExtent2D extent = VkExtent2D {
            .width = width,
            .height = height
        };

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = extent;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(m_CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float> (width);
        viewport.height = static_cast<float>(height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, extent};
        vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);
    }

    void VulkanCommandBuffer::BeginRenderPass(const VulkanSwapChain& swapChain, int imageIndex) {
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapChain.GetRenderPass();
        renderPassInfo.framebuffer = swapChain.GetFrameBuffer(imageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChain.GetSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(m_CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChain.GetSwapChainExtent().width);
        viewport.height = static_cast<float>(swapChain.GetSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, swapChain.GetSwapChainExtent()};
        vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);
    }

    void VulkanCommandBuffer::EndRenderPass() {
        vkCmdEndRenderPass(m_CommandBuffer);
    }

    void VulkanCommandBuffer::DrawIndexed(uint32_t indexCount) {
        vkCmdDrawIndexed(m_CommandBuffer, indexCount, 1, 0, 0, 0);
    }

    void VulkanCommandBuffer::Draw(uint32_t vertexCount) {
        vkCmdDraw(m_CommandBuffer, vertexCount, 1, 0, 0);
    }
}
