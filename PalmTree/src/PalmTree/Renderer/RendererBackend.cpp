#include "RendererBackend.h"

namespace PalmTree {
    RendererBackend* RendererBackend::s_Instance = nullptr;
    
    void RendererBackend::Init(API api) {
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

    void RendererBackend::Shutdown() {
        PT_CORE_ASSERT(s_Instance != nullptr, "RendererBackend has not been initialized!");

        delete s_Instance;
        s_Instance = nullptr;
    }

    RendererBackend* RendererBackend::Get() {
        PT_CORE_ASSERT(s_Instance != nullptr, "RendererBackend has not been initialized!");

        return s_Instance;
    }
}
