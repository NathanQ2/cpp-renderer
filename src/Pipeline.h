#pragma once

#include <string>
#include <vector>

#include "Device.h"

namespace PalmTree {
    struct PipelineConfig {};
    
    class Pipeline {
    public:
        Pipeline(
            PalmTreeDevice& device,
            const std::string& vertPath,
            const std::string& fragPath,
            const PipelineConfig& info
        );

        ~Pipeline() = default;

        Pipeline(const Pipeline&) = delete;
        void operator=(const Pipeline) = delete;

        static PipelineConfig DefaultPipelineConfig(uint32_t width, uint32_t height);
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
