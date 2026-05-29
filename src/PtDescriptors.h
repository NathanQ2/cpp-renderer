#pragma once

#include "PtDevice.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace PalmTree {
    class PtDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(PtDevice& device) : m_device{device} {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1
            );
            std::unique_ptr<PtDescriptorSetLayout> build() const;

        private:
            PtDevice& m_device;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings{};
        };

        PtDescriptorSetLayout(
            PtDevice& ptDevice,
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings
        );
        ~PtDescriptorSetLayout();
        PtDescriptorSetLayout(const PtDescriptorSetLayout&) = delete;
        PtDescriptorSetLayout& operator=(const PtDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return m_descriptorSetLayout; }

    private:
        PtDevice& m_device;
        VkDescriptorSetLayout m_descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings;

        friend class PtDescriptorWriter;
    };

    class PtDescriptorPool {
    public:
        class Builder {
        public:
            Builder(PtDevice& device) : m_device{device} {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<PtDescriptorPool> build() const;

        private:
            PtDevice& m_device;
            std::vector<VkDescriptorPoolSize> m_poolSizes{};
            uint32_t m_maxSets = 1000;
            VkDescriptorPoolCreateFlags m_poolFlags = 0;
        };

        PtDescriptorPool(
            PtDevice& ptDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes
        );
        ~PtDescriptorPool();
        PtDescriptorPool(const PtDescriptorPool&) = delete;
        PtDescriptorPool& operator=(const PtDescriptorPool&) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout,
            VkDescriptorSet& descriptor
        ) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        PtDevice& m_device;
        VkDescriptorPool m_descriptorPool;

        friend class PtDescriptorWriter;
    };

    class PtDescriptorWriter {
    public:
        PtDescriptorWriter(PtDescriptorSetLayout& setLayout, PtDescriptorPool& pool);

        PtDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        PtDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        PtDescriptorSetLayout& m_setLayout;
        PtDescriptorPool& m_pool;
        std::vector<VkWriteDescriptorSet> m_writes;
    };
}
