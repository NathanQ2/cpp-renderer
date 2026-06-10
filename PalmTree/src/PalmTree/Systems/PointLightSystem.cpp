#include "PointLightSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <map>
#include <stdexcept>
#include <glm/ext/matrix_transform.hpp>

namespace PalmTree {
    struct PointLightPushConstants {
        glm::vec4 Position{};
        glm::vec4 Color{};
        float Radius;
    };

    PointLightSystem::PointLightSystem(
        Device& device,
        VkRenderPass renderPass,
        VkDescriptorSetLayout globalSetLayout
    ) : m_Device(device) {
        CreatePipelineLayout(globalSetLayout);
        CreatePipeline(renderPass);
    }

    PointLightSystem::~PointLightSystem() {
        vkDestroyPipelineLayout(m_Device.GetDevice(), m_PipelineLayout, nullptr);
    }

    void PointLightSystem::Update(FrameInfo& frameInfo, GlobalUBO& ubo) {
        auto rotateLight = glm::rotate(glm::mat4(1.0f), frameInfo.FrameTime, {0.0f, -1.0f, 0.0f});

        int lightIndex = 0;
        for (Id id : m_Ids) {
            assert(lightIndex < MAX_LIGHTS && "Point lights exceed maximum specified");

            GameObject& obj = m_Ecs->GetObject(id);

            obj.GetTransform().Translation = glm::vec3(rotateLight * glm::vec4(obj.GetTransform().Translation, 1.0f));

            ubo.PointLights[lightIndex].Position = glm::vec4(obj.GetTransform().Translation, 1.0f);
            PointLightComponent& light = obj.GetComponent<PointLightComponent>();
            ubo.PointLights[lightIndex].Color = glm::vec4(light.Color, light.LightIntensity);

            lightIndex++;
        }

        ubo.NumLights = lightIndex;
    }

    void PointLightSystem::Render(FrameInfo& frameInfo) {
        std::map<float, Id> sorted;
        for (Id id : m_Ids) {
            GameObject& obj = m_Ecs->GetObject(id);

            auto offset = frameInfo.Camera.GetPosition() - obj.GetTransform().Translation;
            float distSquared = glm::dot(offset, offset);
            sorted[distSquared] = obj.GetId();
        }

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


        for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
            auto& obj = m_Ecs->GetObject(it->second);

            PointLightPushConstants push{};
            push.Position = glm::vec4(obj.GetTransform().Translation, 1.0f);
            PointLightComponent& light = obj.GetComponent<PointLightComponent>();
            push.Color = glm::vec4(light.Color, light.LightIntensity);
            push.Radius = obj.GetTransform().Scale.x;

            vkCmdPushConstants(
                frameInfo.CommandBuffer,
                m_PipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(PointLightPushConstants),
                &push
            );

            vkCmdDraw(frameInfo.CommandBuffer, 6, 1, 0, 0);
        }
    }

    void PointLightSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout) {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PointLightPushConstants);

        std::vector descriptorSetLayouts{globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(m_Device.GetDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }
    }

    void PointLightSystem::CreatePipeline(VkRenderPass renderPass) {
        assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

        PipelineConfig pipelineConfig{};
        Pipeline::DefaultPipelineConfig(pipelineConfig);
        Pipeline::EnableAlphaBlending(pipelineConfig);
        pipelineConfig.BindingDescriptions.clear();
        pipelineConfig.AttributeDescriptions.clear();

        pipelineConfig.RenderPass = renderPass;
        pipelineConfig.PipelineLayout = m_PipelineLayout;
        m_Pipeline = std::make_unique<Pipeline>(
            m_Device,
            "PalmTree/pointLight.vert.spv",
            "PalmTree/pointLight.frag.spv",
            pipelineConfig
        );
    }
}
