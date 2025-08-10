#pragma once

#include "PtDevice.h"

#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace PalmTree {
    class PtModel {
    public:
        struct Vertex {
            glm::vec2 position;
            glm::vec3 color;

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };
        
        PtModel(PtDevice& device, const std::vector<Vertex>& vertices);
        ~PtModel();

        PtModel(const PtModel&) = delete;
        PtModel &operator=(const PtModel&) = delete;

        void Bind(VkCommandBuffer commandBuffer);
        void Draw(VkCommandBuffer commandBuffer);
        
    private:
        void CreateVertexBuffers(const std::vector<Vertex>& vertices);
        
        PtDevice& m_Device;

        VkBuffer m_VertexBuffer;
        VkDeviceMemory m_VertexBufferMemory;
        uint32_t m_VertexCount;
    };
}
