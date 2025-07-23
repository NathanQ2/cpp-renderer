#pragma once

#include "Pipeline.h"
#include "Window.h"
#include "SwapChain.h"
#include "Model.h"

#include <memory>
#include <vector>

namespace PalmTree {
    class Application {
    public:
        Application();
        ~Application();

        Application(const Application&) = delete;
        Application& operator=(const Application) = delete;

        void Run();
    private:
        void LoadModels();
        void CreatePipelineLayout();
        void CreatePipeline();
        void CreateCommandBuffers();
        void DrawFrame();
        
        const int m_Width = 1280;
        const int m_Height = 720;
        const std::string m_Title = "Test Window";
    
        Window m_Window = Window(m_Width, m_Height, m_Title);
        PalmTreeDevice m_Device = PalmTreeDevice(m_Window);
        SwapChain m_SwapChain = SwapChain(m_Window, m_Device);
        std::unique_ptr<Pipeline> m_Pipeline;
        VkPipelineLayout m_PipelineLayout;
        std::vector<VkCommandBuffer> m_CommandBuffers;

        std::unique_ptr<Model> m_Model;
    };
}
