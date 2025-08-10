#pragma once

#include "PtPipeline.h"
#include "PtWindow.h"
#include "PtSwapChain.h"
#include "PtModel.h"

#include <memory>
#include <vector>

namespace PalmTree {
    class PtApplication {
    public:
        PtApplication();
        ~PtApplication();

        PtApplication(const PtApplication&) = delete;
        PtApplication& operator=(const PtApplication&) = delete;

        void Run();
    private:
        void LoadModels();
        void CreatePipelineLayout();
        void CreatePipeline();
        void CreateCommandBuffers();
        void FreeCommandBuffers();
        void DrawFrame();
        void RecreateSwapChain();
        void RecordCommandBuffer(int imageIndex);
        
        const int m_Width = 800;
        const int m_Height = 600;
        const std::string m_Title = "PalmTree Window";
    
        PtWindow m_Window = PtWindow(m_Width, m_Height, m_Title);
        PtDevice m_Device = PtDevice(m_Window);
        std::unique_ptr<PtSwapChain> m_SwapChain = std::make_unique<PtSwapChain>(m_Window, m_Device);
        std::unique_ptr<PtPipeline> m_Pipeline;
        VkPipelineLayout m_PipelineLayout;
        std::vector<VkCommandBuffer> m_CommandBuffers;

        std::unique_ptr<PtModel> m_Model;
    };
}
