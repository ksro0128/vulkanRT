#include "include/FrameBuffer.h"

FrameBuffer::~FrameBuffer() {
	cleanup();
}

void FrameBuffer::cleanup() {
	std::cout << "FrameBuffer::cleanup" << std::endl;
	if (m_frameBuffer != VK_NULL_HANDLE) {
		vkDestroyFramebuffer(context->getDevice(), m_frameBuffer, nullptr);
		m_frameBuffer = VK_NULL_HANDLE;
	}
}

std::unique_ptr<FrameBuffer> FrameBuffer::createGbufferFrameBuffer(VulkanContext* context, RenderPass* renderPass, GbufferAttachment& gBufferAttachment, VkExtent2D extent) {
	std::unique_ptr<FrameBuffer> frameBuffer = std::unique_ptr<FrameBuffer>(new FrameBuffer());
	frameBuffer->initGbuffer(context, renderPass, gBufferAttachment, extent);
	return frameBuffer;
}

void FrameBuffer::initGbuffer(VulkanContext* context, RenderPass* renderPass, GbufferAttachment& gBufferAttachment, VkExtent2D extent) {
    this->context = context;

    std::array<VkImageView, 5> attachments = {
        gBufferAttachment.position->getImageView(),
        gBufferAttachment.normal->getImageView(),
        gBufferAttachment.albedo->getImageView(),
        gBufferAttachment.pbr->getImageView(),
        gBufferAttachment.depth->getImageView()
    };

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass->getRenderPass();
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(context->getDevice(), &framebufferInfo, nullptr, &m_frameBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create G-buffer framebuffer!");
    }
}
