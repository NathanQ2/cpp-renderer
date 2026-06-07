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
    ) : m_device{device} {
        createGraphicsPipeline(vertPath, fragPath, config);
    }

    Pipeline::~Pipeline() {
        vkDestroyShaderModule(m_device.device(), m_vertShaderModule, nullptr);
        vkDestroyShaderModule(m_device.device(), m_fragShaderModule, nullptr);
        vkDestroyPipeline(m_device.device(), m_graphicsPipeline, nullptr);
    }

    void Pipeline::defaultPipelineConfig(PipelineConfig& config) {
        config.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        config.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        config.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        config.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        config.viewportInfo.viewportCount = 1;
        config.viewportInfo.pViewports = nullptr;
        config.viewportInfo.scissorCount = 1;
        config.viewportInfo.pScissors = nullptr;

        config.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        config.rasterizationInfo.depthClampEnable = VK_FALSE;
        config.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        config.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        config.rasterizationInfo.lineWidth = 1.0f;
        config.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        config.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        config.rasterizationInfo.depthBiasEnable = VK_FALSE;
        config.rasterizationInfo.depthBiasConstantFactor = 0.0f; // Optional
        config.rasterizationInfo.depthBiasClamp = 0.0f; // Optional
        config.rasterizationInfo.depthBiasSlopeFactor = 0.0f; // Optional

        config.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        config.multisampleInfo.sampleShadingEnable = VK_FALSE;
        config.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        config.multisampleInfo.minSampleShading = 1.0f; // Optional
        config.multisampleInfo.pSampleMask = nullptr; // Optional
        config.multisampleInfo.alphaToCoverageEnable = VK_FALSE; // Optional
        config.multisampleInfo.alphaToOneEnable = VK_FALSE; // Optional

        config.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        config.colorBlendAttachment.blendEnable = VK_FALSE;
        config.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        config.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        config.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
        config.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        config.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        config.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

        config.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        config.colorBlendInfo.logicOpEnable = VK_FALSE;
        config.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
        config.colorBlendInfo.attachmentCount = 1;
        config.colorBlendInfo.pAttachments = &config.colorBlendAttachment;
        config.colorBlendInfo.blendConstants[0] = 0.0f; // Optional
        config.colorBlendInfo.blendConstants[1] = 0.0f; // Optional
        config.colorBlendInfo.blendConstants[2] = 0.0f; // Optional
        config.colorBlendInfo.blendConstants[3] = 0.0f; // Optional

        config.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        config.depthStencilInfo.depthTestEnable = VK_TRUE;
        config.depthStencilInfo.depthWriteEnable = VK_TRUE;
        config.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        config.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        config.depthStencilInfo.minDepthBounds = 0.0f; // Optional
        config.depthStencilInfo.maxDepthBounds = 1.0f; // Optional
        config.depthStencilInfo.stencilTestEnable = VK_FALSE;
        config.depthStencilInfo.front = {}; // Optional
        config.depthStencilInfo.back = {}; // Optional

        config.dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        config.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        config.dynamicStateInfo.pDynamicStates = config.dynamicStateEnables.data();
        config.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(config.dynamicStateEnables.size());
        config.dynamicStateInfo.flags = 0;

        config.bindingDescriptions = Model::Vertex::getBindingDescriptions();
        config.attributeDescriptions = Model::Vertex::getAttributeDescriptions();
    }

    void Pipeline::enableAlphaBlending(PipelineConfig& config) {
        config.colorBlendAttachment.blendEnable = VK_TRUE;
        config.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        config.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        config.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        config.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        config.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        config.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        config.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    }

    void Pipeline::bind(VkCommandBuffer commandBuffer) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);
    }

    std::vector<char> Pipeline::readFile(const std::string& path) {
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

    void Pipeline::createGraphicsPipeline(
        const std::string& vertPath,
        const std::string& fragPath,
        const PipelineConfig& config
    ) {
        assert(
            config.pipelineLayout != VK_NULL_HANDLE &&
            "Cannot create graphics pipeline:: no pipelinelayout provided in config"
        );
        assert(
            config.renderPass != VK_NULL_HANDLE &&
            "Cannot create graphics pipeline:: no renderPass provided in config"
        );

        auto vertCode = readFile(vertPath);
        auto fragCode = readFile(fragPath);

        createShaderModule(vertCode, &m_vertShaderModule);
        createShaderModule(fragCode, &m_fragShaderModule);

        VkPipelineShaderStageCreateInfo shaderStages[2];
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = m_vertShaderModule;
        shaderStages[0].pName = "main";
        shaderStages[0].flags = 0;
        shaderStages[0].pNext = nullptr;
        shaderStages[0].pSpecializationInfo = nullptr;

        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = m_fragShaderModule;
        shaderStages[1].pName = "main";
        shaderStages[1].flags = 0;
        shaderStages[1].pNext = nullptr;
        shaderStages[1].pSpecializationInfo = nullptr;

        auto& bindingDescriptions = config.bindingDescriptions;
        auto& attributeDescriptions = config.attributeDescriptions;
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
        pipelineInfo.pInputAssemblyState = &config.inputAssemblyInfo;
        pipelineInfo.pViewportState = &config.viewportInfo;
        pipelineInfo.pRasterizationState = &config.rasterizationInfo;
        pipelineInfo.pMultisampleState = &config.multisampleInfo;
        pipelineInfo.pColorBlendState = &config.colorBlendInfo;
        pipelineInfo.pDynamicState = &config.dynamicStateInfo;
        pipelineInfo.pDepthStencilState = &config.depthStencilInfo;


        pipelineInfo.layout = config.pipelineLayout;
        pipelineInfo.renderPass = config.renderPass;
        pipelineInfo.subpass = config.subpass;

        pipelineInfo.basePipelineIndex = -1;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(m_device.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline)
            != VK_SUCCESS) {
            throw std::runtime_error("Failed to create graphics pipeline!");
        }
    }

    void Pipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        if (vkCreateShaderModule(m_device.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create shader module!");
        }
    }
}
