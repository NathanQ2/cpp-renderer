#pragma once

#include <cstdint>

namespace PalmTree {
    struct FrameBufferSpecification {
        uint32_t Width;
        uint32_t Height;
        
        // bool SwapChainTarget = false;
    };
    
    class FrameBuffer {
    public:
        static FrameBuffer* Create(const FrameBufferSpecification& spec);
        
        virtual ~FrameBuffer() = default;
        
        virtual void Invalidate() = 0;
        virtual void Resize(uint32_t width, uint32_t height) = 0;
        
        virtual const FrameBufferSpecification& GetSpec() const = 0;
        
        const uint32_t GetWidth() const { return GetSpec().Width; }
        const uint32_t GetHeight() const { return GetSpec().Height; }
    private:
        static FrameBuffer* CreateVulkan(const FrameBufferSpecification& spec);
    };
}
