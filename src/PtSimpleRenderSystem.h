#pragma once

#include "PtPipeline.h"
#include "PtModel.h"
#include "PtGameObject.h"
#include "PtCamera.h"
#include "PtFrameInfo.h"

#include <memory>
#include <vector>

namespace PalmTree {
    class PtSimpleRenderSystem {
    public:
        PtSimpleRenderSystem(PtDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PtSimpleRenderSystem();

        PtSimpleRenderSystem(const PtSimpleRenderSystem&) = delete;
        PtSimpleRenderSystem& operator=(const PtSimpleRenderSystem&) = delete;

        void Run();
        void RenderGameObjects(FrameInfo& fameInfo, std::vector<PtGameObject>& gameObjects);
    private:
        void LoadGameObjects();
        void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void CreatePipeline(VkRenderPass renderPass);
    
        PtDevice& m_Device;
        std::unique_ptr<PtPipeline> m_Pipeline;
        VkPipelineLayout m_PipelineLayout;
    };
}
