#pragma once

namespace PalmTree {
    class CommandBuffer;
    class SwapChain;
    class RenderTarget;

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
        static void EndFrame() { Get()->EndFrameImpl(); }

        static void BeginRenderPass(RenderTarget& target) { Get()->BeginRenderPassImpl(target); }
        static void EndRenderPass() { Get()->EndRenderPassImpl(); }

        static void BeginSwapChainRenderPass() { Get()->BeginSwapChainRenderPassImpl(); }
        static void EndSwapChainRenderPass() { Get()->EndSwapChainRenderPassImpl(); }

        static CommandBuffer& GetCurrentCommandBuffer() { return Get()->GetCurrentCommandBufferImpl(); }

        static SwapChain& GetSwapChain() { return Get()->GetSwapChainImpl(); }
        static int GetInFlightFrameIndex() { return Get()->GetInFlightFrameIndexImpl(); };

        virtual ~RendererBackend() = default;

        virtual API GetAPIImpl() = 0;

        virtual bool BeginFrameImpl() = 0;
        virtual void EndFrameImpl() = 0;

        virtual void BeginRenderPassImpl(RenderTarget& target) = 0;
        virtual void EndRenderPassImpl() = 0;

        virtual void BeginSwapChainRenderPassImpl() = 0;
        virtual void EndSwapChainRenderPassImpl() = 0;

        virtual CommandBuffer& GetCurrentCommandBufferImpl() = 0;

        virtual SwapChain& GetSwapChainImpl() = 0;
        virtual int GetInFlightFrameIndexImpl() = 0;
    private:
        static void InitVulkan();

        static RendererBackend* s_Instance;
    };
}
