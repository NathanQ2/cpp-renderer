#include "ptpch.h"
#include "Model.h"

#include "Utils.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "Logging/Log.h"

#include "PalmTree/Renderer/Buffer.h"


namespace std {
    template<>
    struct hash<PalmTree::Vertex> {
        size_t operator()(PalmTree::Vertex const& vertex) const {
            size_t seed = 0;
            PalmTree::HashCombine(seed, vertex.Position, vertex.Color, vertex.Normal, vertex.Uv);

            return seed;
        }
    };
}

namespace PalmTree {
    void Model::Builder::LoadModel(const std::string& path) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, error;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &error, path.c_str())) {
            throw std::runtime_error(warn + error);
        }

        Vertices.clear();
        Indices.clear();

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex{};

                if (index.vertex_index >= 0) {
                    vertex.Position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };

                    vertex.Color = {
                        attrib.colors[3 * index.vertex_index + 0],
                        attrib.colors[3 * index.vertex_index + 1],
                        attrib.colors[3 * index.vertex_index + 2]
                    };
                }

                if (index.normal_index >= 0) {
                    vertex.Normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    };
                }

                if (index.texcoord_index >= 0) {
                    vertex.Uv = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1],
                    };
                }

                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(Vertices.size());
                    Vertices.push_back(vertex);
                }

                Indices.push_back(uniqueVertices[vertex]);
            }
        }
    }

    Model::Model(const Builder& builder) {
        CreateVertexBuffers(builder.Vertices);
        CreateIndexBuffers(builder.Indices);
    }

    Model::~Model() {}

    std::unique_ptr<Model> Model::CreateModelFromFile(const std::string& path) {
        Builder builder{};

        builder.LoadModel(path);

        return std::make_unique<Model>(builder);
    }

    uint32_t Model::GetVertexCount() const { return m_VertexBuffer->GetCount(); }
    uint32_t Model::GetIndexCount() const { return m_IndexBuffer->GetCount(); }

    void Model::CreateVertexBuffers(const std::vector<Vertex>& vertices) {
        PT_CORE_ASSERT(vertices.size() >= 3, "Vertex count must be at least 3");

        m_VertexBuffer = std::unique_ptr<VertexBuffer>(VertexBuffer::Create(vertices));
    }

    void Model::CreateIndexBuffers(const std::vector<uint32_t>& indices) {
        m_HasIndexBuffer = indices.size() > 0;
        if (!m_HasIndexBuffer) return;

        m_IndexBuffer = std::unique_ptr<IndexBuffer>(IndexBuffer::Create(indices));
    }
}
