#pragma once

#include <cstdint>

#include <imgui.h>

#include "RenderTarget.h"

namespace PalmTree {
    struct FrameBufferSpecification {
        uint32_t Width;
        uint32_t Height;
    };

    class FrameBuffer : public RenderTarget {
    public:
        static FrameBuffer* Create(const FrameBufferSpecification& spec);

        virtual ~FrameBuffer() = default;

        virtual void Invalidate() = 0;
        virtual void Resize(uint32_t width, uint32_t height) = 0;

        virtual const FrameBufferSpecification& GetSpec() const = 0;

        virtual ImTextureID CreateImTextureID() = 0;

        uint32_t GetWidth() const override { return GetSpec().Width; }
        uint32_t GetHeight() const override { return GetSpec().Height; }
    private:
        static FrameBuffer* CreateVulkan(const FrameBufferSpecification& spec);
    };
}
