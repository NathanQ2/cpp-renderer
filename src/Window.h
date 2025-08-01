#pragma once

#define GLFW_INCLUDE_VULKAN
#include <string>
#include <GLFW/glfw3.h>

namespace PalmTree {
    class Window {
    public:
        Window(int width, int height, const std::string& title);
        ~Window();

        Window(const Window&) = delete;
        Window &operator=(const Window&) = delete;

        void Init();
        //void Run();

        [[nodiscard]] bool ShouldClose() const { return glfwWindowShouldClose(m_WindowHandle); }

        void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

        [[nodiscard]] VkExtent2D GetExtent() const {
            return { static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height) };
        }
        [[nodiscard]] int GetWidth() const { return m_Width; }
        [[nodiscard]] int GetHeight() const { return m_Height; }
        [[nodiscard]] bool WasWindowResized() const { return m_FrameBufferResized; }
        void ResetWindowResizedFlag() { m_FrameBufferResized = false; }
    private:
        static void FrameBufferResizeCallback(GLFWwindow* windowHandle, int width, int height);
        
        bool m_Running = false;

        int m_Width; 
        int m_Height;
        bool m_FrameBufferResized = false;
        const std::string m_Title;

        GLFWwindow* m_WindowHandle = nullptr;
    };
}
