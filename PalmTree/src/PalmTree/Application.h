#pragma once

#include "Core.h"
#include "Descriptors.h"
#include "EntityComponentSystem/EntityComponentSystem.h"
#include "Model.h"
#include "Renderer.h"
#include "Window.h"


namespace PalmTree {
    class Application {
    public:
        Application();
        ~Application() = default;

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        void Run();

    private:
        void LoadGameObjects();

        const int m_Width = 800;
        const int m_Height = 600;
        const std::string m_Title = "PalmTree Window";

        Window m_Window = Window(m_Width, m_Height, m_Title);
        Device m_Device = Device(m_Window);
        Renderer m_Renderer = Renderer(m_Window, m_Device);

        // NOTE: Must be declared after PtDevice
        std::unique_ptr<DescriptorPool> m_GlobalPool{};

        EntityComponentSystem m_Ecs{};
    };

    Application* CreateApplication();
}
