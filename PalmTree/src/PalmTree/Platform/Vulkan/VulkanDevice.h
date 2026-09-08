#pragma once

#include "../../Window.h"


namespace PalmTree {
    struct VulkanSwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentModes;
    };

    struct VulkanQueueFamilyIndices {
        uint32_t GraphicsFamily;
        uint32_t PresentFamily;
        bool GraphicsFamilyHasValue = false;
        bool PresentFamilyHasValue = false;
        bool IsComplete() { return GraphicsFamilyHasValue && PresentFamilyHasValue; }
    };

    class VulkanDevice {
    public:
#ifdef PT_DEBUG
        const bool EnableValidationLayers = true;
#else
        const bool EnableValidationLayers = false;
#endif

        VulkanDevice();
        ~VulkanDevice();

        // Not copyable or movable
        VulkanDevice(const VulkanDevice&) = delete;
        VulkanDevice& operator=(const VulkanDevice&) = delete;
        VulkanDevice(VulkanDevice&&) = delete;
        VulkanDevice& operator=(VulkanDevice&&) = delete;

        VkInstance GetInstance() const { return m_Instance; }
        VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
        VkCommandPool GetCommandPool() const { return m_CommandPool; }
        VkDevice GetDevice() const { return m_Device; }
        VkSurfaceKHR Surface() const { return m_Surface; }
        VkQueue GraphicsQueue() const { return m_GraphicsQueue; }
        VkQueue PresentQueue() const { return m_PresentQueue; }

        VulkanSwapChainSupportDetails GetSwapChainSupport() { return QuerySwapChainSupport(m_PhysicalDevice); }
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        VulkanQueueFamilyIndices FindPhysicalQueueFamilies() { return FindQueueFamilies(m_PhysicalDevice); }
        VkFormat FindSupportedFormat(
            const std::vector<VkFormat>& candidates,
            VkImageTiling tiling,
            VkFormatFeatureFlags features
        );

        // Buffer Helper Functions
        void CreateBuffer(
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties,
            VkBuffer& buffer,
            VkDeviceMemory& bufferMemory
        );
        VkCommandBuffer BeginSingleTimeCommands();
        void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void CopyBufferToImage(
            VkBuffer buffer,
            VkImage image,
            uint32_t width,
            uint32_t height,
            uint32_t layerCount
        );

        void CreateImageWithInfo(
            const VkImageCreateInfo& imageInfo,
            VkMemoryPropertyFlags properties,
            VkImage& image,
            VkDeviceMemory& imageMemory
        );

        // TODO: Make this private
        VkPhysicalDeviceProperties m_Properties;
    private:
        void CreateInstance();
        void SetupDebugMessenger();
        void CreateSurface();
        void PickPhysicalDevice();
        void CreateLogicalDevice();
        void CreateCommandPool();

        // helper functions
        bool IsDeviceSuitable(VkPhysicalDevice device);
        std::vector<const char*> GetRequiredExtensions();
        bool CheckValidationLayerSupport();
        VulkanQueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
        void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        void HasGlfwRequiredInstanceExtensions();
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
        VulkanSwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

        VkInstance m_Instance;
        VkDebugUtilsMessengerEXT m_DebugMessenger;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkCommandPool m_CommandPool;

        VkDevice m_Device;
        VkSurfaceKHR m_Surface;
        VkQueue m_GraphicsQueue;
        VkQueue m_PresentQueue;

        const std::vector<const char*> m_ValidationLayers = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char*> m_DeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
        };
    };
}
