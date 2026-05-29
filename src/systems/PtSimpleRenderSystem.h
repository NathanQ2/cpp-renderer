#pragma once

#include "../PtFrameInfo.h"
#include "../PtModel.h"
#include "../PtPipeline.h"

#include <memory>

namespace PalmTree {
    class PtSimpleRenderSystem {
    public:
        PtSimpleRenderSystem(PtDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PtSimpleRenderSystem();

        PtSimpleRenderSystem(const PtSimpleRenderSystem&) = delete;
        PtSimpleRenderSystem& operator=(const PtSimpleRenderSystem&) = delete;

        void renderGameObjects(FrameInfo& fameInfo);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        PtDevice& m_device;
        std::unique_ptr<PtPipeline> m_pipeline;
        VkPipelineLayout m_pipelineLayout;
    };
}
