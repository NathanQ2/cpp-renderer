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
        CleanupFrameBuffer();
        
        CreateRenderPass();
        CreateFrameBuffer();
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

    void VulkanFrameBuffer::CreateRenderPass() {
        // VkAttachmentDescription depthAttachment{};
        // depthAttachment.format = FindDepthFormat();
        // depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        // depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        // depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        // depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        // depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        // depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        // depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        // VkAttachmentReference depthAttachmentRef{};
        // depthAttachmentRef.attachment = 1;
        // depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription colorAttachment = {};
        m_ColorFormat = FindColorFormat();
        colorAttachment.format = m_ColorFormat;
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
        // subpass.pDepthStencilAttachment = &depthAttachmentRef;

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

        // std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
        std::array<VkAttachmentDescription, 1> attachments = {colorAttachment};
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
        VkImageCreateInfo i{};
        i.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        i.imageType = VK_IMAGE_TYPE_2D;
        i.format = m_ColorFormat;
        i.extent = VkExtent3D { .width = m_Spec.Width, .height = m_Spec.Height };
        i.extent.depth = 1;
        i.mipLevels = 1;
        i.arrayLayers = 1;
        i.tiling = VK_IMAGE_TILING_OPTIMAL;
        i.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        i.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        
        PT_CORE_VERIFY(vkCreateImage(m_Device.GetDevice(), &i, nullptr, &m_Image) == VK_SUCCESS, "Failed to create image");
        
        VkImageViewCreateInfo iv{};
        iv.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        iv.image = m_Image;
        iv.viewType = VK_IMAGE_VIEW_TYPE_2D;
        iv.format = m_ColorFormat;
        
        PT_CORE_VERIFY(vkCreateImageView(m_Device.GetDevice(), &iv, nullptr, &m_ImageView) == VK_SUCCESS, "Failed to create image view");
        
        VkFramebufferCreateInfo fb{};
        fb.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fb.renderPass = m_RenderPass;
        fb.width = m_Spec.Width;
        fb.height = m_Spec.Height;
        fb.attachmentCount = 1;
        fb.pAttachments = &m_ImageView;
        
        PT_CORE_VERIFY(vkCreateFramebuffer(m_Device.GetDevice(), &fb, nullptr, &m_FrameBuffer) == VK_SUCCESS, "Failed to create frame buffer");
    }

    void VulkanFrameBuffer::CleanupRenderPass() {
        vkDestroyRenderPass(m_Device.GetDevice(), m_RenderPass, nullptr);
        
        m_RenderPass = VK_NULL_HANDLE;
    }
    
    void VulkanFrameBuffer::CleanupFrameBuffer() {
        vkDestroyFramebuffer(m_Device.GetDevice(), m_FrameBuffer, nullptr);
        vkDestroyImageView(m_Device.GetDevice(), m_ImageView, nullptr);
        vkDestroyImage(m_Device.GetDevice(), m_Image, nullptr);
        
        m_FrameBuffer = VK_NULL_HANDLE;
        m_ImageView = VK_NULL_HANDLE;
        m_Image = VK_NULL_HANDLE;
    }
} 
