#include "include/RenderPass.h"

RenderPass::~RenderPass() {
	cleanup();
}

void RenderPass::cleanup() {
	std::cout << "RenderPass::cleanup" << std::endl;
	if (m_renderPass != VK_NULL_HANDLE) {
		vkDestroyRenderPass(context->getDevice(), m_renderPass, nullptr);
		m_renderPass = VK_NULL_HANDLE;
	}
}

std::unique_ptr<RenderPass> RenderPass::createGbufferRenderPass(VulkanContext* context) {
	std::unique_ptr<RenderPass> renderPass = std::unique_ptr<RenderPass>(new RenderPass());
	renderPass->initGbuffer(context);
	return renderPass;
}

void RenderPass::initGbuffer(VulkanContext* context) {
	this->context = context;

	std::vector<VkAttachmentDescription> attachments(5);

	// Position
	attachments[0].format = VK_FORMAT_R16G16B16A16_SFLOAT;
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// Normal
	attachments[1] = attachments[0];
	attachments[1].format = VK_FORMAT_R16G16B16A16_SFLOAT;

	// Albedo
	attachments[2] = attachments[0];
	attachments[2].format = VK_FORMAT_R8G8B8A8_UNORM;

	// PBR
	attachments[3] = attachments[0];
	attachments[3].format = VK_FORMAT_R8G8B8A8_UNORM;

	// Depth
	attachments[4].format = VK_FORMAT_D32_SFLOAT;
	attachments[4].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[4].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[4].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[4].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[4].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[4].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[4].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// Attachment References
	std::array<VkAttachmentReference, 4> colorRefs{};
	for (uint32_t i = 0; i < colorRefs.size(); ++i) {
		colorRefs[i].attachment = i;
		colorRefs[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}

	VkAttachmentReference depthRef{};
	depthRef.attachment = 4;
	depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// Subpass
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = static_cast<uint32_t>(colorRefs.size());
	subpass.pColorAttachments = colorRefs.data();
	subpass.pDepthStencilAttachment = &depthRef;

	// Dependency
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	// RenderPass Info
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(context->getDevice(), &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create G-buffer render pass!");
	}
}