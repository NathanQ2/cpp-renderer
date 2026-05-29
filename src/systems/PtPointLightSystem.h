#pragma once

#include "../PtFrameInfo.h"
#include "../PtModel.h"
#include "../PtPipeline.h"

#include <memory>

namespace PalmTree {
    class PtPointLightSystem {
    public:
        PtPointLightSystem(PtDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PtPointLightSystem();

        PtPointLightSystem(const PtPointLightSystem&) = delete;
        PtPointLightSystem& operator=(const PtPointLightSystem&) = delete;

        void update(FrameInfo& frameInfo, GlobalUBO& ubo);
        void render(FrameInfo& fameInfo);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        PtDevice& m_device;
        std::unique_ptr<PtPipeline> m_pipeline;
        VkPipelineLayout m_pipelineLayout;
    };
}
