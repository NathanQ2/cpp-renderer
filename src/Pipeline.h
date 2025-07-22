#pragma once

#include <string>
#include <vector>

#include "Device.h"

namespace PalmTree {
    struct PipelineConfig {
        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };
    
    class Pipeline {
    public:
        Pipeline(
            PalmTreeDevice& device,
            const std::string& vertPath,
            const std::string& fragPath,
            const PipelineConfig& info
        );

        ~Pipeline();

        Pipeline(const Pipeline&) = delete;
        void operator=(const Pipeline) = delete;

        static PipelineConfig DefaultPipelineConfig(uint32_t width, uint32_t height);

        void Bind(VkCommandBuffer commandBuffer);
    private:
        static std::vector<char> ReadFile(const std::string& path);

        void CreateGraphicsPipeline(const std::string& vertPath, const std::string& fragPath, const PipelineConfig& config);

        void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

        PalmTreeDevice& m_Device;
        VkPipeline m_GraphicsPipeline;
        VkShaderModule m_VertShaderModule;
        VkShaderModule m_FragShaderModule;
    };
}
