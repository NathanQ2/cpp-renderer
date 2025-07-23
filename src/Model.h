#pragma once

#include "Device.h"

#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace PalmTree {
    class Model {
    public:
        struct Vertex {
            glm::vec2 position;

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };
        
        Model(PalmTreeDevice& device, const std::vector<Vertex>& vertices);
        ~Model();

        Model(const Model&) = delete;
        Model &operator=(const Model&) = delete;

        void Bind(VkCommandBuffer commandBuffer);
        void Draw(VkCommandBuffer commandBuffer);
        
    private:
        void CreateVertexBuffers(const std::vector<Vertex>& vertices);
        
        PalmTreeDevice& m_Device;

        VkBuffer m_VertexBuffer;
        VkDeviceMemory m_VertexBufferMemory;
        uint32_t m_VertexCount;
    };
}
