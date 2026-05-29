#pragma once

#include "PtBuffer.h"
#include "PtDevice.h"

#include <memory>
#include <vector>

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

            void loadModel(const std::string& path);
        };

        PtModel(PtDevice& device, const PtModel::Builder& builder);
        ~PtModel();

        PtModel(const PtModel&) = delete;
        PtModel& operator=(const PtModel&) = delete;

        static std::unique_ptr<PtModel> createModelFromFile(PtDevice& device, const std::string& path);

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

    private:
        void createVertexBuffers(const std::vector<Vertex>& vertices);
        void createIndexBuffers(const std::vector<uint32_t>& indices);

        PtDevice& m_device;

        std::unique_ptr<PtBuffer> m_vertexBuffer;
        uint32_t m_vertexCount;

        bool m_hasIndexBuffer = false;
        std::unique_ptr<PtBuffer> m_indexBuffer;
        uint32_t m_indexCount;
    };
}
