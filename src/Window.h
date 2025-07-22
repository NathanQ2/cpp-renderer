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
    private:
        bool m_Running = false;

        const int m_Width; 
        const int m_Height;
        const std::string m_Title;

        GLFWwindow* m_WindowHandle = nullptr;
    };
}
