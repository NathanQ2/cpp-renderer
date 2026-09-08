#include "Application.h"

#include "EntityComponentSystem/EntityComponentSystem.h"

#include <chrono>

#include "Logging/DataLogger.h"
#include "Logging/DataLoggerUI.h"
#include "Platform/Mac/MacWindow.h"

namespace PalmTree {
    Application* Application::s_Instance = nullptr;

    Application::Application() {
        PT_CORE_VERIFY(s_Instance == nullptr, "Application already exists!");
        s_Instance = this;

        DataLogger::Init();

        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(PT_BIND_EVENT_FN(Application::OnEvent));

        RendererBackend::Init(RendererBackend::API::VULKAN);

        m_ImGuiLayer = PushOverlay<ImGuiLayer>(dynamic_cast<MacWindow&>(*m_Window));

        m_CollisionSystem = std::make_shared<CollisionSystem>();
        m_Ecs.RegisterSystem(
            m_CollisionSystem,
            SignatureBuilder<TransformComponent, ColliderComponent>(m_Ecs.GetComponentManager()).Build()
        );

        m_PhysicsSystem = std::make_shared<PhysicsSystem>();
        m_Ecs.RegisterSystem(
            m_PhysicsSystem,
            SignatureBuilder<TransformComponent, RigidBodyComponent>(m_Ecs.GetComponentManager()).Build()
        );
    }

    Application::~Application() {
        RendererBackend::Shutdown();
    }

    void Application::Run() {
        auto currentTime = std::chrono::high_resolution_clock::now();
        m_ApplicationStartTime = currentTime;

        PushOverlay<DataLoggerUI>(m_ApplicationStartTime);

        m_ImGuiLayer->InitImGui();

        LoopEnabledLayers([](Layer* layer) { layer->OnStart(); });

        while (m_Running) {
            m_Window->OnUpdate();

            auto newTime = std::chrono::steady_clock::now();
            float frameTime = std::chrono::duration<float>(newTime - currentTime).count();
            m_Logger.Record("FrameTime", frameTime);
            currentTime = newTime;
            DataLogger::SetTimestamp(currentTime);

            OnUpdate(frameTime);
        }

        LoopEnabledLayers([](Layer* layer) { layer->OnEnd(); });
    }

    void Application::OnEvent(Event& event) {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowClosedEvent>(PT_BIND_EVENT_FN(Application::OnWindowClosed));

        for (auto it = m_LayerStack.End(); it != m_LayerStack.Begin();) {
            Layer* layer = *--it;
            if (layer->IsEnabled()) {
                bool handled = layer->OnEvent(event);
                if (handled) break;
            }
        }
    }

    bool Application::OnWindowClosed(WindowClosedEvent&) {
        m_Running = false;

        return true;
    }

    void Application::LoopEnabledLayers(std::function<void(Layer*)> func) {
        for (auto it = m_LayerStack.Begin(); it != m_LayerStack.End(); ++it) {
            Layer* layer = *it;

            if (layer->IsEnabled()) func(layer);
        }
    }
}
