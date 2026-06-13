#pragma once

#include "Core.h"
#include "Platform/Vulkan/Descriptors.h"
#include "EntityComponentSystem/EntityComponentSystem.h"
#include "Model.h"
#include "Platform/Vulkan/Renderer.h"
#include "Window.h"
#include "EventSystem/ApplicationEvents.h"


namespace PalmTree {
    class Application {
    public:
        Application();
        ~Application() = default;

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        void Run();

        void OnEvent(Event& event);
    private:
        void LoadGameObjects();
        
        bool OnWindowClosed(WindowClosedEvent& event);

        std::shared_ptr<Window> m_Window;
        std::shared_ptr<Device> m_Device;
        std::unique_ptr<Renderer> m_Renderer;

        // NOTE: Must be initialized after Device
        std::unique_ptr<DescriptorPool> m_GlobalPool;

        EntityComponentSystem m_Ecs{};
        
        bool m_Running = true;
    };

    Application* CreateApplication();
}
