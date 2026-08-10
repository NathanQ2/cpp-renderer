#include "PointLightSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <map>
#include <glm/ext/matrix_transform.hpp>

#include "PalmTree/Renderer/CommandBuffer.h"

namespace PalmTree {
    struct PointLightPushConstants {
        glm::vec4 Position{};
        glm::vec4 Color{};
        float Radius;
    };

    PointLightSystem::PointLightSystem(DescriptorSetLayout& globalSetLayout) {
        Pipeline::CreateInfo info{
            .VertexShaderPath = "../PalmTree/pointLight.vert.spv",
            .FragmentShaderPath = "../PalmTree/pointLight.frag.spv",
            .PushConstants = {
                {
                    .Offset = 0,
                    .Size = sizeof(PointLightPushConstants)
                }
            },
            .DescriptorSetLayout = globalSetLayout,
            .EnableAlphaBlending = true,
            .EnableVertexAttributes = false
        };

        m_Pipeline = std::shared_ptr<Pipeline>(Pipeline::Create(info));
    }

    void PointLightSystem::Update(FrameInfo& frameInfo) {
        auto rotateLight = glm::rotate(glm::mat4(1.0f), frameInfo.FrameTime, {0.0f, -1.0f, 0.0f});

        int lightIndex = 0;
        for (Id id : m_Ids) {
            PT_CORE_ASSERT(lightIndex < MAX_LIGHTS, "Point lights exceed maximum specified");

            GameObject& obj = m_Ecs->GetObject(id);

            obj.GetTransform()->Translation = glm::vec3(rotateLight * glm::vec4(obj.GetTransform()->Translation, 1.0f));

            frameInfo.GlobalUBO.PointLights[lightIndex].Position = glm::vec4(obj.GetTransform()->Translation, 1.0f);
            PointLightComponent* light = obj.GetComponent<PointLightComponent>();
            PT_CORE_ASSERT(light, "Object must have PointLightComponent");
            frameInfo.GlobalUBO.PointLights[lightIndex].Color = glm::vec4(light->Color, light->LightIntensity);

            lightIndex++;
        }

        frameInfo.GlobalUBO.NumLights = lightIndex;
    }

    void PointLightSystem::Render(FrameInfo& frameInfo) {
        CommandBuffer& cmds = RendererBackend::GetCurrentCommandBuffer();
        std::map<float, Id> sorted;
        for (Id id : m_Ids) {
            GameObject& obj = m_Ecs->GetObject(id);

            auto offset = frameInfo.Camera.GetPosition() - obj.GetTransform()->Translation;
            float distSquared = glm::dot(offset, offset);
            sorted[distSquared] = obj.GetId();
        }

        cmds.BindPipeline(m_Pipeline);

        cmds.BindDescriptorSet(frameInfo.GlobalDescriptorSet);

        for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
            auto& obj = m_Ecs->GetObject(it->second);

            PointLightPushConstants push{};
            push.Position = glm::vec4(obj.GetTransform()->Translation, 1.0f);
            PointLightComponent* light = obj.GetComponent<PointLightComponent>();
            PT_CORE_ASSERT(light, "Object must have PointLightComponent");
            push.Color = glm::vec4(light->Color, light->LightIntensity);
            push.Radius = obj.GetTransform()->Scale.x;

            cmds.PushConstants(0, sizeof(PointLightPushConstants), &push);

            cmds.Draw(6);
        }
    }
}
