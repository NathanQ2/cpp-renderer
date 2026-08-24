#include "ptpch.h"
#include "SceneRenderer3D.h"

#include "RendererBackend.h"
#include "PalmTree/Application.h"
#include "PalmTree/Renderer/Descriptors.h"

namespace PalmTree {
    SceneRenderer3D::SceneRenderer3D(Window& window, EntityComponentSystem& ecs, Camera& camera) : m_Window(window),
        m_Ecs(ecs), m_Camera(camera) {
        for (int i = 0; i < m_UboBuffers.size(); i++) {
            m_UboBuffers[i] = std::unique_ptr<UniformBuffer<GlobalUBO>>(UniformBuffer<GlobalUBO>::Create());
        }

        m_DescriptorSetLayout = std::unique_ptr<DescriptorSetLayout>(
            DescriptorSetLayout::Builder()
            .AddBinding({0, DescriptorSetBinding::Type::UniformBuffer})
            .Build()
        );

        m_DescriptorSets.reserve(RendererConstants::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < RendererConstants::MAX_FRAMES_IN_FLIGHT; i++) {
            m_DescriptorSets.emplace_back(DescriptorSet::Create(*m_DescriptorSetLayout));
        }

        for (int i = 0; i < m_DescriptorSets.size(); i++) {
            UniformBuffer<GlobalUBO>& ubo = *m_UboBuffers[i];
            m_DescriptorSets[i]->WriteBuffer(0, ubo);
        }

        m_SimpleRenderSystem = std::make_shared<SimpleRenderSystem>(*m_DescriptorSetLayout);
        m_Ecs.RegisterSystem<SimpleRenderSystem>(
            m_SimpleRenderSystem,
            SignatureBuilder<TransformComponent, ModelComponent>(m_Ecs.GetComponentManager()).Build()
        );

        m_PointLightSystem = std::make_shared<PointLightSystem>(*m_DescriptorSetLayout);
        m_Ecs.RegisterSystem<PointLightSystem>(
            m_PointLightSystem,
            SignatureBuilder<TransformComponent, PointLightComponent>(m_Ecs.GetComponentManager()).Build()
        );
    }

    void SceneRenderer3D::Update(float frameTime) {
        int frameIndex = RendererBackend::GetSwapChainFrameIndex();

        // Delete old frame info
        delete m_FrameInfo;


        m_FrameInfo = new FrameInfo{
            frameIndex,
            frameTime,
            m_Camera,
            *m_DescriptorSets[frameIndex],
            {
                m_Camera.GetProjection(),
                m_Camera.GetView(),
                m_Camera.GetInverseView()
            }
        };

        m_PointLightSystem->Update(*m_FrameInfo);

        m_UboBuffers[frameIndex]->WriteToBuffer(&m_FrameInfo->GlobalUBO);
        m_UboBuffers[frameIndex]->Flush();
    }

    void SceneRenderer3D::Render(float frameTime) {
        PT_CORE_ASSERT(m_FrameInfo != nullptr, "m_FrameInfo is nullptr");

        m_SimpleRenderSystem->Render(*m_FrameInfo);
        m_PointLightSystem->Render(*m_FrameInfo);
    }
}
