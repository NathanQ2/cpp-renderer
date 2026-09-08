#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include <imgui_impl_vulkan.h>

#include "PalmTree/Layer.h"
#include "PalmTree/Platform/Mac/MacWindow.h"
#include "PalmTree/Platform/Vulkan/VulkanDescriptors.h"
#include "PalmTree/Platform/Vulkan/VulkanRendererBackend.h"

namespace PalmTree {
    class ImGuiLayer : public Layer {
    public:
        ImGuiLayer(const MacWindow& window);
        ~ImGuiLayer() override;

        void InitImGui();

        void Begin();
        void End();
    private:
        void InitImGuiVulkan();

        void ShutdownImGui();
        void ShutdownImGuiVulkan();

        const MacWindow& m_Window;
        VulkanRendererBackend* m_Renderer;

        std::unique_ptr<VulkanDescriptorPool> m_DescriptorPool;
    };
}
