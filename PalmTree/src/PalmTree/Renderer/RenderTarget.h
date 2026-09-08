#pragma once

namespace PalmTree {
    class RenderTarget {
    public:
        virtual ~RenderTarget() = default;
       
        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;

        float GetAspectRatio() const { return static_cast<float>(GetWidth()) / static_cast<float>(GetHeight()); }
    };
}
