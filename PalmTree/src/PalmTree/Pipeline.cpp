#include "Pipeline.h"

#include "Model.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace PalmTree {
    Pipeline::Pipeline(
        Device& device,
        const std::string& vertPath,
        const std::string& fragPath,
        const PipelineConfig& config
    ) : m_Device{device} {
        CreateGraphicsPipeline(vertPath, fragPath, config);
    }

    Pipeline::~Pipeline() {
        vkDestroyShaderModule(m_Device.GetDevice(), m_VertShaderModule, nullptr);
        vkDestroyShaderModule(m_Device.GetDevice(), m_FragShaderModule, nullptr);
        vkDestroyPipeline(m_Device.GetDevice(), m_GraphicsPipeline, nullptr);
    }

    void Pipeline::DefaultPipelineConfig(PipelineConfig& config) {
        config.InputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        config.InputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        config.InputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        config.ViewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        config.ViewportInfo.viewportCount = 1;
        config.ViewportInfo.pViewports = nullptr;
        config.ViewportInfo.scissorCount = 1;
        config.ViewportInfo.pScissors = nullptr;

        config.RasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        config.RasterizationInfo.depthClampEnable = VK_FALSE;
        config.RasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        config.RasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        config.RasterizationInfo.lineWidth = 1.0f;
        config.RasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        config.RasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        config.RasterizationInfo.depthBiasEnable = VK_FALSE;
        config.RasterizationInfo.depthBiasConstantFactor = 0.0f; // Optional
        config.RasterizationInfo.depthBiasClamp = 0.0f; // Optional
        config.RasterizationInfo.depthBiasSlopeFactor = 0.0f; // Optional

        config.MultisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        config.MultisampleInfo.sampleShadingEnable = VK_FALSE;
        config.MultisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        config.MultisampleInfo.minSampleShading = 1.0f; // Optional
        config.MultisampleInfo.pSampleMask = nullptr; // Optional
        config.MultisampleInfo.alphaToCoverageEnable = VK_FALSE; // Optional
        config.MultisampleInfo.alphaToOneEnable = VK_FALSE; // Optional

        config.ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        config.ColorBlendAttachment.blendEnable = VK_FALSE;
        config.ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        config.ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        config.ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
        config.ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        config.ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        config.ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

        config.ColorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        config.ColorBlendInfo.logicOpEnable = VK_FALSE;
        config.ColorBlendInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
        config.ColorBlendInfo.attachmentCount = 1;
        config.ColorBlendInfo.pAttachments = &config.ColorBlendAttachment;
        config.ColorBlendInfo.blendConstants[0] = 0.0f; // Optional
        config.ColorBlendInfo.blendConstants[1] = 0.0f; // Optional
        config.ColorBlendInfo.blendConstants[2] = 0.0f; // Optional
        config.ColorBlendInfo.blendConstants[3] = 0.0f; // Optional

        config.DepthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        config.DepthStencilInfo.depthTestEnable = VK_TRUE;
        config.DepthStencilInfo.depthWriteEnable = VK_TRUE;
        config.DepthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        config.DepthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        config.DepthStencilInfo.minDepthBounds = 0.0f; // Optional
        config.DepthStencilInfo.maxDepthBounds = 1.0f; // Optional
        config.DepthStencilInfo.stencilTestEnable = VK_FALSE;
        config.DepthStencilInfo.front = {}; // Optional
        config.DepthStencilInfo.back = {}; // Optional

        config.DynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        config.DynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        config.DynamicStateInfo.pDynamicStates = config.DynamicStateEnables.data();
        config.DynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(config.DynamicStateEnables.size());
        config.DynamicStateInfo.flags = 0;

        config.BindingDescriptions = Model::Vertex::GetBindingDescriptions();
        config.AttributeDescriptions = Model::Vertex::GetAttributeDescriptions();
    }

    void Pipeline::EnableAlphaBlending(PipelineConfig& config) {
        config.ColorBlendAttachment.blendEnable = VK_TRUE;
        config.ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        config.ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        config.ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        config.ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        config.ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        config.ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        config.ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    }

    void Pipeline::Bind(VkCommandBuffer commandBuffer) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
    }

    std::vector<char> Pipeline::ReadFile(const std::string& path) {
        std::ifstream file(path, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + path);
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }

    void Pipeline::CreateGraphicsPipeline(
        const std::string& vertPath,
        const std::string& fragPath,
        const PipelineConfig& config
    ) {
        assert(
            config.PipelineLayout != VK_NULL_HANDLE &&
            "Cannot create graphics pipeline:: no pipelinelayout provided in config"
        );
        assert(
            config.RenderPass != VK_NULL_HANDLE &&
            "Cannot create graphics pipeline:: no renderPass provided in config"
        );

        auto vertCode = ReadFile(vertPath);
        auto fragCode = ReadFile(fragPath);

        CreateShaderModule(vertCode, &m_VertShaderModule);
        CreateShaderModule(fragCode, &m_FragShaderModule);

        VkPipelineShaderStageCreateInfo shaderStages[2];
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = m_VertShaderModule;
        shaderStages[0].pName = "main";
        shaderStages[0].flags = 0;
        shaderStages[0].pNext = nullptr;
        shaderStages[0].pSpecializationInfo = nullptr;

        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = m_FragShaderModule;
        shaderStages[1].pName = "main";
        shaderStages[1].flags = 0;
        shaderStages[1].pNext = nullptr;
        shaderStages[1].pSpecializationInfo = nullptr;

        auto& bindingDescriptions = config.BindingDescriptions;
        auto& attributeDescriptions = config.AttributeDescriptions;
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &config.InputAssemblyInfo;
        pipelineInfo.pViewportState = &config.ViewportInfo;
        pipelineInfo.pRasterizationState = &config.RasterizationInfo;
        pipelineInfo.pMultisampleState = &config.MultisampleInfo;
        pipelineInfo.pColorBlendState = &config.ColorBlendInfo;
        pipelineInfo.pDynamicState = &config.DynamicStateInfo;
        pipelineInfo.pDepthStencilState = &config.DepthStencilInfo;


        pipelineInfo.layout = config.PipelineLayout;
        pipelineInfo.renderPass = config.RenderPass;
        pipelineInfo.subpass = config.Subpass;

        pipelineInfo.basePipelineIndex = -1;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(m_Device.GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline)
            != VK_SUCCESS) {
            throw std::runtime_error("Failed to create graphics pipeline!");
        }
    }

    void Pipeline::CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        if (vkCreateShaderModule(m_Device.GetDevice(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create shader module!");
        }
    }
}
