#pragma once

#include <string>
#include <vector>

#include "Device.h"

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

    class Pipeline {
    public:
        Pipeline(
            Device& device,
            const std::string& vertPath,
            const std::string& fragPath,
            const PipelineConfig& info
        );

        ~Pipeline();

        Pipeline(const Pipeline&) = delete;
        Pipeline& operator=(const Pipeline) = delete;

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

        Device& m_device;
        VkPipeline m_graphicsPipeline;
        VkShaderModule m_vertShaderModule;
        VkShaderModule m_fragShaderModule;
    };
}
