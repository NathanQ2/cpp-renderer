#include "ptpch.h"
#include "MacWindow.h"

#include <vulkan/vulkan.h>

#include "PalmTree/Log.h"
#include "PalmTree/EventSystem/ApplicationEvents.h"
#include "PalmTree/EventSystem/KeyEvents.h"
#include "PalmTree/EventSystem/MouseEvents.h"

namespace PalmTree {
    static void GLFWErrorCallback(int error, const char* description) {
        PT_CORE_ERROR("GLFW error {0}: {1}", error, description);
    }
    
    static bool g_GLFWInitialized = false;
    
    Window* Window::Create(WindowProps props) {
        return new MacWindow(props);
    }

    MacWindow::MacWindow(const WindowProps& props) {
        Init(props);
    }

    MacWindow::~MacWindow() {
        Shutdown();
    }

    void MacWindow::Init(const WindowProps& props) {
        glfwSetErrorCallback(GLFWErrorCallback);
        if (!g_GLFWInitialized) {
            int status = glfwInit();
            
            PT_CORE_ASSERT(status, "Failed to initialize GLFW!");
        }
        
        m_Data.Width = props.Width;
        m_Data.Height = props.Height;
        m_Data.Title = props.Title;

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        m_WindowHandle = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(m_WindowHandle, &m_Data);
        
        glfwSetWindowSizeCallback(m_WindowHandle, [](GLFWwindow* window, int width, int height) {
            WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            
            data->Width = width;
            data->Height = height;
            
            WindowResizedEvent event{ width, height };
            
            data->EventCallback(event);
        });
        
        glfwSetWindowCloseCallback(m_WindowHandle, [](GLFWwindow* window) {
            WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            
            WindowClosedEvent event{};
            data->EventCallback(event);
        });
        
        glfwSetKeyCallback(m_WindowHandle, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            
            switch (action) {
            case GLFW_PRESS: {
                KeyPressedEvent event{key, 0};
                data->EventCallback(event);
                
                break;
            }
            case GLFW_RELEASE: {
                KeyReleasedEvent event{key};
                data->EventCallback(event);
                
                break;
            }
            case GLFW_REPEAT: {
                KeyPressedEvent event{ key, 1};
                data->EventCallback(event);
                
                break;
            }
            }
        });
        
        glfwSetMouseButtonCallback(m_WindowHandle, [](GLFWwindow* window, int button, int action, int mods) {
            WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            
            switch (action) {
                case GLFW_PRESS: {
                    MouseButtonPressedEvent event{button};
                    data->EventCallback(event);
                    
                    break;
                }
                case GLFW_RELEASE: {
                    MouseButtonReleasedEvent event{button};
                    data->EventCallback(event);
                    
                    break;
                }
            }
        });
        
        glfwSetScrollCallback(m_WindowHandle, [](GLFWwindow* window, double xOffset, double yOffset) {
            WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            
            MouseScrolledEvent event{static_cast<float>(xOffset), static_cast<float>(yOffset)};
            data->EventCallback(event);
        });
        
        glfwSetCursorPosCallback(m_WindowHandle, [](GLFWwindow* window, double xPos, double yPos) {
            WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            
            MouseMovedEvent event{static_cast<float>(xPos), static_cast<float>(yPos)};
            data->EventCallback(event);
        });
        
        // glfwSetFramebufferSizeCallback(m_WindowHandle, FrameBufferResizeCallback);

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        PT_CORE_TRACE("{} extensions supported", extensionCount);
    }

    void MacWindow::Shutdown() {
        glfwDestroyWindow(m_WindowHandle);
    }

    void MacWindow::OnUpdate() {
        glfwPollEvents();
    }
    
    void MacWindow::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
        if (glfwCreateWindowSurface(instance, m_WindowHandle, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface!");
        }
    }

    void MacWindow::SetEventCallback(EventCallbackFn callback) {
        m_Data.EventCallback = callback;
    }

    // void Window::FrameBufferResizeCallback(GLFWwindow* windowHandle, int width, int height) {
    //     auto* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(windowHandle));

    //     window->m_FrameBufferResized = true;
    //     window->m_Width = width,
    //     window->m_Height = height;
    // }
}
