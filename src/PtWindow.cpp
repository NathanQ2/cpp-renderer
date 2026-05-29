#include "PtWindow.h"

#include <iostream>
#include <stdio.h>

#include <vulkan/vulkan.h>

namespace PalmTree {
    static void glfw_error_callback(int error, const char* description) {
        fprintf(stderr, "GLFW error %d: %s", error, description);
    }

    PtWindow::PtWindow(const int width, const int height, const std::string& title) : m_width(width), m_height(height),
        m_title(title) {
        init();
    }

    PtWindow::~PtWindow() {
        glfwDestroyWindow(m_windowHandle);
        glfwTerminate();
    }

    void PtWindow::init() {
        glfwSetErrorCallback(glfw_error_callback);
        if (glfwInit() == GLFW_FALSE) {
            std::cerr << "Could not initialize GLFW!" << std::endl;

            return;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        m_windowHandle = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(m_windowHandle, this);
        glfwSetFramebufferSizeCallback(m_windowHandle, frameBufferResizeCallback);

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::cout << extensionCount << " extensions supported" << std::endl;
    }

    void PtWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
        if (glfwCreateWindowSurface(instance, m_windowHandle, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface!");
        }
    }

    void PtWindow::frameBufferResizeCallback(GLFWwindow* windowHandle, int width, int height) {
        auto* window = reinterpret_cast<PtWindow*>(glfwGetWindowUserPointer(windowHandle));

        window->m_frameBufferResized = true;
        window->m_width = width,
            window->m_height = height;
    }
}
