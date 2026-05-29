#pragma once

#include <string>
#include <vector>

#include "PtDevice.h"

namespace PalmTree {
    struct PipelineConfig {
        PipelineConfig() = default;
        PipelineConfig(const PipelineConfig&) = delete;
        PipelineConfig& operator=(const PipelineConfig&) = delete;

        std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
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

        static void defaultPipelineConfig(PipelineConfig& config);
        static void enableAlphaBlending(PipelineConfig& config);

        void bind(VkCommandBuffer commandBuffer);

    private:
        static std::vector<char> readFile(const std::string& path);

        void createGraphicsPipeline(
            const std::string& vertPath,
            const std::string& fragPath,
            const PipelineConfig& config
        );

        void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

        PtDevice& m_device;
        VkPipeline m_graphicsPipeline;
        VkShaderModule m_vertShaderModule;
        VkShaderModule m_fragShaderModule;
    };
}
