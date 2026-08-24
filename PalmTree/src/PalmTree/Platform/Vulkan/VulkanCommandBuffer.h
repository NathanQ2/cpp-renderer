#pragma once

#include "VulkanBuffer.h"
#include "VulkanFrameBuffer.h"
#include "VulkanPipeline.h"
#include "VulkanSwapChain.h"
#include "PalmTree/Renderer/CommandBuffer.h"

namespace PalmTree {
    class VulkanCommandBuffer : public CommandBuffer {
    public:
        VulkanCommandBuffer(const VulkanDevice& device);
        ~VulkanCommandBuffer() override;

        VulkanCommandBuffer(const VulkanCommandBuffer&) = delete;
        VulkanCommandBuffer& operator=(const VulkanCommandBuffer&) = delete;

        VulkanCommandBuffer(VulkanCommandBuffer&& other) = delete;
        VulkanCommandBuffer& operator=(VulkanCommandBuffer&&) = delete;

        void BindPipeline(std::weak_ptr<Pipeline> pipeline) override;
        void BindDescriptorSet(const DescriptorSet& set) override;
        void PushConstants(uint32_t offset, uint32_t size, void* data) override;
        void BindVertexBuffer(const VertexBuffer& vertex) override;
        void BindIndexBuffer(const IndexBuffer& index) override;
        
        void BeginRenderPass(std::shared_ptr<FrameBuffer> frameBuffer) override;
        void BeginRenderPass(const VulkanSwapChain& swapChain, int imageIndex);
        void EndRenderPass() override;
        
        void DrawIndexed(uint32_t indexCount) override;
        void Draw(uint32_t vertexCount) override;

        VkCommandBuffer GetVkCommandBuffer() const { return m_CommandBuffer; }
    private:
        VkCommandBuffer m_CommandBuffer = nullptr;

        const VulkanDevice& m_Device;

        std::weak_ptr<VulkanPipeline> m_Pipeline;
    };
}
