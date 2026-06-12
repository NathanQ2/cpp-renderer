#include "ptpch.h"
#include "Window.h"

#include "Log.h"

#include <stdio.h>

#include <vulkan/vulkan.h>

namespace PalmTree {
    static void glfw_error_callback(int error, const char* description) {
        PT_CORE_ERROR("GLFW error {0}: {1}", error, description);
    }

    Window::Window(const int width, const int height, const std::string& title) : m_Width(width), m_Height(height),
        m_Title(title) {
        Init();
    }

    Window::~Window() {
        glfwDestroyWindow(m_WindowHandle);
        glfwTerminate();
    }

    void Window::Init() {
        glfwSetErrorCallback(glfw_error_callback);
        if (glfwInit() == GLFW_FALSE) {
            PT_CORE_ERROR("Could not initialize GLFW!");

            return;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        m_WindowHandle = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(m_WindowHandle, this);
        glfwSetFramebufferSizeCallback(m_WindowHandle, FrameBufferResizeCallback);

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        PT_CORE_TRACE("{} extensions supported", extensionCount);
    }

    void Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
        if (glfwCreateWindowSurface(instance, m_WindowHandle, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface!");
        }
    }

    void Window::FrameBufferResizeCallback(GLFWwindow* windowHandle, int width, int height) {
        auto* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(windowHandle));

        window->m_FrameBufferResized = true;
        window->m_Width = width,
        window->m_Height = height;
    }
}
