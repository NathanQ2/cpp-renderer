#include "PtModel.h"

#include "PtUtils.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <cassert>
#include <unordered_map>

namespace std {
    template<>
    struct hash<PalmTree::PtModel::Vertex> {
        size_t operator()(PalmTree::PtModel::Vertex const& vertex) const {
            size_t seed = 0;
            PalmTree::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
            
            return seed;
        }
    };
}

namespace PalmTree {
    std::vector<VkVertexInputBindingDescription> PtModel::Vertex::getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> PtModel::Vertex::getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        
        attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
        attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
        attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
        attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

        return attributeDescriptions;
    }

    PtModel::PtModel(PtDevice& device, const PtModel::Builder& builder) : m_Device(device) {
        CreateVertexBuffers(builder.vertices);
        CreateIndexBuffers(builder.indices);
    }

    PtModel::~PtModel() {}

    std::unique_ptr<PtModel> PtModel::CreateModelFromFile(PtDevice& device, const std::string& path) {
        Builder builder{};
        
        builder.LoadModel(path);
        
        return std::make_unique<PtModel>(device, builder);
    }

    void PtModel::Bind(VkCommandBuffer commandBuffer) {
        VkBuffer buffers[] = { m_VertexBuffer->getBuffer() };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        
        
        if (m_HasIndexBuffer) {
            vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
        }
    }

    void PtModel::Draw(VkCommandBuffer commandBuffer) {
        if (m_HasIndexBuffer) {
            vkCmdDrawIndexed(commandBuffer, m_IndexCount, 1, 0, 0, 0);
        }
        else {
            vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
        }
    }

    void PtModel::CreateVertexBuffers(const std::vector<Vertex>& vertices) {
        m_VertexCount = static_cast<uint32_t>(vertices.size());
        assert(m_VertexCount >= 3 && "Vertex count must be at least 3");
        
        VkDeviceSize bufferSize = sizeof(vertices[0]) * m_VertexCount;
        uint32_t vertexSize = sizeof(vertices[0]);
        
        PtBuffer stagingBuffer = PtBuffer(
            m_Device, 
            vertexSize, 
            m_VertexCount, 
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
        
        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void*)vertices.data());
        
        m_VertexBuffer = std::make_unique<PtBuffer>(
            m_Device,
            vertexSize,
            m_VertexCount, 
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );
        
        m_Device.copyBuffer(stagingBuffer.getBuffer(), m_VertexBuffer->getBuffer(), bufferSize);
    }
    
    void PtModel::CreateIndexBuffers(const std::vector<uint32_t>& indices) {
        m_IndexCount = static_cast<uint32_t>(indices.size());
        m_HasIndexBuffer = m_IndexCount > 0;
        
        if (!m_HasIndexBuffer) return;
        
        VkDeviceSize bufferSize = sizeof(indices[0]) * m_IndexCount;
        uint32_t indexSize = sizeof(indices[0]);
        
        PtBuffer stagingBuffer = PtBuffer(
            m_Device,
            indexSize,
            m_IndexCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
        
        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void*)indices.data());
        
        m_IndexBuffer = std::make_unique<PtBuffer>(
            m_Device,
            indexSize,
            m_IndexCount,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );
        
        m_Device.copyBuffer(stagingBuffer.getBuffer(), m_IndexBuffer->getBuffer(), bufferSize);
    }

    void PtModel::Builder::LoadModel(const std::string& path) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, error;
        
        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &error, path.c_str())) {
            throw std::runtime_error(warn + error);
        }
        
        vertices.clear();
        indices.clear();
        
        std::unordered_map<Vertex, uint32_t> uniqueVertices{};
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex{};
                
                if (index.vertex_index >= 0) {
                    vertex.position = { 
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };
                    
                    vertex.color = { 
                        attrib.colors[3 * index.vertex_index + 0],
                        attrib.colors[3 * index.vertex_index + 1],
                        attrib.colors[3 * index.vertex_index + 2]
                    };
                }
                
                if (index.normal_index >= 0) {
                    vertex.normal = { 
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    };
                }
                
                if (index.texcoord_index >= 0) {
                    vertex.uv = { 
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1],
                    };
                }
                
                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }
                
                indices.push_back(uniqueVertices[vertex]);
            }
        }
    }
}
