#include "Pipeline.h"

#include <fstream>
#include <iostream>
#include <stdexcept>

namespace PalmTree {
    Pipeline::Pipeline(
        PalmTreeDevice& device,
        const std::string& vertPath,
        const std::string& fragPath,
        const PipelineConfig& config
    ) : m_Device {device} {
        CreateGraphicsPipeline(vertPath, fragPath, config);
    }

    PipelineConfig Pipeline::DefaultPipelineConfig(uint32_t width, uint32_t height) {
        PipelineConfig config{};

        return config;
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

    void Pipeline::CreateGraphicsPipeline(const std::string& vertPath, const std::string& fragPath, const PipelineConfig& config) {
        auto vertCode = ReadFile(vertPath);
        auto fragCode = ReadFile(fragPath);

        std::cout << "Vertex Size: " << vertCode.size() << std::endl;
        std::cout << "Fragment Size: " << fragCode.size() << std::endl;
    }

    void Pipeline::CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        if (vkCreateShaderModule(m_Device.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create shader module!");
        }
    }
}
