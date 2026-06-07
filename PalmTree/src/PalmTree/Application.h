#pragma once

#include "Core.h"
#include "Descriptors.h"
#include "EntityComponentSystem/EntityComponentSystem.h"
#include "Model.h"
#include "Renderer.h"
#include "Window.h"

#include <memory>
#include <vector>

namespace PalmTree {
    class PT_API Application {
    public:
        Application();
        ~Application() = default;

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        void run();

    private:
        void loadGameObjects();

        const int m_width = 800;
        const int m_height = 600;
        const std::string m_title = "PalmTree Window";

        Window m_window = Window(m_width, m_height, m_title);
        Device m_device = Device(m_window);
        Renderer m_renderer = Renderer(m_window, m_device);

        // NOTE: Must be declared after PtDevice
        std::unique_ptr<DescriptorPool> m_globalPool{};
       
        EntityComponentSystem m_ecs{};
    };
    
    Application* createApplication();
}
