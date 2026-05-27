#pragma once

#include "PtDevice.h"
#include "PtBuffer.h"

#include <vector>
#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace PalmTree {
    class PtModel {
    public:
        struct Vertex {
            glm::vec3 position{};
            glm::vec3 color{};
            glm::vec3 normal{};
            glm::vec2 uv{};

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
            
            bool operator==(const Vertex& other) const {
                return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
            }
        };
        
        struct Builder {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};
            
            void LoadModel(const std::string& path);
        };
        
        PtModel(PtDevice& device, const PtModel::Builder& builder);
        ~PtModel();

        PtModel(const PtModel&) = delete;
        PtModel &operator=(const PtModel&) = delete;
        
        static std::unique_ptr<PtModel> CreateModelFromFile(PtDevice& device, const std::string& path);

        void Bind(VkCommandBuffer commandBuffer);
        void Draw(VkCommandBuffer commandBuffer);
        
    private:
        void CreateVertexBuffers(const std::vector<Vertex>& vertices);
        void CreateIndexBuffers(const std::vector<uint32_t>& indices);
        
        PtDevice& m_Device;

        std::unique_ptr<PtBuffer> m_VertexBuffer;
        uint32_t m_VertexCount;
        
        bool m_HasIndexBuffer = false;
        std::unique_ptr<PtBuffer> m_IndexBuffer;
        uint32_t m_IndexCount;
    };
}
