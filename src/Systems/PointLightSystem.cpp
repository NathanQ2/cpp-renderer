#include "PointLightSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <map>
#include <stdexcept>
#include <glm/ext/matrix_transform.hpp>

namespace PalmTree {
    struct PointLightPushConstants {
        glm::vec4 position{};
        glm::vec4 color{};
        float radius;
    };

    PointLightSystem::PointLightSystem(
        Device& device,
        VkRenderPass renderPass,
        VkDescriptorSetLayout globalSetLayout
    ) : m_device(device) {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    PointLightSystem::~PointLightSystem() {
        vkDestroyPipelineLayout(m_device.device(), m_pipelineLayout, nullptr);
    }

    void PointLightSystem::update(FrameInfo& frameInfo, GlobalUBO& ubo) {
        auto rotateLight = glm::rotate(glm::mat4(1.0f), frameInfo.frameTime, {0.0f, -1.0f, 0.0f});

        int lightIndex = 0;
        for (Id id : m_ids) {
            assert(lightIndex < MAX_LIGHTS && "Point lights exceed maximum specified");

            GameObject& obj = m_ecs->getObject(id);

            obj.getTransform().translation = glm::vec3(rotateLight * glm::vec4(obj.getTransform().translation, 1.0f));

            ubo.pointLights[lightIndex].position = glm::vec4(obj.getTransform().translation, 1.0f);
            PointLightComponent& light = obj.getComponent<PointLightComponent>();
            ubo.pointLights[lightIndex].color = glm::vec4(light.color, light.lightIntensity);

            lightIndex++;
        }

        ubo.numLights = lightIndex;
    }

    void PointLightSystem::render(FrameInfo& frameInfo) {
        std::map<float, Id> sorted;
        for (Id id : m_ids) {
            GameObject& obj = m_ecs->getObject(id);

            auto offset = frameInfo.camera.getPosition() - obj.getTransform().translation;
            float distSquared = glm::dot(offset, offset);
            sorted[distSquared] = obj.getId();
        }

        m_pipeline->bind(frameInfo.commandBuffer);

        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pipelineLayout,
            0,
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr
        );


        for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
            auto& obj = m_ecs->getObject(it->second);

            PointLightPushConstants push{};
            push.position = glm::vec4(obj.getTransform().translation, 1.0f);
            PointLightComponent& light = obj.getComponent<PointLightComponent>();
            push.color = glm::vec4(light.color, light.lightIntensity);
            push.radius = obj.getTransform().scale.x;

            vkCmdPushConstants(
                frameInfo.commandBuffer,
                m_pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(PointLightPushConstants),
                &push
            );

            vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
        }
    }

    void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
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

        if (vkCreatePipelineLayout(m_device.device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }
    }

    void PointLightSystem::createPipeline(VkRenderPass renderPass) {
        assert(m_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

        PipelineConfig pipelineConfig{};
        Pipeline::defaultPipelineConfig(pipelineConfig);
        Pipeline::enableAlphaBlending(pipelineConfig);
        pipelineConfig.bindingDescriptions.clear();
        pipelineConfig.attributeDescriptions.clear();

        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = m_pipelineLayout;
        m_pipeline = std::make_unique<Pipeline>(
            m_device,
            "pointLight.vert.spv",
            "pointLight.frag.spv",
            pipelineConfig
        );
    }
}
