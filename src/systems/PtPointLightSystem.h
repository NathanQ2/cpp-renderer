#pragma once

#include "../PtPipeline.h"
#include "../PtModel.h"
#include "../PtGameObject.h"
#include "../PtCamera.h"
#include "../PtFrameInfo.h"

#include <memory>
#include <vector>

namespace PalmTree {
    class PtPointLightSystem {
    public:
        PtPointLightSystem(PtDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PtPointLightSystem();

        PtPointLightSystem(const PtPointLightSystem&) = delete;
        PtPointLightSystem& operator=(const PtPointLightSystem&) = delete;

        void Render(FrameInfo& fameInfo);
    private:
        void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void CreatePipeline(VkRenderPass renderPass);
    
        PtDevice& m_Device;
        std::unique_ptr<PtPipeline> m_Pipeline;
        VkPipelineLayout m_PipelineLayout;
    };
}
