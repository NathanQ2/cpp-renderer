#pragma once

#define VK_ENABLE_BETA_EXTENSIONS
#define GLFW_INCLUDE_VULKAN
#include <string>
#include <GLFW/glfw3.h>

namespace PalmTree {
    class Window {
    public:
        Window(int width, int height, const std::string& title);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        void init();

        [[nodiscard]] bool shouldClose() const { return glfwWindowShouldClose(m_windowHandle); }

        void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

        [[nodiscard]] VkExtent2D getExtent() const {
            return {static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height)};
        }

        [[nodiscard]] int getWidth() const { return m_width; }
        [[nodiscard]] int getHeight() const { return m_height; }
        [[nodiscard]] bool wasWindowResized() const { return m_frameBufferResized; }
        void resetWindowResizedFlag() { m_frameBufferResized = false; }

        [[nodiscard]] GLFWwindow* getGLFWWindow() const { return m_windowHandle; }

    private:
        static void frameBufferResizeCallback(GLFWwindow* windowHandle, int width, int height);

        bool m_running = false;

        int m_width;
        int m_height;
        bool m_frameBufferResized = false;
        const std::string m_title;

        GLFWwindow* m_windowHandle = nullptr;
    };
}
