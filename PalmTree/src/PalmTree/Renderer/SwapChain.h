#pragma once

#include "RenderTarget.h"

namespace PalmTree {
    class SwapChain : public RenderTarget {
    public:
        SwapChain() = default;
        virtual ~SwapChain() = default;
    };
}
