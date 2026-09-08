#pragma once

#include <glm/glm.hpp>

#include "RendererBackend.h"
#include "PalmTree/Logging/Log.h"

namespace PalmTree {
    struct Vertex {
        glm::vec3 Position{};
        glm::vec3 Color{};
        glm::vec3 Normal{};
        glm::vec2 Uv{};


        bool operator==(const Vertex& other) const {
            return Position == other.Position && Color == other.Color && Normal == other.Normal && Uv == other.Uv;
        }
    };

    class VertexBuffer {
    public:
        static VertexBuffer* Create(const std::vector<Vertex>& vertices);

        virtual ~VertexBuffer() = default;

        virtual uint32_t GetCount() const = 0;
    };

    class IndexBuffer {
    public:
        static IndexBuffer* Create(const std::vector<uint32_t>& indices);

        virtual ~IndexBuffer() = default;

        virtual uint32_t GetCount() const = 0;
    };

    class UniformBufferBase {
    public:
        virtual ~UniformBufferBase() = default;

        virtual void* GetPlatformBufferHandle() const = 0;
    };

    template<typename T>
    class UniformBuffer : public UniformBufferBase {
    public:
        static UniformBuffer* Create();

        UniformBuffer() {}

        virtual void WriteToBuffer(T* uniformObject) = 0;
        virtual void Flush() = 0;
    private:
        static UniformBuffer<T>* CreateVulkan();
    };

    template<typename T>
    UniformBuffer<T>* UniformBuffer<T>::Create() {
        switch (RendererBackend::GetAPI()) {
            case RendererBackend::API::VULKAN: return CreateVulkan();
            default: PT_CORE_ASSERT(false, "UniformBuffer is not supported on the current RendererBackend");
        }

        return nullptr;
    }
}
