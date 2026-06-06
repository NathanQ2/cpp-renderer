#pragma once

#include "../FrameInfo.h"
#include "../Model.h"
#include "../Pipeline.h"

#include "../EntityComponentSystem/EntityComponentSystem.h"

#include <memory>

namespace PalmTree {
    class PointLightSystem : public System {
    public:
        PointLightSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PointLightSystem();

        PointLightSystem(const PointLightSystem&) = delete;
        PointLightSystem& operator=(const PointLightSystem&) = delete;

        void update(FrameInfo& frameInfo, GlobalUBO& ubo);
        void render(FrameInfo& fameInfo);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        Device& m_device;
        std::unique_ptr<Pipeline> m_pipeline;
        VkPipelineLayout m_pipelineLayout{};
    };
}
