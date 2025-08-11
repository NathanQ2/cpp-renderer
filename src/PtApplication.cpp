#include "PtApplication.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>

namespace PalmTree {
    struct SimplePushConstantData {
        glm::mat2 transform{1.0f};
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };
    
    PtApplication::PtApplication()  {
        LoadGameObjects();
        CreatePipelineLayout();
        RecreateSwapChain();
        CreateCommandBuffers();
    }

    PtApplication::~PtApplication() {
        vkDestroyPipelineLayout(m_Device.device(), m_PipelineLayout, nullptr);
    }

    void PtApplication::Run() {
        while (!m_Window.ShouldClose()) {
            glfwPollEvents();
            
            DrawFrame();
        }

        vkDeviceWaitIdle(m_Device.device());
    }

    void PtApplication::LoadGameObjects() {
        std::vector<PtModel::Vertex> vertices {
            { { 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
            { { 0.5, 0.5f }, { 0.0f, 1.0f, 0.0f } },
            { { -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } }
        };

        auto model = std::make_shared<PtModel>(m_Device, vertices);

        auto triangle = PtGameObject::CreateGameObject();
        triangle.model = model;
        triangle.color = glm::vec3(0.1f, 0.8f, 0.1f);
        triangle.transform.translation.x = 0.2f;
        triangle.transform.scale.x = 2.0f;
        triangle.transform.scale.y = 0.5f;
        triangle.transform.rotation = 0.25f * glm::two_pi<float>();

        m_GameObjects.push_back(std::move(triangle));
    }

    void PtApplication::CreatePipelineLayout() {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);
        
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(m_Device.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }
    }

    void PtApplication::CreatePipeline() {
        assert(m_SwapChain != nullptr && "Cannot create pipeline before swap chain!");
        assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");
        PipelineConfig pipelineConfig{};
        PtPipeline::DefaultPipelineConfig(pipelineConfig);

        pipelineConfig.renderPass = m_SwapChain->getRenderPass();
        pipelineConfig.pipelineLayout = m_PipelineLayout;
        m_Pipeline = std::make_unique<PtPipeline>(
            m_Device,
            "simpleShader.vert.spv",
            "simpleShader.frag.spv",
            pipelineConfig
        );
    }

    void PtApplication::CreateCommandBuffers() {
        m_CommandBuffers.resize(m_SwapChain->imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_Device.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

        if (vkAllocateCommandBuffers(m_Device.device(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers!");
        }
    }

    void PtApplication::FreeCommandBuffers() {
        vkFreeCommandBuffers(
            m_Device.device(),
            m_Device.getCommandPool(),
            static_cast<uint32_t>(m_CommandBuffers.size()),
            m_CommandBuffers.data()
        );
        
        m_CommandBuffers.clear();
    }

    void PtApplication::DrawFrame() {
        uint32_t imageIndex;
        auto result = m_SwapChain->acquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            RecreateSwapChain();
            
            return;
        }
        
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed to acquire swap chain image!");
        }

        RecordCommandBuffer(imageIndex);
        result = m_SwapChain->submitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);
        
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.WasWindowResized()) {
            m_Window.ResetWindowResizedFlag();
            RecreateSwapChain();
            
            return;
        }
        
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to present swap chain image!");
        }
    }

    void PtApplication::RecreateSwapChain() {
        // auto extent = m_Window.GetExtent();
        // while (extent.width == 0 || extent.height == 0) {
        //     extent = m_Window.GetExtent();
        //     glfwWaitEvents();
        // }

        // vkDeviceWaitIdle(m_Device.device());
        // m_SwapChain = std::make_unique<SwapChain>(m_Window, m_Device);
        m_SwapChain->recreateSwapChain();
        if (m_SwapChain->imageCount() != m_CommandBuffers.size()) {
            // Vulkan will complain if we free 0 command buffers
            if (!m_CommandBuffers.empty())
                FreeCommandBuffers();
            CreateCommandBuffers();
        }

        CreatePipeline();
    }

    void PtApplication::RecordCommandBuffer(int imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(m_CommandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_SwapChain->getRenderPass();
        renderPassInfo.framebuffer = m_SwapChain->getFrameBuffer(imageIndex);

        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = m_SwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color =  { 0.01f, 0.01f, 0.01f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(m_CommandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_SwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(m_SwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, m_SwapChain->getSwapChainExtent()};
        vkCmdSetViewport(m_CommandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(m_CommandBuffers[imageIndex], 0, 1, &scissor);

        RenderGameObjects(m_CommandBuffers[imageIndex]);

        vkCmdEndRenderPass(m_CommandBuffers[imageIndex]);
        if (vkEndCommandBuffer(m_CommandBuffers[imageIndex]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create command buffer!");
        }
    }

    void PtApplication::RenderGameObjects(VkCommandBuffer commandBuffer) {
        m_Pipeline->Bind(commandBuffer);

        for (auto& object : m_GameObjects) {
            object.transform.rotation = glm::mod(object.transform.rotation + 0.01f, glm::two_pi<float>());
            
            SimplePushConstantData push;
            push.offset = object.transform.translation;
            push.color = object.color;
            push.transform = object.transform.getMat();

            vkCmdPushConstants(
                commandBuffer,
                m_PipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push
            );

            object.model->Bind(commandBuffer);
            object.model->Draw(commandBuffer);
        }
    }
}
