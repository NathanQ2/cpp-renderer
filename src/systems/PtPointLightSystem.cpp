#include "PtPointLightSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>

namespace PalmTree {
    PtPointLightSystem::PtPointLightSystem(PtDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : m_Device(device) {
        CreatePipelineLayout(globalSetLayout);
        CreatePipeline(renderPass);
    }

    PtPointLightSystem::~PtPointLightSystem() {
        vkDestroyPipelineLayout(m_Device.device(), m_PipelineLayout, nullptr);
    }

    void PtPointLightSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout) {
        // VkPushConstantRange pushConstantRange{};
        // pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        // pushConstantRange.offset = 0;
        // pushConstantRange.size = sizeof(SimplePushConstantData);
        
        std::vector descriptorSetLayouts{globalSetLayout};
        
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(m_Device.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }
    }

    void PtPointLightSystem::CreatePipeline(VkRenderPass renderPass) {
        assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");
        
        PipelineConfig pipelineConfig{};
        PtPipeline::DefaultPipelineConfig(pipelineConfig);
        pipelineConfig.bindingDescriptions.clear();
        pipelineConfig.attributeDescriptions.clear();

        pipelineConfig.renderPass = renderPass; 
        pipelineConfig.pipelineLayout = m_PipelineLayout;
        m_Pipeline = std::make_unique<PtPipeline>(
            m_Device,
            "pointLight.vert.spv",
            "pointLight.frag.spv",
            pipelineConfig
        );
    }

    void PtPointLightSystem::Render(FrameInfo& frameInfo) {
        m_Pipeline->Bind(frameInfo.commandBuffer);

        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_PipelineLayout,
            0, 
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr
        );
        
         vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
    }
}
