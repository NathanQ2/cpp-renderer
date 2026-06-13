#pragma once


#include "Device.h"

namespace PalmTree {
    struct PipelineConfig {
        PipelineConfig() = default;
        PipelineConfig(const PipelineConfig&) = delete;
        PipelineConfig& operator=(const PipelineConfig&) = delete;

        std::vector<VkVertexInputBindingDescription> BindingDescriptions{};
        std::vector<VkVertexInputAttributeDescription> AttributeDescriptions{};
        VkPipelineViewportStateCreateInfo ViewportInfo;
        VkPipelineInputAssemblyStateCreateInfo InputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo RasterizationInfo;
        VkPipelineMultisampleStateCreateInfo MultisampleInfo;
        VkPipelineColorBlendAttachmentState ColorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo ColorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo DepthStencilInfo;
        std::vector<VkDynamicState> DynamicStateEnables;
        VkPipelineDynamicStateCreateInfo DynamicStateInfo;
        VkPipelineLayout PipelineLayout = nullptr;
        VkRenderPass RenderPass = nullptr;
        uint32_t Subpass = 0;
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

        static void DefaultPipelineConfig(PipelineConfig& config);
        static void EnableAlphaBlending(PipelineConfig& config);

        void Bind(VkCommandBuffer commandBuffer);

    private:
        static std::vector<char> ReadFile(const std::string& path);

        void CreateGraphicsPipeline(
            const std::string& vertPath,
            const std::string& fragPath,
            const PipelineConfig& config
        );

        void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

        Device& m_Device;
        VkPipeline m_GraphicsPipeline;
        VkShaderModule m_VertShaderModule;
        VkShaderModule m_FragShaderModule;
    };
}
