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
        void Run();

        void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
    private:
        bool m_Running = false;

        const int m_Width; 
        const int m_Height;
        const std::string m_Title;

        GLFWwindow* m_WindowHandle = nullptr;
    };
}
