#pragma once

#include <string>
#include <vector>

#include "PtDevice.h"

namespace PalmTree {
    struct PipelineConfig {
        PipelineConfig() = default;
        PipelineConfig(const PipelineConfig&) = delete;
        PipelineConfig& operator=(const PipelineConfig&) = delete;

        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };
    
    class PtPipeline {
    public:
        PtPipeline(
            PtDevice& device,
            const std::string& vertPath,
            const std::string& fragPath,
            const PipelineConfig& info
        );

        ~PtPipeline();

        PtPipeline(const PtPipeline&) = delete;
        PtPipeline& operator=(const PtPipeline) = delete;

        static void DefaultPipelineConfig(PipelineConfig& config);

        void Bind(VkCommandBuffer commandBuffer);
    private:
        static std::vector<char> ReadFile(const std::string& path);

        void CreateGraphicsPipeline(const std::string& vertPath, const std::string& fragPath, const PipelineConfig& config);

        void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

        PtDevice& m_Device;
        VkPipeline m_GraphicsPipeline;
        VkShaderModule m_VertShaderModule;
        VkShaderModule m_FragShaderModule;
    };
}
