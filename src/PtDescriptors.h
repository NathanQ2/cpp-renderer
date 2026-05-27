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
            Builder(PtDevice& device) : ptDevice{device} {
            }

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<PtDescriptorSetLayout> build() const;

        private:
            PtDevice& ptDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        PtDescriptorSetLayout(
            PtDevice& ptDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~PtDescriptorSetLayout();
        PtDescriptorSetLayout(const PtDescriptorSetLayout&) = delete;
        PtDescriptorSetLayout& operator=(const PtDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        PtDevice& ptDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class PtDescriptorWriter;
    };

    class PtDescriptorPool {
    public:
        class Builder {
        public:
            Builder(PtDevice& device) : ptDevice{device} {
            }

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<PtDescriptorPool> build() const;

        private:
            PtDevice& ptDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        PtDescriptorPool(
            PtDevice& ptDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~PtDescriptorPool();
        PtDescriptorPool(const PtDescriptorPool&) = delete;
        PtDescriptorPool& operator=(const PtDescriptorPool&) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        PtDevice& ptDevice;
        VkDescriptorPool descriptorPool;

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
        PtDescriptorSetLayout& setLayout;
        PtDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };
}
