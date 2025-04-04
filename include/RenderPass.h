#pragma once

#include "Common.h"
#include "VulkanContext.h"

class RenderPass {
public:
	static std::unique_ptr<RenderPass> createGbufferRenderPass(VulkanContext* context);
	~RenderPass();

	VkRenderPass getRenderPass() { return m_renderPass; }
private:
	VulkanContext* context;
	VkRenderPass m_renderPass;

	void initGbuffer(VulkanContext* context);
	void cleanup();
};