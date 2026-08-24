#pragma once

#include "../Logging/Log.h"
#include "RendererConstants.h"

namespace PalmTree {
    class VulkanRendererBackend;
    class Model;
    class CommandBuffer;
    class FrameBuffer;

    class RendererBackend {
    public:
        enum class API {
            NONE = 0,
            VULKAN = 1
        };

        static void Init(API api);

        static void Shutdown();

        static RendererBackend* Get();

        static API GetAPI() { return Get()->GetAPIImpl(); }

        static bool BeginFrame() { return Get()->BeginFrameImpl(); }
        static void EndFrame() { return Get()->EndFrameImpl(); }
        
        static void BeginSwapChainRenderPass() { return Get()->BeginSwapChainRenderPassImpl(); }
        static void EndSwapChainRenderPass() { return Get()->EndSwapChainRenderPassImpl(); }

        static void BeginRenderPass(const std::shared_ptr<FrameBuffer>& frameBuffer) { return Get()->BeginRenderPassImpl(frameBuffer); }
        static void EndRenderPass() { return Get()->EndRenderPassImpl(); }

        static CommandBuffer& GetCurrentCommandBuffer() { return Get()->GetCurrentCommandBufferImpl(); }

        static int GetSwapChainFrameIndex() { return Get()->GetSwapChainFrameIndexImpl(); }

        static float GetSwapChainAspectRatio() { return Get()->GetSwapChainAspectRatioImpl(); }

        virtual ~RendererBackend() = default;

        virtual API GetAPIImpl() = 0;

        virtual bool BeginFrameImpl() = 0;
        virtual void EndFrameImpl() = 0;
        
        virtual void BeginSwapChainRenderPassImpl() = 0;
        virtual void EndSwapChainRenderPassImpl() = 0;

        virtual void BeginRenderPassImpl(std::shared_ptr<FrameBuffer> frameBuffer) = 0;
        virtual void EndRenderPassImpl() = 0;

        virtual CommandBuffer& GetCurrentCommandBufferImpl() = 0;

        virtual int GetSwapChainFrameIndexImpl() const = 0;

        virtual float GetSwapChainAspectRatioImpl() const = 0;
    private:
        static void InitVulkan();

        static RendererBackend* s_Instance;
    };
}
