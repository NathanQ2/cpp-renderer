#include "ptpch.h"
#include "Pipeline.h"

#include "PalmTree/Logging/Log.h"

namespace PalmTree {
    Pipeline* Pipeline::Create(CreateInfo& createInfo) {
        switch (RendererBackend::GetAPI()) {
            case RendererBackend::API::VULKAN: return CreateVulkan(createInfo);
            default: PT_CORE_ASSERT(false, "Unsupported RendererBackend!");
        }

        return nullptr;
    }
}
