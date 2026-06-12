#pragma once

#include "../FrameInfo.h"
#include "../Model.h"
#include "../Pipeline.h"

#include "../EntityComponentSystem/EntityComponentSystem.h"


namespace PalmTree {
    class PointLightSystem : public System {
    public:
        PointLightSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PointLightSystem();

        PointLightSystem(const PointLightSystem&) = delete;
        PointLightSystem& operator=(const PointLightSystem&) = delete;

        void Update(FrameInfo& frameInfo, GlobalUBO& ubo);
        void Render(FrameInfo& fameInfo);

    private:
        void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void CreatePipeline(VkRenderPass renderPass);

        Device& m_Device;
        std::unique_ptr<Pipeline> m_Pipeline;
        VkPipelineLayout m_PipelineLayout{};
    };
}
