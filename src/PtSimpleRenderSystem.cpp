#include "PtSimpleRenderSystem.h"

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
    
    PtSimpleRenderSystem::PtSimpleRenderSystem(PtDevice& device, VkRenderPass renderPass) : m_Device(device) {
        CreatePipelineLayout();
        CreatePipeline(renderPass);
    }

    PtSimpleRenderSystem::~PtSimpleRenderSystem() {
        vkDestroyPipelineLayout(m_Device.device(), m_PipelineLayout, nullptr);
    }

    void PtSimpleRenderSystem::CreatePipelineLayout() {
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

    void PtSimpleRenderSystem::CreatePipeline(VkRenderPass renderPass) {
        assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");
        
        PipelineConfig pipelineConfig{};
        PtPipeline::DefaultPipelineConfig(pipelineConfig);

        pipelineConfig.renderPass = renderPass; 
        pipelineConfig.pipelineLayout = m_PipelineLayout;
        m_Pipeline = std::make_unique<PtPipeline>(
            m_Device,
            "simpleShader.vert.spv",
            "simpleShader.frag.spv",
            pipelineConfig
        );
    }

    void PtSimpleRenderSystem::RenderGameObjects(VkCommandBuffer commandBuffer, std::vector<PtGameObject>& gameObjects) {
        m_Pipeline->Bind(commandBuffer);

        for (auto& object : gameObjects) {
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
