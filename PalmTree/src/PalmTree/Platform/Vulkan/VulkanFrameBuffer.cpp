#include "VulkanFrameBuffer.h"

#include "PalmTree/Application.h"

namespace PalmTree {
    FrameBuffer* FrameBuffer::CreateVulkan(const FrameBufferSpecification& spec) {
        return new VulkanFrameBuffer(spec);
    }

    VulkanFrameBuffer::VulkanFrameBuffer(const FrameBufferSpecification& spec) : m_Spec(spec), m_Device(VulkanRendererBackend::Get()->GetDevice()) { VulkanFrameBuffer::Invalidate(); }
    
    VulkanFrameBuffer::~VulkanFrameBuffer() {
        CleanupRenderPass();
        CleanupFrameBuffer();
    }
    
    void VulkanFrameBuffer::Invalidate() {
        // TODO: Recreate RenderPass necessary?
        CleanupRenderPass();
        CleanupImage();
        CleanupDepthImage();
        CleanupFrameBuffer();
        CleanupSampler();
        
        CreateRenderPass();
        CreateImage();
        CreateDepthImage();
        CreateFrameBuffer();
        CreateSampler();
    }
    
    void VulkanFrameBuffer::Resize(uint32_t width, uint32_t height) {
        m_Spec.Width = width;
        m_Spec.Height = height;
        Invalidate();
    }

    VkFormat VulkanFrameBuffer::FindDepthFormat() {
        return m_Device.FindSupportedFormat(
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
    }

    VkFormat VulkanFrameBuffer::FindColorFormat() {
        return m_Device.FindSupportedFormat(
            { VK_FORMAT_B8G8R8A8_SRGB },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT
        );
    }

    ImTextureID VulkanFrameBuffer::CreateImTextureID() {
        return reinterpret_cast<ImTextureID>(ImGui_ImplVulkan_AddTexture(m_Sampler, m_ImageView, VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL));
    }

    void VulkanFrameBuffer::CreateRenderPass() {
        VkAttachmentDescription depthAttachment{};
        m_DepthImageFormat = FindDepthFormat();
        depthAttachment.format = m_DepthImageFormat; 
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription colorAttachment = {};
        m_ImageFormat = FindColorFormat();
        colorAttachment.format = m_ImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        VkSubpassDependency dependency = {};

        dependency.dstSubpass = 0; // For our current render pass
        dependency.dstAccessMask =
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        // we write access our color attachment
        dependency.dstStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        // at color attachment output stage

        // only after
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL; // the previous subpass
        dependency.srcAccessMask = 0; // does whatever
        dependency.srcStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        // at its color attachment output stage

        // i.e this means we can't write to our color attachment until after the previous render pass has
        // finished with its color attachment stage

        std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(m_Device.GetDevice(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS) {
            PT_CORE_VERIFY(false, "Failed to create render pass!");
        }
    }
    
    void VulkanFrameBuffer::CreateFrameBuffer() {
        VkFramebufferCreateInfo fb{};
        fb.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fb.renderPass = m_RenderPass;
        fb.width = m_Spec.Width;
        fb.height = m_Spec.Height;
        fb.attachmentCount = 2;
        std::array<VkImageView, 2> attachments{ m_ImageView, m_DepthImageView };
        fb.pAttachments = attachments.data();
        fb.layers = 1;
        
        PT_CORE_VERIFY(vkCreateFramebuffer(m_Device.GetDevice(), &fb, nullptr, &m_FrameBuffer) == VK_SUCCESS, "Failed to create frame buffer");
    }

    void VulkanFrameBuffer::CreateImage() {
        VkImageCreateInfo i{};
        i.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        i.imageType = VK_IMAGE_TYPE_2D;
        i.format = m_ImageFormat;
        i.extent = VkExtent3D { .width = m_Spec.Width, .height = m_Spec.Height };
        i.extent.depth = 1;
        i.mipLevels = 1;
        i.arrayLayers = 1;
        i.tiling = VK_IMAGE_TILING_OPTIMAL;
        i.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        i.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        i.samples = VK_SAMPLE_COUNT_1_BIT;
        
        // m_Device.CreateImageWithInfo(i, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_Image, m_ImageMemory);
        
        PT_CORE_VERIFY(vkCreateImage(m_Device.GetDevice(), &i, nullptr, &m_Image) == VK_SUCCESS, "Failed to create image");
        
        VkMemoryRequirements mr;
        vkGetImageMemoryRequirements(m_Device.GetDevice(), m_Image, &mr);
        
        VkMemoryAllocateInfo ai{};
        ai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        ai.allocationSize = mr.size;
        ai.memoryTypeIndex = m_Device.FindMemoryType(mr.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        
        PT_CORE_VERIFY(vkAllocateMemory(m_Device.GetDevice(), &ai, nullptr, &m_ImageMemory) == VK_SUCCESS, "Failed to allocate image memory");
        PT_CORE_VERIFY(vkBindImageMemory(m_Device.GetDevice(), m_Image, m_ImageMemory, 0) == VK_SUCCESS, "Failed to bind image memory");
        
        VkImageSubresourceRange isr{};
        isr.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        isr.baseArrayLayer = 0;
        isr.layerCount = 1;
        isr.baseMipLevel = 0;
        isr.levelCount = 1;
        
        VkImageViewCreateInfo iv{};
        iv.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        iv.pNext = nullptr;
        iv.image = m_Image;
        iv.viewType = VK_IMAGE_VIEW_TYPE_2D;
        iv.format = m_ImageFormat;
        iv.subresourceRange = isr;
        
        VkDevice device = m_Device.GetDevice();
        PT_CORE_VERIFY(vkCreateImageView(device, &iv, nullptr, &m_ImageView) == VK_SUCCESS, "Failed to create image view");
    }
    
    void VulkanFrameBuffer::CreateDepthImage() {
        VkImageCreateInfo i{};
        i.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        i.imageType = VK_IMAGE_TYPE_2D;
        i.format = m_DepthImageFormat;
        i.extent = VkExtent3D { .width = m_Spec.Width, .height = m_Spec.Height };
        i.extent.depth = 1;
        i.mipLevels = 1;
        i.arrayLayers = 1;
        i.tiling = VK_IMAGE_TILING_OPTIMAL;
        i.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        i.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        i.samples = VK_SAMPLE_COUNT_1_BIT;
        
        PT_CORE_VERIFY(vkCreateImage(m_Device.GetDevice(), &i, nullptr, &m_DepthImage) == VK_SUCCESS, "Failed to create image");
        
        VkMemoryRequirements mr;
        vkGetImageMemoryRequirements(m_Device.GetDevice(), m_DepthImage, &mr);
        
        VkMemoryAllocateInfo ai{};
        ai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        ai.allocationSize = mr.size;
        ai.memoryTypeIndex = m_Device.FindMemoryType(mr.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        
        PT_CORE_VERIFY(vkAllocateMemory(m_Device.GetDevice(), &ai, nullptr, &m_DepthImageMemory) == VK_SUCCESS, "Failed to allocate image memory");
        PT_CORE_VERIFY(vkBindImageMemory(m_Device.GetDevice(), m_DepthImage, m_DepthImageMemory, 0) == VK_SUCCESS, "Failed to bind image memory");
        
        VkImageSubresourceRange isr{};
        isr.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        isr.baseArrayLayer = 0;
        isr.layerCount = 1;
        isr.baseMipLevel = 0;
        isr.levelCount = 1;
        
        VkImageViewCreateInfo iv{};
        iv.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        iv.pNext = nullptr;
        iv.image = m_DepthImage;
        iv.viewType = VK_IMAGE_VIEW_TYPE_2D;
        iv.format = m_DepthImageFormat;
        iv.subresourceRange = isr;
        
        VkDevice device = m_Device.GetDevice();
        PT_CORE_VERIFY(vkCreateImageView(device, &iv, nullptr, &m_DepthImageView) == VK_SUCCESS, "Failed to create image view");
    }

    void VulkanFrameBuffer::CreateSampler() {
        VkSamplerCreateInfo s{};
        s.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        s.magFilter = VK_FILTER_LINEAR;
        s.minFilter = VK_FILTER_LINEAR;
        s.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        s.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        s.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        s.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        s.anisotropyEnable = VK_FALSE;
        s.compareEnable = VK_FALSE;
        
        vkCreateSampler(m_Device.GetDevice(), &s, nullptr, &m_Sampler);
    }
    
    void VulkanFrameBuffer::CleanupSampler() {
        vkDestroySampler(m_Device.GetDevice(), m_Sampler, nullptr);
        
        m_Sampler = VK_NULL_HANDLE;
    }

    void VulkanFrameBuffer::CleanupImage() {
        vkDestroyImageView(m_Device.GetDevice(), m_ImageView, nullptr);
        vkDestroyImage(m_Device.GetDevice(), m_Image, nullptr);
        
        m_ImageView = VK_NULL_HANDLE;
        m_Image = VK_NULL_HANDLE;
    }
    
    void VulkanFrameBuffer::CleanupDepthImage() {
        vkDestroyImageView(m_Device.GetDevice(), m_DepthImageView, nullptr);
        vkDestroyImage(m_Device.GetDevice(), m_DepthImage, nullptr);
        
        m_DepthImageView = VK_NULL_HANDLE;
        m_DepthImage = VK_NULL_HANDLE;
    }

    void VulkanFrameBuffer::CleanupRenderPass() {
        vkDestroyRenderPass(m_Device.GetDevice(), m_RenderPass, nullptr);
        
        m_RenderPass = VK_NULL_HANDLE;
    }
    
    void VulkanFrameBuffer::CleanupFrameBuffer() {
        vkDestroyFramebuffer(m_Device.GetDevice(), m_FrameBuffer, nullptr);
        
        m_FrameBuffer = VK_NULL_HANDLE;
    }
} 
