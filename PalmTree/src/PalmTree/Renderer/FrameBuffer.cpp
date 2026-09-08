#include "ptpch.h"
#include "FrameBuffer.h"

#include "RendererBackend.h"
#include "PalmTree/Logging/Log.h"

namespace PalmTree {
    FrameBuffer* FrameBuffer::Create(const FrameBufferSpecification& spec) {
        switch (RendererBackend::GetAPI()) {
            case RendererBackend::API::VULKAN: return CreateVulkan(spec);
            default: PT_CORE_VERIFY(false, "FrameBuffer not supported on current renderer backend");
        }
    }
}
