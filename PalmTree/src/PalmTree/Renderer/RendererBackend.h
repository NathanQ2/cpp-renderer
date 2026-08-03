#pragma once

#include "../Logging/Log.h"
#include "RendererConstants.h"

namespace PalmTree {
    class VulkanRendererBackend;
    class Model;
    class CommandBuffer;

    class RendererBackend {
    public:
        enum class API {
            NONE = 0,
            VULKAN = 1
        };

        static void Init(API api) {
            PT_CORE_ASSERT(s_Instance == nullptr, "Renderer backend has already been initialized!");

            switch (api) {
                case API::VULKAN:
                    InitVulkan();

                    break;
                case API::NONE:
                    PT_CORE_ERROR("No renderer backend selected!");

                    break;
            }
        }

        static void Shutdown() {
            PT_CORE_ASSERT(s_Instance != nullptr, "RendererBackend has not been initialized!");

            delete s_Instance;
            s_Instance = nullptr;
        }

        static RendererBackend* Get() {
            PT_CORE_ASSERT(s_Instance != nullptr, "RendererBackend has not been initialized!");

            return s_Instance;
        }

        static API GetAPI() { return Get()->GetAPIImpl(); }

        static bool BeginFrame() { return Get()->BeginFrameImpl(); }
        static void EndFrame() { return Get()->EndFrameImpl(); }

        static void BeginRenderPass() { return Get()->BeginRenderPassImpl(); }
        static void EndRenderPass() { return Get()->EndRenderPassImpl(); }

        static CommandBuffer& GetCurrentCommandBuffer() { return Get()->GetCurrentCommandBufferImpl(); }

        static int GetFrameIndex() { return Get()->GetFrameIndexImpl(); }

        static float GetAspectRatio() { return Get()->GetAspectRatioImpl(); }

        virtual ~RendererBackend() = default;

        virtual API GetAPIImpl() = 0;

        virtual bool BeginFrameImpl() = 0;
        virtual void EndFrameImpl() = 0;

        virtual void BeginRenderPassImpl() = 0;
        virtual void EndRenderPassImpl() = 0;

        virtual CommandBuffer& GetCurrentCommandBufferImpl() = 0;

        virtual int GetFrameIndexImpl() const = 0;

        virtual float GetAspectRatioImpl() const = 0;
    private:
        static void InitVulkan();

        static RendererBackend* s_Instance;
    };
}
