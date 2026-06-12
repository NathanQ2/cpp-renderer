#include "SimpleRenderSystem.h"

#include "../EntityComponentSystem/EntityComponentSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <stdexcept>

namespace PalmTree {
    struct SimplePushConstantData {
        glm::mat4 ModelMatrix{1.0f};
        glm::mat4 NormalMatrix{1.0f};
    };

    SimpleRenderSystem::SimpleRenderSystem(
        Device& device,
        VkRenderPass renderPass,
        VkDescriptorSetLayout globalSetLayout
    ) : m_Device(device) {
        CreatePipelineLayout(globalSetLayout);
        CreatePipeline(renderPass);
    }

    SimpleRenderSystem::~SimpleRenderSystem() {
        vkDestroyPipelineLayout(m_Device.GetDevice(), m_PipelineLayout, nullptr);
    }

    void SimpleRenderSystem::RenderGameObjects(FrameInfo& frameInfo) {
        m_Pipeline->Bind(frameInfo.CommandBuffer);

        vkCmdBindDescriptorSets(
            frameInfo.CommandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_PipelineLayout,
            0,
            1,
            &frameInfo.GlobalDescriptorSet,
            0,
            nullptr
        );

        for (Id id : m_Ids) {
            auto& obj = m_Ecs->GetObject(id);

            SimplePushConstantData push{};
            push.ModelMatrix = obj.GetTransform().Mat4();
            push.NormalMatrix = obj.GetTransform().NormalMatrix();

            vkCmdPushConstants(
                frameInfo.CommandBuffer,
                m_PipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push
            );

            ModelComponent& model = obj.GetComponent<ModelComponent>();
            model.Model->Bind(frameInfo.CommandBuffer);
            model.Model->Draw(frameInfo.CommandBuffer);
        }
    }

    void SimpleRenderSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout) {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        std::vector descriptorSetLayouts{globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(
            m_Device.GetDevice(),
            &pipelineLayoutInfo,
            nullptr,
            &m_PipelineLayout
        ) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }
    }

    void SimpleRenderSystem::CreatePipeline(VkRenderPass renderPass) {
        PT_CORE_ASSERT(m_PipelineLayout != nullptr, "Cannot create pipeline before pipeline layout!");

        PipelineConfig pipelineConfig{};
        Pipeline::DefaultPipelineConfig(pipelineConfig);

        pipelineConfig.RenderPass = renderPass;
        pipelineConfig.PipelineLayout = m_PipelineLayout;
        m_Pipeline = std::make_unique<Pipeline>(
            m_Device,
            "PalmTree/simpleShader.vert.spv",
            "PalmTree/simpleShader.frag.spv",
            pipelineConfig
        );
    }
}
