// Implementation based on glfw/vulkan example from PalmTree/PalmTree/vendor/imgui/examples/example_glfw_vulkan

#include "ptpch.h"
#include "ImGuiLayer.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <GLFW/glfw3.h>

#include <implot.h>

#include "PalmTree/Renderer/RendererConstants.h"

// Defined in imgui_impl_glfw.cpp
ImGuiKey ImGui_ImplGlfw_KeyToImGuiKey(int keycode, int scancode);

namespace PalmTree {
    ImGuiLayer::ImGuiLayer(const MacWindow& window) : Layer("ImGui"), m_Window(window) {
        PT_CORE_ASSERT(
            RendererBackend::GetAPI() == RendererBackend::API::VULKAN,
            "ImGuiLayer requires the vulkan rendering backend!"
        );

        m_Renderer = VulkanRendererBackend::Get();
    }

    ImGuiLayer::~ImGuiLayer() {
        ShutdownImGui();
    }

    void ImGuiLayer::Begin() {
        // Start the Dear ImGui frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
    }

    void ImGuiLayer::End() {
        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_Renderer->GetCurrentVkCommandBuffer());
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    void ImGuiLayer::InitImGui() {
        float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImPlot::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGuiStyle& style = ImGui::GetStyle();
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // Setup scaling
        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(main_scale);
        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
        style.FontScaleDpi = main_scale;
        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)

        switch (RendererBackend::GetAPI()) {
            case RendererBackend::API::VULKAN:
                InitImGuiVulkan();

                break;
            default:
                PT_CORE_ASSERT(false, "ImGuiLayer does not support the current renderering backend!");

                break;
        }
    }

    void ImGuiLayer::InitImGuiVulkan() {
        VulkanDevice& device = m_Renderer->GetDevice();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForVulkan(m_Window.GetGLFWWindow(), true);
        m_DescriptorPool = VulkanDescriptorPool::Builder(device)
            .SetMaxSets(IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE)
            .Build();

        VulkanSwapChain& swapChain = dynamic_cast<VulkanSwapChain&>(m_Renderer->GetSwapChain());

        ImGui_ImplVulkan_InitInfo initInfo{};
        initInfo.ApiVersion = VK_HEADER_VERSION_COMPLETE;
        initInfo.Instance = device.GetInstance();
        initInfo.PhysicalDevice = device.GetPhysicalDevice();
        initInfo.Device = device.GetDevice();
        initInfo.Queue = device.GraphicsQueue();
        initInfo.DescriptorPool = m_DescriptorPool->GetDescriptorPool();
        initInfo.MinImageCount = swapChain.GetImageCount();
        initInfo.ImageCount = RendererConstants::MAX_FRAMES_IN_FLIGHT;
        // initInfo.PipelineCache = VK_NULL_HANDLE;
        // initInfo.Allocator = VK_NULL_HANDLE;
        initInfo.RenderPass = swapChain.GetRenderPass();
        initInfo.Subpass = 0;
        initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        // initInfo.CheckVkResultFn = check_vk_result;
        ImGui_ImplVulkan_Init(&initInfo);
    }

    void ImGuiLayer::ShutdownImGui() {
        switch (RendererBackend::GetAPI()) {
            case RendererBackend::API::VULKAN:
                ShutdownImGuiVulkan();

                break;
            default:
                PT_CORE_ASSERT(false, "ImGuiLayer does not support the current rendering backend!");

                break;
        }

        ImPlot::DestroyContext();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::ShutdownImGuiVulkan() {
        ImGui_ImplVulkan_Shutdown();
    }
}
