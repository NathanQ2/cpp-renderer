#pragma once
#include "Buffer.h"
#include "Pipeline.h"

namespace PalmTree {
    class CommandBuffer {
    public:
        virtual ~CommandBuffer() = default;

        virtual void BeginRenderPass(std::shared_ptr<FrameBuffer> frameBuffer) = 0;
        virtual void EndRenderPass() = 0;
        virtual void BindPipeline(std::weak_ptr<Pipeline> pipeline) = 0;
        virtual void BindDescriptorSet(const DescriptorSet& set) = 0;
        virtual void PushConstants(
            uint32_t offset,
            uint32_t size,
            void* push
        ) = 0;
        virtual void BindVertexBuffer(const VertexBuffer& vertex) = 0;
        virtual void BindIndexBuffer(const IndexBuffer& index) = 0;
        virtual void DrawIndexed(uint32_t indexCount) = 0;
        virtual void Draw(uint32_t vertexCount) = 0;
    };
}
