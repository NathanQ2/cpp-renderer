#pragma once

#include "PtPipeline.h"
#include "PtModel.h"
#include "PtGameObject.h"

#include <memory>
#include <vector>

namespace PalmTree {
    class PtSimpleRenderSystem {
    public:
        PtSimpleRenderSystem(PtDevice& device, VkRenderPass renderPass);
        ~PtSimpleRenderSystem();

        PtSimpleRenderSystem(const PtSimpleRenderSystem&) = delete;
        PtSimpleRenderSystem& operator=(const PtSimpleRenderSystem&) = delete;

        void Run();
        void RenderGameObjects(VkCommandBuffer commandBuffer, std::vector<PtGameObject>& gameObjects);
    private:
        void LoadGameObjects();
        void CreatePipelineLayout();
        void CreatePipeline(VkRenderPass renderPass);
    
        PtDevice& m_Device;
        std::unique_ptr<PtPipeline> m_Pipeline;
        VkPipelineLayout m_PipelineLayout;
    };
}
