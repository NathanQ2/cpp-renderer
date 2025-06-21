#pragma once

#define GLFW_INCLUDE_VULKAN
#include <string>
#include <GLFW/glfw3.h>

class Application {
public:
    Application();
    ~Application();

    Application(const Application&) = delete;
    Application &operator=(const Application&) = delete;

    void Init();
    void Run();
private:
    bool m_Running = false;

    const int m_Width = 1280;
    const int m_Height = 720;
    const std::string m_Title = "Test Window";

    GLFWwindow* m_WindowHandle = nullptr;
};
