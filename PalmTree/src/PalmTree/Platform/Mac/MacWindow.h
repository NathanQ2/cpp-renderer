#pragma once
#include <string>

#include "PalmTree/Window.h"

namespace PalmTree {
    class MacWindow : public Window {
    public:
        MacWindow(const WindowProps& props);
        ~MacWindow();
        
        void OnUpdate() override;
        
        [[nodiscard]] bool ShouldClose() override { return glfwWindowShouldClose(m_WindowHandle); };
        [[nodiscard]] VkExtent2D GetExtent() override {
            return VkExtent2D{static_cast<uint32_t>(m_Data.Width), static_cast<uint32_t>(m_Data.Height)};
        }

        [[nodiscard]] int GetWidth() override { return m_Data.Width; }
        [[nodiscard]] int GetHeight() override { return m_Data.Height; }
        
        void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface) override;
        void SetEventCallback(EventCallbackFn callback) override;
        
        GLFWwindow* GetGLFWWindow() const { return m_WindowHandle; }
    private:
        struct WindowData {
            int Width;
            int Height;
            
            std::string Title;
            
            bool VSync;
            
            EventCallbackFn EventCallback;
        };
        
        void Init(const WindowProps& props);
        void Shutdown();
        
        // void FrameBufferResized(GLFWwindow* window, int width, int height);
        
        GLFWwindow* m_WindowHandle = nullptr;
        
        WindowData m_Data;
    };
}
