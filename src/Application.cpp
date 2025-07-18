#include "Application.h"

#include <stdexcept>

namespace PalmTree {
    Application::Application()  {
        CreatePipelineLayout();
        CreatePipeline();
        CreateCommandBuffers();
    }

    Application::~Application() {
        vkDestroyPipelineLayout(m_Device.device(), m_PipelineLayout, nullptr);
    }


    void Application::Run() {
        m_Window.Run();
    }

    void Application::CreatePipelineLayout() {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(m_Device.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }
    }

    void Application::CreatePipeline() {
        auto pipelineConfig = Pipeline::DefaultPipelineConfig(m_SwapChain.width(), m_SwapChain.height());

        pipelineConfig.renderPass = m_SwapChain.getRenderPass();
        pipelineConfig.pipelineLayout = m_PipelineLayout;
        m_Pipeline = std::make_unique<Pipeline>(
            m_Device,
            "../shaders/simpleShader.vert.spv",
            "../shaders/simpleShader.frag.spv",
            pipelineConfig
        );
    }

    void Application::CreateCommandBuffers() {
    }

    void Application::DrawFrame() {
    }
}
