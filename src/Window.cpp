#include "Window.h"

#include <stdio.h>
#include <iostream>

#include <vulkan/vulkan.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>

// static VkAllocationCallbacks* g_Allocator = nullptr;
// static VkInstance g_Instance = VK_NULL_HANDLE;
// static VkPhysicalDevice g_PhysicalDevice = VK_NULL_HANDLE;
// static VkDevice g_Device = VK_NULL_HANDLE;
// static uint32_t g_QueueFamily = (uint32_t)-1;
// static VkQueue g_Queue = VK_NULL_HANDLE;
// static VkDebugReportCallbackEXT g_DebugReport = VK_NULL_HANDLE;
// static VkPipelineCache g_PipelineCache = VK_NULL_HANDLE;
// static VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE

namespace PalmTree {
    static void glfw_error_callback(int error, const char* description) {
        fprintf(stderr, "GLFW error %d: %s", error, description);
    }

    Window::Window(const int width, const int height, const std::string& title) : m_Width(width), m_Height(height), m_Title(title) {
        Init();
    }

    Window::~Window() {
        glfwDestroyWindow(m_WindowHandle);
        glfwTerminate();
    }

    void Window::Init() {
        glfwSetErrorCallback(glfw_error_callback);
        if (glfwInit() == GLFW_FALSE) {
            std::cerr << "Could not initialize GLFW!" << std::endl;

            return;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        m_WindowHandle = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::cout << extensionCount << " extensions supported" << std::endl;

        // ImGuiIO& io = ImGui::GetIO();
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        // //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows}
        //
        // ImGui::StyleColorsDark();
        //
        // ImGui_ImplGlfw_InitForVulkan(m_WindowHandle, true);
    
    }

    void Window::Run()
    {
        m_Running = true;
    
        while (!glfwWindowShouldClose(m_WindowHandle)) {
            glfwPollEvents();
        }
    }

    void Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
        if (glfwCreateWindowSurface(instance, m_WindowHandle, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface!");
        }
    }
}
